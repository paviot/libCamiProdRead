#include <iostream>
#include <stdlib.h>
#include "Expression.h"
#include "StoreNet.h"

ostream& operator<< (ostream& os, const BodyExp& Property)
{
	bool		DebElt  = true;

	for (ExpAdd::const_iterator EltIt = Property.Sum->begin(); EltIt != Property.Sum->end(); 
																							++EltIt)
	{
		const ExpElt& Body = **EltIt;

		if (Body.second > 0)
			if (!DebElt)
				os << " + ";

		DebElt = false;

		if (Body.second != 1)
			os << Body.second << "*";

		os << Property.N->GetName(Body.first);
	}

	return os;
}

ostream& operator<< (ostream& os, const Expression& Property)
{
	
	os << *(Property.Body) << " ";

	switch(Property.Comp)
	{
		case InfEq:
			os << "<= ";
			break;
		case SupEq:
			os << ">= ";
			break;
		case Eq:
			os << "=";
			break;
		default:
			cout << "Unknown comparator";
			exit(1);
	}

	os << Property.Cst;

	return os;
}

ostream& operator<< (ostream& os, const ExpList& Property)
{
	bool Debut = true;

	for (ExpList::const_iterator LExpIt = Property.begin();	LExpIt != Property.end(); ++LExpIt)
	{
		
		if (Debut)
			Debut = false;
		else
			os << " or ";

		os << **LExpIt;
	}

	return os;
}
