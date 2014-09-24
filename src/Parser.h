#ifndef __PARSERH__

#define __PARSERH__

#include <iostream>
#include "Expression.h"

/*! \brief Lecture d'une somme coefficient�e de places.

	\param N Le r�seau de Petri auquel se r�f�re la contrainte
	\param str La ligne contenant la contrainte
	\return La contrainte interpr�t�e */

BodyExp* ParseSum(StoreNet *N, const string& str);

/*! \brief Lecture d'une contrainte lin�aire.

	\param N Le r�seau de Petri auquel se r�f�re la contrainte
	\param FileName Le nom du fichier contenant la contrainte
	\return La contrainte interpr�t�e */

ExpList* ParseProperty(StoreNet* N, const char *FileName);

/*! \brief Lecture d'une contrainte lin�aire.

	\param N Le r�seau de Petri auquel se r�f�re la contrainte
	\param str La ligne contenant la contrainte
	\return La contrainte interpr�t�e */

ExpList* ParseProperty(StoreNet *N, const string& str);

#endif
