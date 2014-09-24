/*
 *  Expression.h
 *  CTraps
 *
 *  Created by Emmanuel Paviot-Adet on Wed Nov 10 2004.
 *  Copyright (c) 2004 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __EXPRESSION__

#define __EXPRESSION__

#include <utility>
#include <vector>
#include "StoreNet.h"

typedef enum {InfEq, SupEq, Eq} ExpComparator;

typedef pair<unsigned int, int> ExpElt;

typedef vector<ExpElt *> ExpAdd;

struct BodyExp {
	ExpAdd*		Sum;
	StoreNet*	N;
};

struct Expression {
	BodyExp*		Body;
	ExpComparator   Comp;
	int				Cst;
};

/*! \brief Liste d'expressions. */

typedef vector<Expression *> ExpList;

/*! \brief Opérateur d'impression du corps d'une contrainte linéaire. */

ostream& operator<< (ostream& os, const BodyExp& Property);

/*! \brief Opérateur d'impression d'une contrainte linéaire. */

ostream& operator<< (ostream& os, const Expression& s);

/*! \brief Opérateur d'impression d'une liste de contraintes linéaires. */

ostream& operator<< (ostream& os, const ExpList& s);

#endif
