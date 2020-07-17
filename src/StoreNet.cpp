/*
 *  Net.cpp
 *  Ordre
 *
 *  Created by Emmanuel Paviot-Adet on Sat May 18 2002.
 *  Copyright (c) 2001 LIP6. All rights reserved.
 *
 */

#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <cassert>
#include <functional>
#include <algorithm>

#include <iostream>

#include "Utilities.h"
#include "Node.h"
#include "StoreNet.h"

#include "XML.h"

//#define yyparse cpr_parse

extern unsigned int NbPhi;

// enum {CN, PO, CT, PT, CA, PI};

using namespace std;

// ********************** StoreNet definition **********************

void StoreNet::IsolateNode(unsigned int Id, bool /*Place*/)
{
	// Destroy all arcs related to the node

	CamiProdRead::Node *Nd = NULL;

	if (Nodes.find(Id) == Nodes.end())
		Error("Unknown node Id in IsolateNode\n");
	else
		Nd = Nodes[Id];

	// Removes input arcs

	for(CamiProdRead::ArcsContainer::iterator It = Nd->In.begin(); It != Nd->In.end(); It++)
	{
		(*It)->GetStart()->RemoveOut(*It);
		Arcs.erase(*It);
		delete *It;
	}

	// Removes output arcs

	for(CamiProdRead::ArcsContainer::iterator It = Nd->Out.begin(); It != Nd->Out.end(); It++)
	{
		(*It)->GetEnd()->RemoveIn(*It);
		Arcs.erase(*It);
		delete *It;
	}

	Nodes.erase(Id);
}

bool ComparePArc(const CamiProdRead::Arc *N1, const CamiProdRead::Arc *N2)
{return *N1 < *N2;}

void SortArcs(CamiProdRead::Node *N)
{
	sort(N->In.begin(), N->In.end(), ComparePArc);
	sort(N->Out.begin(), N->Out.end(), ComparePArc);
}

void StoreNet::init(const int argc, const char** argv)
{
	if (argc < 2)
		Error("Parameters: [-pnml] <InputFile>");

	if (strcmp(argv[1], "-pnml") == 0)
 		if (argc < 3)
 			Error("Parameters: [-pnml] <InputFile>");
 		else
 			PNML(argv[2], this);
	else if (strcmp(argv[1], "-prod") == 0)
            Error("-prod is not supported anymore");
        else
		CAMIInit(argv[1]);

	
	for_each(Places.begin(), Places.end(), SortArcs);
	for_each(Transitions.begin(), Transitions.end(), SortArcs);
}

StoreNet::StoreNet(const int argc, const char** argv): Places(), Transitions()
{
	init(argc, argv);
}

StoreNet::StoreNet(const char *fileName, bool cami): Places(), Transitions()
{
	int argc;
	const char *argv[3];
	
	if (cami)
	{
		argv[1] = fileName;
		argc = 2;
	}
	else
	{
		argv[1] = "-pnml";
		argv[2] = fileName;
		argc = 3;
	}
	
	init(argc, argv);
}

StoreNet::~StoreNet()
{
	for(NodeList::iterator It = Nodes.begin(); It != Nodes.end(); ++It)
		delete It->second;
}

void StoreNet::Elements::erase(const CamiProdRead::Node *P)
{
//	NodeVector::iterator it =find_if(_Elements.begin(), _Elements.end(), 
//									 bind2nd(equal_to<CamiProdRead::Node *>(), P));

	NodeVector::iterator it = _Elements.begin();
	
	for (; (it != _Elements.end()) && (P != *it); ++it)
		;

	_Elements.erase(it);
}

void StoreNet::SetName(const unsigned int Id, char *Name)
{
/*! \pre \a Id doit �tre connue dans la variable #Nodes. */
	if (Nodes.find(Id) == Nodes.end())
				Error("Unknown Place Id");
	else
				Nodes[Id]->AddName(Name);
};

const string StoreNet::GetName(const unsigned int Id) const
{
/*! \pre \a Id doit �tre connue dans la variable #Nodes, sinon on termine en 
	erreur. */
	
	NodeList::const_iterator it = Nodes.find(Id);
	
	assert(it != Nodes.end());

	return it->second->GetName();
};

template<class T>
class EqualName {
private:
	T Name;
public:
	EqualName(T N): Name(N) {};
	
	bool operator() (const CamiProdRead::Node *P)
	{return P->GetName() == Name;}
};

template<class T>
NodeVector::const_iterator StoreNet::Elements::GetElementNodeHelper(T Name) const
{
	NodeVector::const_iterator it = find_if(_Elements.begin(), _Elements.end(), 
											EqualName<T>(Name));

	return it;
}

CamiProdRead::Node *StoreNet::Elements::GetElementNode(const char *Name) const
{
	NodeVector::const_iterator it = GetElementNodeHelper<const char *>(Name);
	
/*! \post Termine le programme en erreur si la place n'existe pas. */

	assert(it != _Elements.end());

  	return *it;
}

CamiProdRead::Node *StoreNet::Elements::GetElementNode(const string &Name) const
{
	NodeVector::const_iterator it = GetElementNodeHelper<const string &>(Name);
	
/*! \post Termine le programme en erreur si la place n'existe pas. */

	assert(it != _Elements.end());

  	return *it;
}

void StoreNet::AddPlace(const unsigned int Id)
{
	CamiProdRead::Place *P = new CamiProdRead::Place(Id);
	
	Nodes[Id] = (CamiProdRead::Node *)P;
	Places.insert((CamiProdRead::Node *)P);
}

void StoreNet::RemovePlace(const unsigned int Id)
{
	CamiProdRead::Node *P = Nodes[Id];
	
	IsolateNode(Id, true);
	Places.erase(P);
	Nodes.erase(Id);
}

void StoreNet::AddTransition(const unsigned int Id)
{
	CamiProdRead::Transition *P = new CamiProdRead::Transition(Id);
	
	Nodes[Id] = (CamiProdRead::Transition *)P;
	Transitions.insert((CamiProdRead::Transition *)P);
}

void StoreNet::RemoveTransition(const unsigned int Id)
{
	CamiProdRead::Node *P = Nodes[Id];
	
	IsolateNode(Id, true);
	Transitions.erase(P);
	Nodes.erase(Id);
}
void StoreNet::ReadCN(ifstream &File)
{
	unsigned int Nb, Size;
	char         Str[255], C;

	// Object type definition
	
	File >> Size;
	File.read(&C, 1);
	File.read(Str,Size);
	Str[Size] = '\0';
	File.read(&C, 1);

	// Object number definition
	
	File >> Nb;
	File.read(&C, 1);
	File.read(&C, 1);

	// Object creation

	if (strcmp(Str, "net") == 0)
		;
	else if (strcmp(Str, "place") == 0)
		AddPlace(Nb);
	else if (strcmp(Str, "transition") == 0)
		AddTransition(Nb);
	else if (strcmp(Str, "net") != 0)
		Error("Unknown type definition in a CN command");
}

void StoreNet::ReadCT(ifstream &File)
{
	unsigned int Nb, Size;
	char         Str[255], C, *Val;

	Val = (char *)malloc(255);

	if (Val == NULL)
		Error("Cannot allocate memory");
	
	// Field definition
	
	File >> Size;
	File.read(&C, 1);
	File.read(Str,Size);
	Str[Size] = '\0';
	File.read(&C, 1);

	// Object number definition

	File >> Nb;
	File.read(&C, 1);

	// Field valuation

	File >> Size;
	File.read(&C, 1);
	File.read(Val,Size);
	Val[Size] = '\0';
	File.read(&C, 1);
	File.read(&C, 1);

	// Action

	if (strcmp(Str, "name") == 0)
		Nodes[Nb]->AddName(Val);
	else if (strcmp(Str, "marking") == 0)
//	{
		ModifyInitialMarking(Nb, (unsigned int)atoi(Val));
//		free(Val);
//	}
	else if (strcmp(Str, "valuation") == 0)
//	{
		ModifyArc(Nb, (unsigned int)atoi(Val));
//		free(Val);
//	}
	else if (strcmp(Str, "guard") == 0)
	{
	 	if (strcmp(Val, "true") != 0)
			Error("Non true guards are not treated");
	}
        else if (strcmp(Str, "priority") == 0)
        {
                if (strcmp(Val, "0") != 0)
                        Error("Non 0 priorities are not treated");
        }
        else if (strcmp(Str, "domain") == 0)
        {   
                if (strcmp(Val, "null") != 0)   
                        Error("Non null domains are not treated");
        }
	else if ((strcmp(Str, "version") != 0) && (strcmp(Str, "declaration") != 0) && (strcmp(Str, "author(s)") != 0) && (strcmp(Str, "project") != 0) && (strcmp(Str, "title") != 0) && (strcmp(Str, "date") != 0) && (strcmp(Str, "code") != 0))
	{
		cout << Str << endl;
		Error("unrecognized field in CT command");
	}
	
	free(Val);
}

void StoreNet::ReadCA(ifstream &File)
{
	unsigned int Nb, Init, Dest, Size;
	char         Str[255], C;

	// Arc definition
	
	File >> Size;
	File.read(&C, 1);
	File.read(Str,Size);
	Str[Size] = '\0';
	File.read(&C, 1);

	// Arc, Init and Dest numbers

	File >> Nb;
	File.read(&C, 1);
	File >> Init;
	File.read(&C, 1);
	File >> Dest;
	File.read(&C, 1);
	File.read(&C, 1);

	// Arc creation

	if (strcmp(Str, "arc") == 0)
		AddArc(Init, Dest, Nb);
	else
	{
		cout << Str << endl;
		Error("Unkown arc type in a CA command");
	}
}

void StoreNet::FlushLine(ifstream &File)
{
	char C;

	File.read(&C, 1);

	while ((File) && (C != 10) && (C != 13))
		File.read(&C, 1);
}

void StoreNet::CAMIInit(const char *FileName)
{
	ifstream File(FileName);
	char     Op[3];

	if (!File)
		Error("Cannot open input file");

	while (File)
	{
		File.read(Op, 3);
		Op[2] = '\0';

    if (File)
    {
  		if (strcmp(Op, "CN") == 0)
  			ReadCN(File);
	  	else if (strcmp(Op, "PO") == 0)
	  		FlushLine(File);
	  	else if (strcmp(Op, "CT") == 0)
	  	  	ReadCT(File);
	  	else if (strcmp(Op, "PT") == 0)
	  		FlushLine(File);
	  	else if (strcmp(Op, "CA") == 0)
	  		ReadCA(File);
	  	else if (strcmp(Op, "PI") == 0)
	  		FlushLine(File);
	  	else if (strcmp(Op, "DB") == 0)
			FlushLine(File);
		else if (strcmp(Op, "FB") == 0)
			FlushLine(File);
	  	else if (strcmp(Op, "DE") == 0)
			FlushLine(File);
		else if (strcmp(Op, "FE") == 0)
			FlushLine(File);
		else
	  		Error("Unknown CAMI command");
	  }

/*		cout << "Etat pr�sent" << endl;
		Print(); */
	}

	File.close();
}

// void StoreNet::ProdInit(const char *FileName)
// {
// 	extern FILE *cpr_in;
// 	int yyparse(void *N);
// 	
//   if ((cpr_in=fopen(FileName,"r"))==NULL)
//     Error("Cannot open input file");
// 
// 	int i = (int)yyparse((void *)this);
// 
// 	if (i != 0)
// 		Error("Error in input file");
// 
// 	fclose(cpr_in);
// }

void StoreNet:: AddArc(const unsigned int IdInit, const unsigned int IdDest,
					       	const unsigned int IdArc)
{
//	CamiProdRead::Node *Init = Nodes[IdInit];
//	CamiProdRead::Node *Dest = Nodes[IdDest];
	CamiProdRead::Node *Init;
	CamiProdRead::Node *Dest;
	
	NodeList::const_iterator it = Nodes.find(IdInit);
	
	if (it != Nodes.end())
		Init = it->second;
	else
	{
		cerr << "Unknown node reference while creating an arc: " << IdInit << endl;
		exit(1);
	}
	
	it = Nodes.find(IdDest);
	
	if (it != Nodes.end())
		Dest = it->second;
	else
	{
		cerr << "Unknown node reference while creating an arc: " << IdDest << endl;
		exit(1);
	}
	
	CamiProdRead::Arc *A = new CamiProdRead::Arc(Init, Dest, IdArc);
	
	Arcs.insert(A);
	Init->AddOut(A);
	Dest->AddIn(A);
}

template<class T>
struct PrintElement {
	PrintElement() {};
	
	// Impossible de passer const Node *P en param�tre car Print peut modifier 
	//	l'�tat du noeud (calcul de Action si ce n'avait pas �t� fait avant.
	
	void operator() (CamiProdRead::Node *P) {((T)P)->Print();}
};

void StoreNet::Print(void)
{
	cout << Places.NbElements() << " places; ";
	cout << Transitions.NbElements() << " transitions";
	cout << endl;
	
	cout << "Places : " << endl;
	for_each(Places.begin(), Places.end(), PrintElement<CamiProdRead::Place *>());
	
	cout << "Transitions : " << endl;
	for_each(Transitions.begin(), Transitions.end(), 
			 PrintElement<CamiProdRead::Transition *>());
}

/*void StoreNet::PrintCAMI(const char *FileName)
{
	ofstream File(FileName);

	File << "DB()" << endl;

	// Print places

	for(NodeVector::const_iterator It = Places.begin(); It != Places.end(); 
		It++)
	{
		unsigned int Id = (*It)->GetId();
		unsigned int Mark = ((CamiProdRead::Place *)(*It))->GetInitial();
		const string Name = (*It)->GetName();

		File << "CN(5:place," << Id << ")" << endl;
		File << "CN(4:name," << Id << "," << Name.size() << ":" << Name << ")"; 
		File << endl;
		File << "CT(6:domain," << Id << ",4:null)" << endl;

		if (Mark > 0)
		{
			char M[50];

			sprintf(M, "%d", Mark);
			File << "CT(7:marking," << Id << "," << strlen(M) << ":" << Mark;
			File << ")" << endl;
		}
	}

	// Print transitions

	for(NodeVector::const_iterator It = Transitions.begin(); 
		It != Transitions.end(); It++)
	{
		unsigned int Id		= (*It)->GetId();
		const string Name	= (*It)->GetName();

		File << "CN(10:transition," << Id << ")" << endl;
		File << "CN(4:name," << Id << "," << Name.size() << ":" << Name << ")";
		File << endl;
		File << "CT(5:guard," << Id << ",4:true)" << endl;
		File << "CT(8:priority," << Id << ",1:0)" << endl;
	}

	// Print arcs

	for(CamiProdRead::ArcsContainer::iterator It = Arcs.begin(); It != Arcs.end(); It++)
	{
		unsigned int Id = (*It)->GetId();
		unsigned int Source = (*It)->GetStart()->GetId();
		unsigned int Dest = (*It)->GetEnd()->GetId();
		unsigned int Val = (*It)->GetValue();;
		char V[50];

		sprintf(V, "%d", Val);
		File << "CA(3:arc," << Id << "," << Source << "," << Dest << ")";
		File << endl;
		File << "CT(9:valuation," << Id << "," << strlen(V) << ":" << Val;
		File << ")" << endl;
	}

	File << "FB()" << endl;
} */
