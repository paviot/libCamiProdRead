/*
 *  Waiting.h
 *  PolyhedralRepr
 *
 *  Created by Emmanuel Paviot-Adet on 29/01/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __WAITING__

#define __WAITING__

#include <deque>
#include "StoreNet.h"

template <class StoreType> class AddTrans;

class Waiting
{
protected:
	StoreNet &N;
	virtual void Insert_Transition(CamiProdRead::Transition *T)=0;
	virtual ~Waiting(){}

public:
	Waiting(StoreNet &n);
	virtual CamiProdRead::Transition *GetNext(void)=0;
	virtual void StoreSucc(CamiProdRead::Transition *T)=0;
};

template <class StoreType>
class Waiting_Simple: public Waiting
{
protected:
	friend class AddTrans<StoreType>;
	StoreType Store;
	void Insert_Transition(CamiProdRead::Transition *T)
		{Store.push_back(T);};
public:
	Waiting_Simple(StoreNet &n);
	void StoreSucc(CamiProdRead::Transition *T);
};


class DepthFirst: public Waiting_Simple<vector<CamiProdRead::Transition *> >
{
public:
	DepthFirst(StoreNet &n): Waiting_Simple<vector<CamiProdRead::Transition *> >(n) {};
	CamiProdRead::Transition *GetNext(void);
};

class BreadthFirst: public Waiting_Simple<deque<CamiProdRead::Transition *> >
{
public:
	BreadthFirst(StoreNet &n): Waiting_Simple<deque<CamiProdRead::Transition *> >(n) {};
	CamiProdRead::Transition *GetNext(void);
};

class StructuralOrder: public Waiting
{
private:
	vector<unsigned int>	Index;		// Associates a transition rank to an index
	vector<bool>			Store;		// For each transition (in the order of N.Transitions) we if they are to fire, or not
	unsigned int			ToStudy;	// Next transition rank to study

	void Insert_Transition(CamiProdRead::Transition *T)
		{Store[T->GetInfo()] = true;}
	
public:
	StructuralOrder(StoreNet &n);
	CamiProdRead::Transition *GetNext(void);
	void StoreSucc(CamiProdRead::Transition *T);
};

// Le code moche

template <class StoreType>
class AddTrans
	{
	public:
		Waiting_Simple<StoreType> *W;
		AddTrans(Waiting_Simple<StoreType> *w): W(w) {};
		void operator() (const CamiProdRead::Arc *A)
		{
			if (find(W->Store.begin(), W->Store.end(), (CamiProdRead::Transition *)A->GetEnd()) == 
				W->Store.end())
				W->Insert_Transition((CamiProdRead::Transition *)A->GetEnd());
		}
	};

template <class StoreType>
struct SuccPlace
{
	Waiting_Simple<StoreType> *W;
	SuccPlace(Waiting_Simple<StoreType> *w): W(w) {};
	void operator() (const CamiProdRead::Arc *A)
	{
		CamiProdRead::Node *Pl = A->GetEnd();
		
		for_each(Pl->Out.begin(), Pl->Out.end(), AddTrans<StoreType>(W));
	}
};

template <class StoreType>
Waiting_Simple<StoreType>::Waiting_Simple(StoreNet &n): Waiting(n)
{
//cout << "Debut Init" << endl;
	for (unsigned int i = 0; i < N.NbTransitions(); i++)
		Insert_Transition((CamiProdRead::Transition *)(N.Transitions[i]));
//cout << "Fin Init" << endl;
}

template <class StoreType>
void Waiting_Simple<StoreType>::StoreSucc(CamiProdRead::Transition *T)
{
	for_each(T->Out.begin(), T->Out.end(), SuccPlace<StoreType>(this));
}

#endif
