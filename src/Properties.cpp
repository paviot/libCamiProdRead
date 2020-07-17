/*
 * GetProperties.cpp
 *
 *  Created on: Jul 5, 2019
 *      Author: paviot
 */

#include "Properties.h"

#include <string>
#include <iostream>
#include <vector>
#include <utility>

#include <stdlib.h>

#include <boost/variant.hpp>

#include "XML.h"

using namespace std;

class add2Formula : public boost::static_visitor<Formula>
{
private:
	const Formula f;
public:
	add2Formula(const Formula &_f):static_visitor(), f(_f){}

	Formula operator()(int) const
	{
		cerr << "add2Formula: A formula shouldn't end with an integer while constructing" << endl;
		exit(1);
	}

	Formula operator()(const string &s) const
	{
		vector<Formula> result;

		result.push_back(s);
		result.push_back(f);
		return result;
	}

	Formula operator()(const vector<Formula> &v) const
	{
		vector<Formula> result = v;

		result.push_back(f);
		return result;
	}
};

class formula2vector : public boost::static_visitor<vector<Formula> >
{
private:
public:
	vector<Formula> operator()(int) const
	{
		cerr << "formula2vector: A formula shouldn't be an integer while conversion" << endl;
		exit(1);
	}

	vector<Formula> operator()(const string) const
	{
		cerr << "formula2vector: A formula shouldn't be a string while conversion" << endl;
		exit(1);
	}

	vector<Formula> operator()(const vector<Formula> &v) const
	{
		return v;
	}
};

class getString : public boost::static_visitor<string>
{
public:
	string operator()(int) const
	{
		cerr << "A formula shouldn't begin with an integer" << endl;
		exit(1);
	}

	string operator()(const string& s) const
	{
		return s;
	}

	string operator()(const vector<Formula>&) const
	{
		cerr << "A formula shouldn't begin with a vector" << endl;
		exit(1);
	}
};

class isComplete : public boost::static_visitor<bool>
{
public:
	bool operator()(int) const
	{
		cerr << "isComplete: A formula shouldn't end with an integer while constructing" << endl;
		exit(1);
	}

	bool operator()(const string&) const
	{
		return false;
	}

	bool operator()(const vector<Formula> &v) const
	{
		string key = boost::apply_visitor(getString(), v[0]);

		if ((key.compare("disjunction") == 0) || (key.compare("conjunction") == 0) ||
			(key.compare("integer-le") == 0) || (key.compare("until") == 0))
			return (v.size() == 3);
		else if ((key.compare("all-paths") == 0) || (key.compare("finally") == 0) ||
				 (key.compare("next") == 0) || (key.compare("integer-constant") == 0) ||
				 (key.compare("tokens-count") == 0) || (key.compare("negation") == 0) ||
				 (key.compare("globally") == 0) || (key.compare("exists-path") == 0) ||
				 (key.compare("before") == 0) || (key.compare("reach") == 0) ||
				 (key.compare("is-fireable") == 0)|| (key.compare("place-bound") == 0))
			return true;
		else
		{
			cerr << "Unrecognized operator: " << key << endl;
			exit(1);
		}
	}
};

class printFormula : public boost::static_visitor<void>
{
public:
	void operator()(int i) const
	{
		cout << ' ' << i;
	}

	void operator()(const string &s) const
	{
		cout << ' ' << s;
	}

	void operator()(const vector<Formula> &f) const
	{
		cout << '(';

		for (auto it=f.begin(); it != f.end(); ++it)
			boost::apply_visitor(printFormula(), *it);

		cout << " )";
	}
};

Properties::Properties(const char *f) {
	XML basic(f);
	string elem = basic.getNextElement();

	if (elem.compare("?xml") != 0)
	{
		cerr << "Reading a property: no ?xml element, found: " << elem << endl;
		exit(1);
	}

	elem = basic.getNextElement();

	if (elem.compare("property-set") != 0)
	{
		cerr << "Reading a property: no property-set element, found: " << elem << endl;
		exit(1);
	}

	elem = basic.getNextElement();

	if (elem.compare("property") != 0)
	{
		cerr << "Reading a property: a property does not begin with property element, found: " << elem << endl;
		exit(1);
	}

	while (! basic.isFinished())
	{
		vector<Formula> f;
		string			name;

		elem = basic.getNextElement();

		if (elem.compare("id") != 0)
		{
			cerr << "Reading a property: a property with no id element, found: " << elem << endl;
			exit(1);
		}

		name = basic.getToken();
//		cout << "Identité de la formule : " << name << endl;
		elem = basic.getNextElement();

		if (elem.compare("description") != 0)
		{
			cerr << "Reading a property: a property with no description element, found: " << elem << endl;
			exit(1);
		}

		elem = basic.getNextElement();

		if (elem.compare("formula") != 0)
		{
			cerr << "Reading a property: a property with no formula element, found: " << elem << endl;
			exit(1);
		}


		elem = basic.getToken();

		while ((! basic.isFinished()) && (elem.compare("property") != 0))
		{
			if ((elem.compare("tokens-count") == 0) || (elem.compare("place-bound") == 0))
			{
				vector<Formula>	tmp;

				tmp.push_back(elem);
				elem = basic.getNextElement();

				while ((! basic.isFinished()) && (elem.compare("place") == 0))
				{
					elem = basic.getToken();
					tmp.push_back(elem);
					elem = basic.getNextElement();
				}

				addTerminalFormula(f,tmp);
			}
			else if (elem.compare("is-fireable") == 0)
			{
				vector<Formula>	tmp;

				tmp.push_back(elem);
				elem = basic.getNextElement();

				while ((! basic.isFinished()) && (elem.compare("transition") == 0))
				{
					elem = basic.getToken();
					tmp.push_back(elem);
					elem = basic.getNextElement();
				}

				addTerminalFormula(f,tmp);
			}
			else if (elem.compare("integer-constant") == 0)
			{
				vector<Formula> tmp;

				tmp.push_back(elem);
				elem = basic.getToken();
				tmp.push_back(stoi(elem));
				addTerminalFormula(f,tmp);
				elem = basic.getNextElement();
			}
			else
			{
				f.push_back(elem);
				elem = basic.getNextElement();
			}
		}
		prop.push_back(pair<string,Formula>(name,f));
//		boost::apply_visitor(printFormula(), (Formula)f);
//		cout << endl;
	}
}

Properties::~Properties() {
	// TODO Auto-generated destructor stub
}

void Properties::addTerminalFormula(vector<Formula> &f, const vector<Formula> &term)
{
	bool finished = false;
	Formula toAdd = term;

//	cout << "DEBUG: entrée de addTerminalFormula" << endl;
//	boost::apply_visitor(printFormula(), (Formula)f);
//	cout << endl;

	while ((f.size() > 0) && (!finished))
	{
		Formula last = f.back();

		f.pop_back();
		toAdd = boost::apply_visitor(add2Formula(toAdd), last);
		finished = ! boost::apply_visitor(isComplete(), toAdd);
//		cout << "DEBUG: boucle de addTerminalFormula" << endl;
//		boost::apply_visitor(printFormula(), toAdd);
//		cout << endl;
	}

	if (f.size() > 0)
		f.push_back(toAdd);
	else
		f = boost::apply_visitor(formula2vector(),toAdd);
}

void Properties::print() const
{
	for (auto it=prop.begin(); it != prop.end(); ++it)
	{
		cout << "Property: " << it->first << endl;
		boost::apply_visitor(printFormula(), it->second);
		cout << endl;
	}
}

