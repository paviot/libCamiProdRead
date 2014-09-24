#ifndef __IO__

#define __IO__

#include <iostream>
#include "StoreNet.h"

/*! \brief Classe servant à imprimer un modèle au format CAMI. */

class PrintCami
{
private:

/*! \var const StoreNet &N
	\brief Le réseau à imprimer. */

	const StoreNet &N;
	ostream &os;

public:
	PrintCami(const StoreNet &n, ostream &o): N(n), os(o) {};
	void operator() (void);
};

#endif
