/*
 * PNML.h
 *
 *  Created on: Jun 25, 2019
 *      Author: paviot
 */

#ifndef SRC_PNML_H_
#define SRC_PNML_H_

#include "StoreNet.h"
#include "XML.h"

/** \brief La classe permettant de lire efficacement un PNML
 */

class PNML {
	XML basic;
public:
	PNML(const char *f, StoreNet *N);
private:
	char *duplicate(std::string &s);
	void findElement(const char *s);
};

#endif /* SRC_PNML_H_ */
