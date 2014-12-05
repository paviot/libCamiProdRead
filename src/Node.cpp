/*
 *  Node.cpp
 *  Ordre
 *
 *  Created by Emmanuel Paviot-Adet on Tue May 28 2002.
 *  Copyright (c) 2002 LIP6. All rights reserved.
 *
 */

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <iostream>
#include <algorithm>


#include "Utilities.h"
#include "Node.h"

using namespace CamiProdRead;
using namespace std;

// ********************** Node definition **********************

Node::~Node()
{
	for(ArcsContainer::iterator It = In.begin(); It != In.end(); ++It)
		delete *It;
}

class IsArcIn
{
	const Node *N;
public:
	IsArcIn(const Node *n): N(n) {};
	bool operator()(const Arc *Test)
	{return Test->GetStart() == N;};
};

unsigned int Node::GetIn(const Node *N)
{
	ArcsContainer::const_iterator it = find_if(In.begin(), In.end(), IsArcIn(N));
	
	if (it == In.end())
		return 0;
	else
		return (*it)->GetValue();
}

class IsArcOut	// A UNIFIER AVEC ISARCIN VIA UN TEMPLATE
{
	const Node *N;
public:
	IsArcOut(const Node *n): N(n) {};
	bool operator()(const Arc *Test)
	{return Test->GetEnd() == N;};
};

unsigned int Node::GetOut(const Node *N)
{
	ArcsContainer::const_iterator it = find_if(Out.begin(), Out.end(), IsArcOut(N));
	
	if (it == Out.end())
		return 0;
	else
		return (*it)->GetValue();
}

struct PrintArc
{
	PrintArc(void){};
	void operator()(const Arc *A)
	{
		cout << "    " << A->GetStart()->GetName() << " -> " << A->GetEnd()->GetName();
		cout << " Val : " << A->GetValue() << endl;
	};
};

void Node::Print(void)	// A METTRE DANS LA CLASSE ARC
{
	cout << Name << ", Id " << Id << " : " << endl << "  In : ";
	cout << In.NbElements() << " arcs" << endl;

	for_each(In.begin(), In.end(), PrintArc());

	cout << "  Out : " << Out.NbElements() << " arcs" << endl;

	for_each(Out.begin(), Out.end(), PrintArc());
}

struct NbSimple
{
	int Nb;
	NbSimple(void) {Nb = 0;};
	void operator() (ArcsDescription A)
	{
		if ((A.GetIn() == 0) || (A.GetOut() == 0))
			Nb++;
	}
};

int Node::NbSimpleArcs(void)
{
	return for_each(EnumerateArcs.begin(), EnumerateArcs.end(), NbSimple()).Nb;
}

struct NbDouble
{
	int Nb;
	NbDouble(void) {Nb = 0;};
	void operator() (ArcsDescription A)
	{
/*		cout << "Le noeud sur lequel porte l'itération : " << A.GetRefNode()->GetName() << endl;
		cout << "Le noeud sur à l'autre bout : " << A.GetNode()->GetName() << endl;
		cout << "Valeur sur l'arc entrant : " << A.GetIn() << endl;
		cout << "Valeur sur l'arc sortant : " << A.GetOut() << endl; */
		
		if ((A.GetIn() != 0) && (A.GetOut() != 0))
			Nb++;
	}
};

int Node::NbDoubleArcs(void)
{
	return for_each(EnumerateArcs.begin(), EnumerateArcs.end(), NbDouble()).Nb;
}

// ********************** ArcsIterator definition **********************

ArcsIterator ArcsIterator::begin(void)
{
	AD.N->In.Sort();
	AD.N->Out.Sort();
	AD.itIn = AD.N->In.begin();
	AD.itOut = AD.N->Out.begin();

	return ++(*this);
}

ArcsIterator TheEnd(NULL, true);

ArcsIterator ArcsIterator::end(void) const
{
	return TheEnd;
}

/*ArcsIterator ArcsIterator::operator++ ()
{
	if ((itIn == N->In.end()) && (itOut == N->Out.end()))
		End = true;
	else if (itIn == N->In.end())
	{
		V = (*itOut)->GetEnd();
		In = 0;
		Out = (*itOut)->GetValue();
		itOut++;
	}
	else if (itOut == N->Out.end())
	{
		V = (*itIn)->GetStart();
		In = (*itIn)->GetValue();
		Out = 0;
		itIn++;
	}
	else if ((*itOut)->GetEnd()->GetId() < (*itIn)->GetStart()->GetId())
	{
		V = (*itOut)->GetEnd();
		In = 0;
		Out = (*itOut)->GetValue();
		itOut++;
	}
	else if ((*itIn)->GetStart()->GetId() < (*itOut)->GetEnd()->GetId())
	{
		V = (*itIn)->GetStart();
		In = (*itIn)->GetValue();
		Out = 0;
		itIn++;
	}
	else
	{
		V = (*itIn)->GetStart();
		In = (*itIn)->GetValue();
		Out = (*itOut)->GetValue();
		itIn++;
		itOut++;
	}

	return *this;
} */

void ArcsIterator::increment()
{
//	cout << "On incrémente" << endl;
	if ((AD.itIn == AD.N->In.end()) && (AD.itOut == AD.N->Out.end()))
		AD.End = true;
	else if (AD.itIn == AD.N->In.end())
	{
		AD.V = (*(AD.itOut))->GetEnd();
		AD.In = 0;
		AD.Out = (*(AD.itOut))->GetValue();
		AD.itOut++;
//		cout << "1" << endl;
	}
	else if (AD.itOut == AD.N->Out.end())
	{
		AD.V = (*(AD.itIn))->GetStart();
		AD.In = (*(AD.itIn))->GetValue();
		AD.Out = 0;
		AD.itIn++;
//		cout << "2" << endl;
	}
	else if ((*(AD.itOut))->GetEnd()->GetId() < (*(AD.itIn))->GetStart()->GetId())
	{
		AD.V = (*(AD.itOut))->GetEnd();
		AD.In = 0;
		AD.Out = (*(AD.itOut))->GetValue();
		AD.itOut++;
//		cout << "3" << endl;
	}
	else if ((*(AD.itIn))->GetStart()->GetId() < (*(AD.itOut))->GetEnd()->GetId())
	{
		AD.V = (*(AD.itIn))->GetStart();
		AD.In = (*(AD.itIn))->GetValue();
		AD.Out = 0;
		AD.itIn++;
//		cout << "4" << endl;
	}
	else
	{
		AD.V = (*(AD.itIn))->GetStart();
		AD.In = (*(AD.itIn))->GetValue();
		AD.Out = (*(AD.itOut))->GetValue();
		AD.itIn++;
		AD.itOut++;
//		cout << "5" << endl;
	}
/*		cout << "$$$ Le noeud sur lequel porte l'itération : " << AD.GetRefNode()->GetName() << endl;
		cout << "$$$ Le noeud sur à l'autre bout : " << AD.GetNode()->GetName() << endl;
		cout << "$$$ Valeur sur l'arc entrant : " << AD.GetIn() << endl;
		cout << "$$$ Valeur sur l'arc sortant : " << AD.GetOut() << endl;*/
//		this->base_reference() = &AD;
}

/*bool ArcsIterator::operator!= (const ArcsIterator &A) const
{
	if (End != A.IsEnd())
		return true;
	else if (End)
		return false;

	return (V != A.GetNode()) || (N != A.GetRefNode()) || (In != A.GetIn()) || (Out != A.GetOut());
}*/

/*
bool ArcsIterator::equal(const ArcsIterator &a) const
{
	if (AD.End != a.End)
		return false;
	else if (AD.End)
		return true;

	return 	(AD.V == a.GetNode()) && (AD.N == a.GetRefNode()) && 
			(AD.In == a.GetIn()) && (AD.Out == a.GetOut());
}
*/

bool ArcsIterator::operator==(const ArcsIterator &a) const
{
	if (AD.End != a.AD.End)
		return false;
	else if (AD.End)
		return true;

	return 	(AD.V == a.GetNode()) && (AD.N == a.GetRefNode()) && 
			(AD.In == a.GetIn()) && (AD.Out == a.GetOut());
}

// ********************** Transition definition **********************

char Transition::IsPrecondValid(const Node *N, const unsigned int Val)
{
	unsigned int Pre = GetIn(N);
	
	if (Pre == 0)
		return NOTINVOLVED;
	
	if (Val >= Pre)
		return VALID;
	else
		return INVALID;
}

// ********************** Place definition **********************

void Place::Print(void)
{
	this->Node::Print();
	cout << "  Initial marking: " << this->GetInitial() << endl;
}
