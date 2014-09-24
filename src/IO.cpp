#include <iostream>
#include <sstream>
#include <algorithm>
#include "Node.h"
#include "IO.h"

/* ---------------------- Impression CAMI ---------------------- */

class PrintCamiPlace
{
private:
	ostream &os;
public:
	PrintCamiPlace(ostream &o): os(o) {};
	void operator() (CamiProdRead::Node *P)
	{
		unsigned int Id = P->GetId();
		unsigned int Mark = ((CamiProdRead::Place *)P)->GetInitial();
		const string Name = P->GetName();

		os << "CN(5:place," << Id << ")" << endl;
		os << "CT(4:name," << Id << "," << Name.size() << ":" << Name << ")" << endl;
//		os << "CT(6:domain," << Id << ",4:null)" << endl;

		if (Mark > 0)
		{
			ostringstream conv;

			conv << Mark;
			os << "CT(7:marking," << Id << "," << conv.str().size() << ":" << Mark;
			os << ")" << endl;
		}
	};
};

class PrintCamiTransition
{
private:
	ostream &os;
public:
	PrintCamiTransition(ostream &o): os(o) {};
	void operator() (CamiProdRead::Node *T)
	{
		unsigned int Id		= T->GetId();
		const string Name	= T->GetName();

		os << "CN(10:transition," << Id << ")" << endl;
		os << "CT(4:name," << Id << "," << Name.size() << ":" << Name << ")" << endl;
//		os << "CT(5:guard," << Id << ",4:true)" << endl;
//		os << "CT(8:priority," << Id << ",1:0)" << endl;
	};
};

class PrintCamiArc
{
private:
	ostream &os;
public:
	PrintCamiArc(ostream &o): os(o) {};
	void operator() (CamiProdRead::Arc *A)
	{
		unsigned int Source = A->GetStart()->GetId();
		unsigned int Dest = A->GetEnd()->GetId();
		unsigned int Val = A->GetValue();
		ostringstream conv;

		conv << Val;
		os << "CA(3:arc," << A->GetId() << "," << Source << "," << Dest << ")" << endl;
		os << "CT(9:valuation," << A->GetId() << "," << conv.str().size() << ":" << Val;
		os << ")" << endl;
	};
};

void PrintCami::operator() (void)
{
	os << "DB()" << endl;

	for_each(N.Places.begin(), N.Places.end(), PrintCamiPlace(os));
	for_each(N.Transitions.begin(), N.Transitions.end(), PrintCamiTransition(os));
	for_each(N.Arcs.begin(), N.Arcs.end(), PrintCamiArc(os));

	os << "FB()" << endl;

}
