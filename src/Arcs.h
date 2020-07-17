#ifndef __ARCS__

#define __ARCS__

#include <vector>

using namespace std;

class StoreNet;

namespace CamiProdRead {

/*! \brief Classe d�crivant les arcs.  */

class Node;

// ********************** Arc definition **********************

class Arc 
{
private:

/*! \var Node *Input
	\brief Noeud de d�part de l'arc. */
	
	Node 		*Input;

/*! \var Node *Output
	\brief Noeud d'arriv�e de l'arc. */

	Node 		*Output;

/*! \var unsigned int Id
	\brief Identificateur de l'arc. */

	unsigned int Id;
	
/*! \var unsigned int Value
	\brief Valeur associ�e � l'arc. */
	
	unsigned int Value;

public:

/*! \brief Le constructeur de la classe.

	\param I Noeud de d�part de l'arc.
	\param O Noeud d'arriv�e de l'arc.
	\param i Identificateur de l'arc (valeur par d�faut 0).
	\param V Valeur associ�e � l'arc (valeur par d�faut : 1). */

	Arc(Node *I, Node *O, unsigned int i = 0, unsigned int V = 1): Input(I), Output(O), Id(i), 
																	Value(V) {};
	
	/*! \brief Retourne le noeud de d�part de l'arc. */
	
	Node *GetStart(void) const
	{return Input;};
	
	/*! \brief Retourne le noeud d'arriv�e de l'arc. */
	
	Node *GetEnd(void) const
	{return Output;};
	
	/*! \brief Modifie la valeur associ�e � l'arc. */
	
	void SetValue(unsigned int V)
	{Value = V;};
	
	/*! \brief Retourne la valeur associ�e � l'arc. */
	
	unsigned int GetValue(void) const
	{return Value;};
	
	/*! \brief Modifie la valeur de l'identificateur. */
	
	void SetId(unsigned int I)
	{Id = I;};
	
	/*! \brief Retourne la valeur de l'identificateur. */
	
	unsigned int GetId(void) const
	{return Id;};
	
	/*! \brief Comparaison de 2 arcs. 
	
		Le test s'effectue d'abord sur l'identit� des noeuds de d�parts des 2 arcs. En cas 
		d'�galit�, le test s'effectue sur l'identit� des noeuds d'arriv�es des 2 arcs. */
	
	bool operator < (const Arc &A) const;
};

/*! \brief Type utilis� pour stocker un ensemble d'arcs. */

typedef vector<Arc *> ArcsVector;

/*! \brief Un container pour les arcs bas� sur les vecteurs. */

// ********************** ArcsContainer definition **********************

class ArcsContainer
{
private:

friend class Node;
friend class ::StoreNet;

/*! \var ArcsVector _Arcs
	\brief Liste des arcs. */

		ArcsVector _Arcs;

/*! \brief Ajout d'un arc � la liste des arcs.
	\param P est le pointeur sur l'arc � ajouter.
		
	Cette m�thode est utilis�e par Node::AddIn, Node::AddOut et StoreNet::AddArc. Elle ne doit pas 
	�tre utilis�e autrement. */

	void insert(CamiProdRead::Arc *P)
	{_Arcs.push_back(P);};
	
/*! \brief On retire un �l�ment de la liste des �l�ments.

	\param P est le pointeur sur l'arc � retirer.
		
	Cette m�thode est utilis�e par Node::RemoveIn et Node::RemoveOut. Elle ne doit pas �tre utilis�e
	autrement. */

	void erase(const CamiProdRead::Arc *P);

public:
	
/*! \brief Constructeur. */

	ArcsContainer ():_Arcs() {};

/*! \brief Destructeur. */

	~ArcsContainer () {};

/*! \brief Iterateur constant sur les �l�ments.

	\sa begin, end */

	typedef ArcsVector::const_iterator const_iterator;

/*! \brief Iterateur sur les places.
	
	\sa begin, end */

	typedef ArcsVector::iterator iterator;

/*! \brief Initialisation de l'it�rateur sur les �l�ments. */

	iterator begin(void)
	{return _Arcs.begin();};

/*! \brief Initialisation de l'it�rateur constant sur les �l�ments. */

	const_iterator begin(void) const
	{return _Arcs.begin();};

/*! \brief Valeur de fin de l'it�rateur sur les �l�ments. */

	iterator end(void)
	{return _Arcs.end();};

/*! \brief Valeur de fin de l'it�rateur constant sur les �l�ments. */

	const_iterator end(void) const
	{return _Arcs.end();};

/*! \brief Retourne le nombre de places dans le mod�le. */

	unsigned int NbElements(void) const
	{return _Arcs.size();};

/*! \brief Acc�s direct � la ni�me place. */

	const CamiProdRead::Arc *operator[](unsigned int i) const
	{return _Arcs[i];};

/*! \brief S�lection suivant l'identificateur.
	\param Id Identificateur de l'arc � chercher.
	\return l'arc trouv� ou NULL.
	
	Utile lors de l'initialisation du CAMI.*/

	Arc *SelectId(unsigned int Id);

/*! \brief Trie les arcs du container. */

	void Sort(void);
};

}

#endif

