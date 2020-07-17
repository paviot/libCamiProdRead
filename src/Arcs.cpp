#include <algorithm>
#include <functional>

#include "Node.h"
#include "Arcs.h"

using namespace CamiProdRead;

// ********************** Arc::operator< **********************

bool Arc::operator< (const Arc &A) const
{
	if (Input->GetId() == A.Input->GetId())
		return Output->GetId() < A.Output->GetId();
	else
		return Input->GetId() < A.Input->GetId();
}


// ********************** ArcsContainer::erase **********************

void ArcsContainer::erase(const Arc *P)
{
	ArcsVector::iterator it;
	
	for (it = _Arcs.begin(); (it != _Arcs.end()) && P != *it; ++it)
		;
//	ArcsVector::iterator it =find_if(_Arcs.begin(), _Arcs.end(), 
//									 bind2nd(equal_to<const Arc *const>(), P));

	_Arcs.erase(it);
}

// ********************** ArcsContainer::SelectId **********************

class IsId
{
	unsigned int Id;

public:

	IsId(unsigned int i): Id(i){};
	bool operator()(const Arc* A){return A->GetId() == Id;};
};

Arc * ArcsContainer::SelectId(unsigned int Id)
{
	ArcsContainer::iterator it = find_if(_Arcs.begin(), _Arcs.end(), IsId(Id));
	
	if (it == _Arcs.end())
		return NULL;
	else
		return *it;
}

// ********************** ArcsContainer::Sort **********************

bool ArcCompare(const Arc *A1, const Arc *A2)
{
	bool Result;
	unsigned int A1Start = A1->GetStart()->GetId();
	unsigned int A2Start = A2->GetStart()->GetId();
	
	if (A1Start < A2Start)
		Result = true;
	else if (A1Start == A2Start)
		Result = A1->GetEnd()->GetId() < A2->GetEnd()->GetId();
	else
		Result = false;

	return Result;
}

void ArcsContainer::Sort(void)
{
	sort(_Arcs.begin(), _Arcs.end(), ArcCompare);
}
