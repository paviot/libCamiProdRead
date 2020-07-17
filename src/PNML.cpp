/*
 * PNML.cpp
 *
 *  Created on: Jun 25, 2019
 *      Author: paviot
 */

#include <iostream>
#include <unordered_map>
#include <forward_list>
#include <string.h>
#include <tuple>

#include "PNML.h"
#include "StoreNet.h"
#include "XML.h"

using namespace std;
using namespace CamiProdRead;

PNML::PNML(const char *f, StoreNet *N): basic(f)
{
	unsigned int id = 0;
	unordered_map<string, unsigned int> nd;
	forward_list<tuple<string,string,unsigned int> > remainders;

	string elem, attr;

	findElement("?xml");
	findElement("pnml");
	findElement("net");
	basic.getNextAttribute(elem, attr);
	basic.getNextAttribute(elem, attr);

	if (elem.compare("type") != 0)
	{
		cerr << "Reading PNML file: attribute type missing for net element" << endl;
		exit(1);
	}

	if (attr.compare("http://www.pnml.org/version-2009/grammar/ptnet") != 0)
	{
		cerr << "Reading PNML file: wrong PNML version" << endl;
		exit(1);
	}

	findElement("page");

	bool noGet = false;

	while (!basic.isFinished())
	{
		unsigned int	sc, dst;

		if (!noGet)
			elem = basic.getNextElement();

		while ((!basic.isFinished()) && (elem.compare("place") != 0) &&
			   (elem.compare("transition") != 0) && (elem.compare("arc") != 0) && (!noGet))
			elem = basic.getNextElement();

		noGet = false;

		if (!basic.isFinished())
		{
			if ((elem.compare("place") == 0) || (elem.compare("transition") == 0))
			{
				bool pl;

				if (elem.compare("place") == 0)
					pl = true;
				else
					pl = false;

				basic.getNextAttribute(elem, attr);

				if (elem.compare("id") != 0)
				{
					cerr << "Reading PNML file: element place without attribute id" << endl;
					exit(1);
				}

				if (pl)
				{
					N->AddPlace(id);
					nd[attr] = id;
				}
				else
				{
					N->AddTransition(id);
					nd[attr] = id;
				}

				N->SetName(id,duplicate(attr));
				++id;
			}
			else	// arc
			{
				string 	ssc;
				bool	noArc = false;

				basic.getNextAttribute(elem, attr);

				if (elem.compare("id") != 0)
				{
					cerr << "Reading PNML file: arc without an id" << endl;
					exit(1);
				}

				basic.getNextAttribute(elem,attr);

				if (elem.compare("source") == 0)
				{
					auto it = nd.find(attr);

					if (it == nd.end())
						noArc = true;
					else
						sc = it->second;

					ssc = attr;
				}
				else
				{
					cerr << "Reading PNML file: no source attribute for an arc" << endl;
					exit(1);
				}

				basic.getNextAttribute(elem,attr);

				if (elem.compare("target") == 0)
				{
					if (!noArc)
					{
						auto it = nd.find(attr);

						if (it == nd.end())
						{
							noArc = true;
						}
						else
							dst = it->second;
					}
				}
				else
				{
					cerr << "Reading PNML file: no source target for an arc" << endl;
					exit(1);
				}

				elem = basic.getNextElement();

				if (elem.compare("inscription") == 0)
				{
					noGet = false;
					elem = basic.getNextElement();

					if (elem.compare("text") == 0)
					{
						string val = basic.getToken();

						if (noArc)
							remainders.push_front(tuple<string,string,unsigned int>(ssc,attr,stoi(val)));
						else
							N->AddArc(sc,dst,stoi(val));
					}
					else
					{
						cerr << "Reading PNML file: inscription for an arc without text element, found: " << elem << endl;
						exit(1);
					}
				}
				else
				{
					if ((elem.compare("arc") == 0) || (elem.compare("place") == 0) || (elem.compare("transition") == 0))
						noGet = true;
					else
						noGet = false;

					if (noArc)
						remainders.push_front(tuple<string,string,unsigned int>(ssc,attr,1));
					else
						N->AddArc(sc,dst,1);
				}
			}
		}
	}

	// On ajoute les arcs pour lesquels les noeuds n'ont pas pu être résolus

	while (!remainders.empty())
	{
		auto it = remainders.begin();
		unsigned int sc, dst;
		auto itnd = nd.find(get<0>(*it));

		if (itnd == nd.end())
		{
			cerr << "Reading PNML file: no origin found for an arc - " << get<0>(*it) << endl;
			exit(1);
		}
		else
			sc = itnd->second;

		itnd = nd.find(get<1>(*it));

		if (itnd == nd.end())
		{
			cerr << "Reading PNML file: no target found for an arc - " << get<1>(*it) << endl;
			exit(1);
		}
		else
			dst = itnd->second;

		N->AddArc(sc,dst,get<2>(*it));
		remainders.pop_front();
}
}

char *PNML::duplicate(string &s)
{
	char* result = (char *)malloc(s.length()+1);

	strcpy(result,s.c_str());

	return result;
}

void PNML::findElement(const char *s)
{
	string &&elem = basic.getNextElement();

	while ((elem.compare(s) != 0) && (! basic.isFinished()))
		elem = basic.getNextElement();

	if (basic.isFinished())
	{
		cerr << "Reading PNML file: element " << s << " missing, found: " << elem << endl;
		exit(1);
	}
}
