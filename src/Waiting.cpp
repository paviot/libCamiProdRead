/*
 *  Waiting.cpp
 *  PolyhedralRepr
 *
 *  Created by Emmanuel Paviot-Adet on 29/01/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include <algorithm>
#include <forward_list>
#include <vector>

#include "StoreNet.h"
#include "Waiting.h"
 
using namespace std;
using namespace CamiProdRead;
//using namespace __gnu_cxx;


Waiting::Waiting(StoreNet &n): N(n)
{
	for(unsigned int i = 0; i < N.NbTransitions(); ++i)
		N.Transitions[i]->SetInfo(i);

	for(unsigned int i = 0; i < N.NbPlaces(); ++i)
		N.Places[i]->SetInfo(i);
}

Transition *DepthFirst::GetNext(void)
{
	Transition *Result;
	
	if (Store.empty())
		Result = 0;
	else
	{
		Result = Store.back();
		Store.pop_back();
	}
	
	return Result;
}

Transition *BreadthFirst::GetNext(void)
{
	Transition *Result;
	
	if (Store.empty())
		Result = 0;
	else
	{
		Result = Store.front();
		Store.pop_front();
	}
	
	return Result;
}

class ScanPrecond{
public:
	bool Fireable;
	int *PlacePrec;
	ScanPrecond(int *placep): Fireable(true), PlacePrec(placep) {};
	void operator() (const CamiProdRead::Arc *A)
	{
		Node *Pl = A->GetStart();
		
		if ((Fireable) && (PlacePrec[Pl->GetInfo()] > 0))
			Fireable = false;
	}
};

class FirePrecond{
public:
	int			*PlacePrec;
	vector<int>	&ToRemove;
	
	FirePrecond(int *placep, vector<int> &torem): PlacePrec(placep), ToRemove(torem) {}
	void operator() (const CamiProdRead::Arc *A)
	{
		Node *Pl = A->GetStart();
		
		PlacePrec[Pl->GetInfo()]++;
		
		if (PlacePrec[Pl->GetInfo()] == 0)
			ToRemove.push_back(Pl->GetInfo());
	}
};

class FirePostcond{
public:
	int						*PlacePrec;
	forward_list<unsigned int>	&ToSee;
	vector<bool>			&MarkedPlace;
	FirePostcond(int *placep, forward_list<unsigned int> &tosee, vector<bool> &markedpl): PlacePrec(placep), ToSee(tosee), 
																							MarkedPlace(markedpl) {}
	void operator() (const ArcsDescription A)
	{
		Node *Pl = A.GetNode();
		
		if ((A.GetIn() == 0) && (PlacePrec[Pl->GetInfo()] > 0))	// Test arcs are not taken into account
		{
			PlacePrec[Pl->GetInfo()]--;
			MarkedPlace[Pl->GetInfo()] = true;
		}
		
		if ((A.GetOut() > 0) && (PlacePrec[Pl->GetInfo()] == 0))
		{
			ToSee.push_front(Pl->GetInfo());
			PlacePrec[Pl->GetInfo()] = -1*Pl->Out.NbElements();
		}
	}
};

class ScanTransition{
public:
	unsigned int			&Treated;
	int						*PlacePrec;
	vector<unsigned int>	&Index;
	vector<bool>			&FiredTrans;
	vector<bool>			&MarkedPlace;
	forward_list<unsigned int>		&ToSee;
	vector<int>				&ToRemove;
	
	ScanTransition(unsigned int &treat, int *placep, vector<unsigned int> &index, vector<bool> &firedt, vector<bool> &markedpl, 
					forward_list<unsigned int> &tosee, vector<int> &torem): Treated(treat), PlacePrec(placep), Index(index), 
					FiredTrans(firedt), MarkedPlace(markedpl), ToSee(tosee), ToRemove(torem) {};
	void operator() (const CamiProdRead::Arc *A)
	{
		Node *Trans = A->GetEnd();
		
		cout << "On regarde la transition : " << Trans->GetName() << endl;
		
		if (!FiredTrans[Trans->GetInfo()])
			if (for_each(Trans->In.begin(), Trans->In.end(), ScanPrecond(PlacePrec)).Fireable)
			{
				Index.push_back(Trans->GetInfo());
				++Treated;
				FiredTrans[Trans->GetInfo()] = true;
				for_each(Trans->In.begin(), Trans->In.end(), FirePrecond(PlacePrec, ToRemove));
				for_each(Trans->EnumerateArcs.begin(), Trans->EnumerateArcs.end(), FirePostcond(PlacePrec, ToSee, MarkedPlace));
			}
	}
};

class IsFireable{
public:
	vector<bool>	&MarkedPlace;
	int				*PlacePrec;
	unsigned int	Deficit;
	unsigned int	Nb;
	IsFireable(vector<bool> &markedp, int *placep, unsigned int d): MarkedPlace(markedp), PlacePrec(placep), 
																	Deficit(d), Nb(0) {}
	void operator() (const CamiProdRead::Arc *A)
	{
		Node *Pl = A->GetEnd();
		
		if ((Nb == Deficit) || (!MarkedPlace[Pl->GetInfo()]))
			Nb = Deficit;
		else
		{
			if (PlacePrec[Pl->GetInfo()] > 0)
			Nb += PlacePrec[Pl->GetInfo()];
		}
	}
};

StructuralOrder::StructuralOrder(StoreNet &n): Waiting(n), ToStudy(0)
{
	int						PlacePrec[N.NbTransitions()];	// Number of predecessor transitions to see before 
															// the place can be considered as useful
															// When a place is useful, a negative number indicates it 
															// as well as the actual number of successor functions to insert
	forward_list<unsigned int>		ToSee;							// Places rank of useful places
	unsigned int			Treated = 0;
	vector<bool>			FiredTrans;
	vector<bool>			MarkedPlace;
	vector<int>				ToRemove;

	for (unsigned int i = 0; i < N.NbPlaces(); ++i)
	{
		if (((Place *)(N.Places[i]))->GetInitial() > 0)
		{
			PlacePrec[i] = -1*N.Places[i]->Out.NbElements();
			ToSee.push_front(i);
			MarkedPlace.push_back(true);
		}
		else
		{
			PlacePrec[i] = N.Places[i]->In.NbElements() - N.Places[i]->NbDoubleArcs();
			MarkedPlace.push_back(false);
		}
	}
		
	for (unsigned int i = 0; i < N.NbTransitions(); ++i)
	{
		FiredTrans.push_back(false);
		Store.push_back(true);
	}
	
	while (Treated < N.NbTransitions())
	{
		unsigned int OldTreated = Treated;

		cout << "On boucle" << endl;
		cout << "ToSee : ";
//		for (forward_list<unsigned int>::iterator It = ToSee.begin(); It != ToSee.end(); ++It)
//			cout << *It << " ";
//		cout << endl; 
		
		for (forward_list<unsigned int>::iterator It1 = ToSee.begin(); It1 != ToSee.end(); ++It1)
		{
			cout << *It1 << " " << *(ToSee.begin()) << endl;
//			cout << *(ToSee.begin()) << " " << endl;

			for_each(N.Places[*It1]->Out.begin(), N.Places[*It1]->Out.end(), ScanTransition(Treated, PlacePrec, Index, 
																						  FiredTrans, MarkedPlace, ToSee, ToRemove));
		}
		
		if (OldTreated == Treated)
		{
			unsigned int Id = N.NbTransitions();
			unsigned int Deficit = N.NbPlaces()*N.NbTransitions();
			
			for(unsigned int i = 0; i < N.NbTransitions(); ++i)
				if (!FiredTrans[i])
				{
					unsigned int temp = for_each(N.Transitions[i]->In.begin(), N.Transitions[i]->In.end(), 
												 IsFireable(MarkedPlace, PlacePrec, Deficit)).Nb;
					if (temp < Deficit)
					{
						Id = i;
						Deficit = temp;
					}
				}
			
			if (Id == N.NbTransitions())
			{
				cout << "Warning: cannot find a fireable transition" << endl;
				Id = 0;
				
				while (FiredTrans[Id])
					++Id;
			}
			
			Index.push_back(N.Transitions[Id]->GetInfo());
			++Treated;
			FiredTrans[N.Transitions[Id]->GetInfo()] = true;
			for_each(N.Transitions[Id]->In.begin(), N.Transitions[Id]->In.end(), FirePrecond(PlacePrec, ToRemove));
			for_each(N.Transitions[Id]->EnumerateArcs.begin(), N.Transitions[Id]->EnumerateArcs.end(), 
					 FirePostcond(PlacePrec, ToSee, MarkedPlace));
		}
		
		while (!ToRemove.empty())
		{
			unsigned int Elem = ToRemove.back();
			
			ToRemove.pop_back();
			forward_list<unsigned int>::iterator Prec = ToSee.end();
			forward_list<unsigned int>::iterator It = ToSee.begin();
			
			while (It != ToSee.end())
			{
				if (*It == Elem)
				{
					if (Prec == ToSee.end())
						ToSee.pop_front();
					else
						ToSee.erase_after(Prec);
					
					It = ToSee.end();
				}
				else
				{
					Prec = It;
					++It;
				}
			}
		}
	}
}

Transition *StructuralOrder::GetNext(void)
{
	unsigned int Seen = 0;
	
	while ((!Store[Index[ToStudy]]) && (Seen < N.NbTransitions()))
	{
		ToStudy = (++ToStudy) % N.NbTransitions();
		++Seen;
	}
	
	if (Seen == N.NbTransitions())
		return 0;
	else
	{
		Store[Index[ToStudy]] = false;
		return (Transition *)N.Transitions[Index[ToStudy]];
	}
}

void StructuralOrder::StoreSucc(CamiProdRead::Transition *T)
{
	for (ArcsContainer::iterator ItPl = T->Out.begin(); ItPl != T->Out.end(); ++ItPl)
	{
		Node *Pl = (*ItPl)->GetEnd();
		for (ArcsContainer::iterator ItTr = Pl->Out.begin(); ItTr != Pl->Out.end(); ++ItTr)
		{
			Node *Tr = (*ItTr)->GetEnd();
			
			if (Tr != T)
				Store[Tr->GetInfo()] = true;
		}
	}
}

