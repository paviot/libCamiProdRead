/* Remarque générale : dans le type Expression, le StoreNet* était initialement un const StoreNet&.
  J'arrivais à doner la valeur, mais dès quon sortait de la fonction NewExpression, la valeur
  semble être perdue. Du coup, je me suis rabattu sur le pointeur... */


//#define DEBUG_PARSER
//#define BOOST_SPIRIT_DEBUG

/* Pour pouvoir utiliser plus de 3 arguments dans les templates de phoenix */

#define PHOENIX_LIMIT 6
#define BOOST_SPIRIT_CLOSURE_LIMIT 6

#include <iostream>
#include <string>
#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_iterator.hpp>
#include <boost/spirit/include/classic_attribute.hpp>
#include <boost/spirit/include/phoenix1_functions.hpp>


#include "StoreNet.h"
#include "Expression.h"
#include "Parser.h"

using namespace std;
using namespace boost::spirit;
using namespace boost::spirit::classic;
using namespace phoenix;

struct getplid_
{
	template <typename N, typename PN>
	struct result {typedef int type;};

	template <typename N, typename PN>
	int operator()(N& name, PN& n)
	{
#ifdef DEBUG_PARSER
		cout << "GetPlaceId, adresse de N : " << &n << " name: " << name << endl;
#endif
		return n->Places.GetElementId(name);
	};
};

phoenix::function<getplid_> const GetPlaceId = getplid_();

struct newexpelt_
{
	template <typename PI, typename C>
	struct result {typedef ExpElt* type;};

	template <typename PI, typename C>
	ExpElt* operator()(PI& plid, C& coeff)
	{
		ExpElt* e = new ExpElt(plid, coeff);
#ifdef DEBUG_PARSER
		cout << "NewExpElt, adresse du ExpElt : " << e << " plid : " << e->first << " coeff : ";
		cout << e->second << endl;
#endif
		return e;
	};
};

phoenix::function<newexpelt_> const NewExpElt = newexpelt_();

struct newbodyexp_
{
	template <typename E, typename N>
	struct result {typedef BodyExp* type;};

	template <typename E, typename N>
	BodyExp* operator()(E& e, N& n)
	{
		BodyExp* t = new BodyExp();

		t->Sum = new ExpAdd();
		t->Sum->push_back(e);
		t->N = n;

#ifdef DEBUG_PARSER
		cout << "NewBodyExp, adresse du ExpAdd : " << t << " Adresse de l'élément : " << e;
		cout << " (détail : élément id :  " << e->first << ", coeff : " << e->second <<")";
		cout << " adresse du réseau : " << n << endl;
#endif
		return t;
	};
};

phoenix::function<newbodyexp_> const NewBodyExp = newbodyexp_(); 


struct modcoeff_
{
	template<typename EE, typename C>
	struct result {typedef void type;};

	template<typename EE, typename C>
	void operator()(EE& e, C& coeff)
	{
		e->second *= coeff;
#ifdef DEBUG_PARSER
		cout << "ModifCoeffExpElt coefficient : " << coeff << endl;
#endif
	};
};

phoenix::function<modcoeff_> const ModifCoeffExpElt = modcoeff_();

struct pushback_
{
	template <typename C, typename E>
	struct result {typedef void type;};

	template <typename C, typename E>
	void operator()(C& c, E& e)
	{
		c->push_back(e);
#ifdef DEBUG_PARSER
		cout << "PushBack, adresse du container : " << c << " Adresse de l'élément : " << e << endl;
#endif
	};
};

phoenix::function<pushback_> const PushBack = pushback_();

struct pushbackbodyexp_
{
	template <typename C, typename E>
	struct result {typedef void type;};

	template <typename C, typename E>
	void operator()(C& c, E& e)
	{
		c->Sum->push_back(e);
#ifdef DEBUG_PARSER
		cout << "PushBack, adresse du container : " << c << " Adresse de l'élément : " << e << endl;
#endif
	};
};

phoenix::function<pushbackbodyexp_> const PushBackBodyExp = pushbackbodyexp_();

struct newexpr_
{
	template <typename B, typename C, typename I>
	struct result {typedef Expression* type;};

	template <typename B, typename C, typename I>
	Expression* operator()(B& b, C& c, I& i)
	{
		Expression *e = new Expression();

		e->Body = b;
		e->Comp = c;
		e->Cst = i;

#ifdef DEBUG_PARSER
		cout << "NewExpression, adresse de l'expression : " << e  << endl;
		cout << "Adresse de Body : " << b << endl;
#endif
		return e;
	};
};

phoenix::function<newexpr_> const NewExpression = newexpr_();

struct newexplist_
{
	template <typename E>
	struct result {typedef ExpList* type;};

	template <typename E>
	ExpList* operator()(E& e)
	{
		ExpList* t = new ExpList();

		t->push_back(e);

#ifdef DEBUG_PARSER
		cout << "NewExpList, adresse de l'ExpList : " << t << " Adresse de l'élément : " << e;
		cout << endl;
#endif
		return t;
	};
};

phoenix::function<newexplist_> const NewExpList = newexplist_();

struct int_closure : boost::spirit::classic::closure<int_closure, int>
{
	member1 val;
};

struct id_closure : boost::spirit::classic::closure<id_closure, unsigned int>
{
	member1 plid; // Attention on ne peut pas utiliser id comme nom
};

struct ExpElt_closure : boost::spirit::classic::closure<ExpElt_closure, ExpElt *, unsigned int, int>
{
	member1 val;
	member2 coeff;
};

/* struct ExpAdd_closure : boost::spirit::classic::closure<ExpAdd_closure, ExpAdd *, int>
{
	member1 val;
	member2 coeff;
}; */

struct BodyExp_closure : boost::spirit::classic::closure<BodyExp_closure, BodyExp*, int>
{
	member1 val;
	member2 coeff;
};

struct ExpComparator_closure : boost::spirit::classic::closure<ExpComparator_closure, ExpComparator>
{
	member1 comp;
};

struct Expression_closure : boost::spirit::classic::closure<Expression_closure, Expression*, BodyExp*,
													ExpComparator>
{
	member1 exp;
	member2 namelist;
	member3 comp;
};

struct ExpList_closure : boost::spirit::classic::closure<ExpList_closure, ExpList*>
{
	member1 val;
};

struct readSum : public grammar<readSum, BodyExp_closure::context_t>
{
	StoreNet*& N;
	
	readSum(StoreNet*& n): N(n) {};

	template <typename ScannerT>
	struct definition
	{
		definition(readSum const& self)
		{
			first = (
				Init			=	(
										eps_p
											[self.coeff = 1]
										>>
										NameList
									),
				NameList		=	!(NegSign(1)
										[self.coeff = arg1]
									)
									>> 
									NameCoeff((ExpElt*)0, 1, 1)	// Pour initialiser le coefficient
										[ModifCoeffExpElt(arg1, self.coeff)]
										[self.val = NewBodyExp(arg1, self.N)]
										[self.coeff = 1]
											>> 
									*(
										(
											ch_p('+') | 
											(NegSign(1)
												[self.coeff = arg1]
											)
										) >> 
										NameCoeff((ExpElt*)0, 1, 1) // Pour initialiser le coefficient
											[ModifCoeffExpElt(arg1, self.coeff)]
											[PushBackBodyExp(self.val, arg1)]
											[self.coeff = 1]
								 	 ),


				NegSign			= ch_p('-')
										[NegSign.val = -1]
									,

				NameCoeff		=	(
										!(Coeff
											[NameCoeff.coeff = arg1]
										)
										>> 
										Name
											[NameCoeff.val = NewExpElt(arg1, NameCoeff.coeff)]
									)
									,

				Name 			= 	(
										(
											alpha_p | 
											'_'
										)
										>> 
										*(
											alnum_p | 
											'_'
										 )
									)
										[Name.plid = GetPlaceId(construct_<std::string>(arg1, arg2),
																self.N)]
									,

				Coeff			= 	uint_p
										[Coeff.val = arg1] 
									>> 
									'*'
			);
            BOOST_SPIRIT_DEBUG_NODE(NameList);
            BOOST_SPIRIT_DEBUG_NODE(NameCoeff);
            BOOST_SPIRIT_DEBUG_NODE(Name);
            BOOST_SPIRIT_DEBUG_NODE(Coeff);
            BOOST_SPIRIT_DEBUG_NODE(NegSign);
            BOOST_SPIRIT_DEBUG_NODE(first);
		}
		
		rule<ScannerT> const& start() const { return first; }
		
	private:
		subrule<0, int_closure::context_t>				Coeff;
		subrule<1, id_closure::context_t>				Name;
		subrule<2, ExpElt_closure::context_t> 			NameCoeff;
		subrule<3, int_closure::context_t>				NegSign;
		subrule<4, BodyExp_closure::context_t>			NameList;
		subrule<5>										Init;

		rule<ScannerT>  first;

	};
};

BodyExp* ParseSum(StoreNet *N, const string& str)
{
    BodyExp* Result;
    readSum grammar(N);

	parse(str.c_str(), grammar[var(Result) = arg1], space_p);

	return Result;
}


struct readExpressions : public grammar<readExpressions, ExpList_closure::context_t>
{
	StoreNet*& N;
	
	readExpressions(StoreNet*& n): N(n) {};

	template <typename ScannerT>
	struct definition
	{
		readSum readSumParser;
		subrule<0, ExpComparator_closure::context_t>	Comp;
		subrule<1, Expression_closure::context_t>		Expression1;
		subrule<2, ExpList_closure::context_t>			ExpressionList;

		rule<ScannerT>  first;

		definition(readExpressions const& self): readSumParser(self.N)
		{
			first = (
				ExpressionList	= 	Expression1
										[self.val = NewExpList(arg1)]
									>> 
									*(
										ch_p('|') >> 
										'|' >> 
										Expression1
											[PushBack(self.val, arg1)]
									 )
									 >> 
									!ch_p(';')
									,

				Expression1		= 	readSumParser((BodyExp*)0, 1)
										[Expression1.namelist = arg1]
									>>
									Comp(Eq)
										[Expression1.comp = arg1]
									>>
									int_p
										[Expression1.exp = NewExpression(Expression1.namelist,
																	Expression1.comp, arg1)]
										,

				Comp			= 	!(
										(
											ch_p('>')
												[Comp.comp = SupEq]
										)
										|
										(
											ch_p('<')
												[Comp.comp = InfEq]
										)
									)
									>>
									'='

			);
            BOOST_SPIRIT_DEBUG_NODE(ExpressionList);
            BOOST_SPIRIT_DEBUG_NODE(Expression1);
            BOOST_SPIRIT_DEBUG_NODE(Comp);
            BOOST_SPIRIT_DEBUG_NODE(readSumParser);
            BOOST_SPIRIT_DEBUG_NODE(first);
			}
		
		rule<ScannerT> const& start() const { return first; }
	};
};

// ------------------------------------- ParseProperty -------------------------------------

ExpList *ParseProperty(StoreNet *N, const char *FileName)
{
	file_iterator<> first(FileName);

	if (!first)
	{
		std::cout << "Unable to open file: " << FileName << endl;
		exit(1);
	}

    file_iterator<> last = first.make_end();
    ExpList *List;
    readExpressions grammar(N);

	parse(first, last, grammar[var(List) = arg1], space_p);

	return List;
}

ExpList *ParseProperty(StoreNet *N, const string& str)
{
    ExpList *List;
    readExpressions grammar(N);

	parse(str.c_str(), grammar[var(List) = arg1], space_p);

	return List;
}
