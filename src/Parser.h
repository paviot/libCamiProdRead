#ifndef __PARSERH__

#define __PARSERH__

#include <iostream>
#include "Expression.h"

/*! \brief Lecture d'une somme coefficientée de places.

	\param N Le réseau de Petri auquel se réfère la contrainte
	\param str La ligne contenant la contrainte
	\return La contrainte interprétée */

BodyExp* ParseSum(StoreNet *N, const string& str);

/*! \brief Lecture d'une contrainte linéaire.

	\param N Le réseau de Petri auquel se réfère la contrainte
	\param FileName Le nom du fichier contenant la contrainte
	\return La contrainte interprétée */

ExpList* ParseProperty(StoreNet* N, const char *FileName);

/*! \brief Lecture d'une contrainte linéaire.

	\param N Le réseau de Petri auquel se réfère la contrainte
	\param str La ligne contenant la contrainte
	\return La contrainte interprétée */

ExpList* ParseProperty(StoreNet *N, const string& str);

#endif
