/*
 *  Utilities.cpp
 *  Ordre
 *
 *  Created by Emmanuel Paviot-Adet on Mon Jun 03 2002.
 *  Copyright (c) 2002 LIP6. All rights reserved.
 *
 */

#include <stdlib.h>
#include <iostream>
#include "Utilities.h"

using namespace std;

void Error(const char *Mess)
{
	cerr << Mess << endl;
	exit(1);
}
