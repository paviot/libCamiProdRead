#ifndef __ARCS__

#define __ARCS__

#include <vector>

using namespace std;

class StoreNet;

namespace CamiProdRead {

/*! \brief Classe décrivant les arcs.  */

class Node;

// ********************** Arc definition **********************

class Arc 
{
private:

/*! \var Node *Input
	\brief Noeud de départ de l'arc. */
	
	Node 		*Input;

/*! \var Node *Output
	\brief Noeud d'arrivée de l'arc. */

	Node 		*Output;

/*! \var unsigned int Id
	\brief Identificateur de l'arc. */

	unsigned int Id;
	
/*! \var unsigned int Value
	\brief Valeur associée à l'arc. */
	
	unsigned int Value;

public:

/*! \brief Le constructeur de la classe.

	\param I Noeud de départ de l'arc.
	\param O Noeud d'arrivée de l'arc.
	\param i Identificateur de l'arc (valeur par défaut 0).
	\param V Valeur associée à l'arc (valeur par défaut : 1). */

	Arc(Node *I, Node *O, unsigned int i = 0, unsigned int V = 1): Input(I), Output(O), Id(i), 
																	Value(V) {};

	/*! \brief Retourne le noeud de départ de l'arc. */
	
	Node *GetStart(void) const
	{return Input;};
	
	/*! \brief Retourne le noeud d'arrivée de l'arc. */
	
	Node *GetEnd(void) const
	{return Output;};
	
	/*! \brief Modifie la valeur associée à l'arc. */
	
	void SetValue(unsigned int V)
	{Value = V;};
	
	/*! \brief Retourne la valeur associée à l'arc. */
	
	unsigned int GetValue(void) const
	{return Value;};
	
	/*! \brief Modifie la valeur de l'identificateur. */
	
	void SetId(unsigned int I)
	{Id = I;};
	
	/*! \brief Retourne la valeur de l'identificateur. */
	
	unsigned int GetId(void) const
	{return Id;};
	
	/*! \brief Comparaison de 2 arcs. 
	
		Le test s'effectue d'abord sur l'identité des noeuds de départs des 2 arcs. En cas 
		d'égalité, le test s'effectue sur l'identité des noeuds d'arrivées des 2 arcs. */
	
	bool operator < (const Arc &A) const;
};

/*! \brief Type utilisé pour stocker un ensemble d'arcs. */

typedef vector<Arc *> ArcsVector;

/*! \brief Un container pour les arcs basé sur les vecteurs. */

// ********************** ArcsContainer definition **********************

class ArcsContainer
{
private:

friend class Node;
friend class ::StoreNet;

/*! \var ArcsVector _Arcs
	\brief Liste des arcs. */

		ArcsVector _Arcs;

/*! \brief Ajout d'un arc à la liste des arcs.
	\param P est le pointeur sur l'arc à ajouter.
		
	Cette méthode est utilisée par Node::AddIn, Node::AddOut et StoreNet::AddArc. Elle ne doit pas 
	être utilisée autrement. */

	void insert(CamiProdRead::Arc *P)
	{_Arcs.push_back(P);};
	
/*! \brief On retire un élément de la liste des éléments.

	\param P est le pointeur sur l'arc à retirer.
		
	Cette méthode est utilisée par Node::RemoveIn et Node::RemoveOut. Elle ne doit pas être utilisée
	autrement. */

	void erase(const CamiProdRead::Arc *P);

public:
	
/*! \brief Constructeur. */

	ArcsContainer () {};

/*! \brief Destructeur. */

	~ArcsContainer () {};

/*! \brief Iterateur constant sur les éléments.

	\sa begin, end */

	typedef ArcsVector::const_iterator const_iterator;

/*! \brief Iterateur sur les places.
	
	\sa begin, end */

	typedef ArcsVector::iterator iterator;

/*! \brief Initialisation de l'itérateur sur les éléments. */

	iterator begin(void)
	{return _Arcs.begin();};

/*! \brief Initialisation de l'itérateur constant sur les éléments. */

	const_iterator begin(void) const
	{return _Arcs.begin();};

/*! \brief Valeur de fin de l'itérateur sur les éléments. */

	iterator end(void)
	{return _Arcs.end();};

/*! \brief Valeur de fin de l'itérateur constant sur les éléments. */

	const_iterator end(void) const
	{return _Arcs.end();};

/*! \brief Retourne le nombre de places dans le modèle. */

	unsigned int NbElements(void) const
	{return _Arcs.size();};

/*! \brief Accès direct à la nième place. */

	const CamiProdRead::Arc *operator[](unsigned int i) const
	{return _Arcs[i];};

/*! \brief Sélection suivant l'identificateur.
	\param Id Identificateur de l'arc à chercher.
	\return l'arc trouvé ou NULL.
	
	Utile lors de l'initialisation du CAMI.*/

	Arc *SelectId(unsigned int Id);

/*! \brief Trie les arcs du container. */

	void Sort(void);
};

}

#endif

