/*
 *  Node.h
 *  Ordre
 *
 *  Created by Emmanuel Paviot-Adet on Tue May 28 2002.
 *  Copyright (c) 2002 LIP6. All rights reserved.
 *
 */

#ifndef __NODE__

#define __NODE__

#include <vector>
#include<sstream>
#include <boost/iterator/iterator_adaptor.hpp>

#include "Arcs.h"

using namespace std;

class StoreNet;

namespace CamiProdRead {

class Node;

// ********************** ArcsIterator definition **********************

class ArcsIterator;

/*! \brief Une classe décrit la relation entre une place et une transition.

	Cette classe est utilisée pour lister tous les arcs. Il est alors facile de detecter 
	les arcsc doubles. */

class ArcsDescription
{
private:
friend class ArcsIterator;

/*! \var Node *N
	\brief Le noeud sur lequel va porter l'énumération. */

	Node *N;

/*! \var bool End
	\brief Indique si on est à la fin. */

	bool End;
		
/*! \var ArcsContainer::iterator itIn
	\brief Pointeur sur les arcs en entrée du noeud. */
	
	ArcsContainer::iterator itIn;

/*! \var ArcsContainer::iterator itOut
	\brief Pointeur sur les arcs en sortie du noeud. */
	
	ArcsContainer::iterator itOut;

/*! \var Node *V
	\brief Le noeud à l'autre bout de l'arc. */

	Node *V;

/*! \var unsigned int In
	\brief Valeur associée à l'arc entrant sur le noeud CamiProdRead::ArcsIterator::N.
	
	0 s'il n'existe pas d'arc entrant. */

	unsigned int In;

/*! \var unsigned int Out
	\brief Valeur associée à l'arc sortant du noeud CamiProdRead::ArcsIterator::N.
	
	0 s'il n'existe pas d'arc sortant. */

	unsigned int Out;

/*! \brief Le constructeur. */

	ArcsDescription(Node *n, bool e): N(n), End(e)
	{};

public:

/*! \brief Retourne le noeud à l'autre bout de l'arc. */

	Node *GetNode(void) const
	{return V;}

/*! \brief Retourne le noeud sur lequel porte l'énumération. */

	Node *GetRefNode(void) const
	{return N;}

/*! \brief Retourne la valeur sur l'arc entrant (du noeud CamiProdRead::ArcsIterator::N
	au noeud CamiProdRead::ArcsIterator::V). */

	unsigned int GetIn(void) const
	{return In;};

/*! \brief Retourne la valeur sur l'arc sortant (du noeud CamiProdRead::ArcsIterator::V
	au noeud CamiProdRead::ArcsIterator::N). */

	unsigned int GetOut(void) const
	{return Out;};

/*! \brief Indique si on a atteint la fin. */

	bool IsEnd(void) const
	{return End;};
};

/*! \brief Un itérateur qui permet d'énumérer tous les arcs, qu'ils soient Pre, Post ou double. */

class ArcsIterator
  : public boost::iterator_adaptor<
        ArcsIterator					// Derived
      , ArcsDescription*				// Base
      , boost::use_default				// Value
      , boost::forward_traversal_tag	// CategoryOrTraversal
    >
{
private:

	ArcsDescription AD;

 public:

/*! \brief Un constructeur. */

	ArcsIterator(Node* const n, bool b)
	  : ArcsIterator::iterator_adaptor_(&AD), AD(n, b) {}

/*! \brief Un constructeur. */

	explicit ArcsIterator(ArcsDescription* p)
	  : ArcsIterator::iterator_adaptor_(p), AD(0, true) {}

/*! \brief Retourne le noeud à l'autre bout de l'arc. */

        Node *GetNode(void) const
        {return AD.V;}

/*! \brief Retourne le noeud sur lequel porte l'énumération. */

        Node *GetRefNode(void) const
        {return AD.N;}

/*! \brief Retourne la valeur sur l'arc entrant (du noeud CamiProdRead::ArcsIterator::N
        au noeud CamiProdRead::ArcsIterator::V). */

        unsigned int GetIn(void) const
        {return AD.In;};

/*! \brief Retourne la valeur sur l'arc sortant (du noeud CamiProdRead::ArcsIterator::V
        au noeud CamiProdRead::ArcsIterator::N). */

        unsigned int GetOut(void) const
        {return AD.Out;};

/*! \brief Indique si on a atteint la fin. */

        bool IsEnd(void) const
        {return AD.End;};

/*! \brief Initialisation de l'énumération. */

	ArcsIterator begin(void);

/*! \brief Terminaison de l'énumération. */

	ArcsIterator end(void) const;

private:
	friend class boost::iterator_core_access;
    
/*! \brief Implementation de l'operateur ++. */

	void increment();

/*! \brief Implementation de l'opérateur ==. */

	bool equal(ArcsIterator::iterator_adaptor_ const& x) const;
};

// ********************** Node definition **********************

/*! \brief Définition du type du noeud. */

enum NodeType {
	PL,			/*!< Noeud de type place. */
	TR			/*!< Noeud de type transition. */
};

/*! \brief Cette classe décrit un noeud (place ou transition) de façon générale.
	
	Il s'agit d'une classe abstraite : elle n'est pas utilisée directement, mais au travers des 
	deux classes qui héritent d'elle : CamiProdRead::Place et CamiProdRead::Transition. */

class Node
{
protected:

/*! \var unsigned int Id
	\brief L'identificateur du noeud. */

	unsigned int Id;

/*! \var char *Name
	\brief Le nom du noeud. */

	string Name;

/*! \var NodeType Type
	\brief Type de noeud. */

	NodeType Type;

/*! \var int Info
	\brief Attribut servant à stocker une valeur associée au noeud.
	
	Ce champ n'est pas utilisé par la bibliothèque. L'utilisateur peut s'en servir directement 
	ou pour stocker un indice dans un tableau. */

	int Info;

/*! \brief Le constructeur de la classe. 

	\param i Identificateur du nouveau noeud.
	\param T Type de noeud. 

	Le constructeur est défini comme protected pour éviter une instanciation directe. Seules les 
	classes héritées (CamiProdRead::Place et CamiProdRead:Transition) peuvent être instanciées. */

	Node(const unsigned int i, NodeType T):Id(i), Type(T), EnumerateArcs(this, false) {};

public:
	
/*! \var ArcsContainer In
	\brief Liste des arcs entrant dans le noeud. */

	ArcsContainer In;

/*! \var ArcsContainer Out
	\brief Liste des arcs sortant du noeud. */

	ArcsContainer Out;

/*! \var ArcsIterator EnumerateArcs
	\brief Permet d'énumérer tous les arcs et de déterminer quels sont les arcs doubles. */

	ArcsIterator EnumerateArcs;

/*! \brief Le destructeur de la classe. */

	virtual ~Node(void);

/*! \brief Retourne le type du noeud. */

	NodeType GetType(void) const
	{return Type;};

/*! \brief Retourne l'identificateur du noeud. */
	
	unsigned int GetId(void) const
	{return Id;};

/*! \brief Ajout d'un arc entrant.

	\param A L'arc à ajouter. */

	void AddIn(Arc *A)
	{In.insert(A);};

/*! \brief Suppression d'un arc entrant.

	\param A L'arc à retirer. */

	void RemoveIn(const Arc *A)
	{In.erase(A);};

/* \brief Retourne la liste des arcs en entrée. */

/*	const ArcValuation &LinksIn(void) const
	{return In;}; */

/*! \brief Ajout d'un arc sortant.

	\param A L'arc à ajouter. */

	void AddOut(Arc *A)
	{Out.insert(A);};

/*! \brief Suppression d'un arc sortant.

	\param A L'arc à retirer. */

	void RemoveOut(const Arc *A)
	{Out.erase(A);};

/* \brief Retourne la liste des arcs en sortie. */

/*	const ArcValuation &LinksOut(void) const
	{return Out;}; */

/*! \brief Recherche d'un arc entrant.

	\param N Le noeud à l'autre bout de l'arc.
	\return La valuation associée à l'arc. */

	unsigned int GetIn(const Node *N);

/*! \brief Recherche d'un arc sortant.

	\param N Le noeud à l'autre bout de l'arc.
	\return La valuation associée à l'arc. */

	unsigned int GetOut(const Node *N);

/*! \brief Donne un nom au noeud. 
	
	\param N Le nouveau nom du noeud.*/

	void AddName(char *N)
	{Name = N;};

/*! \brief Donne le nom du noeud. 

	\return Le nom du noeud. */

	const string GetName(void) const
	{return Name;};

/*! \brief Met une valeur dans le champ Info.

	\param i La nouvelle valeur du champ info. */

	void SetInfo(int i)
	{Info = i;};

/*! \brief Retourne la valeur du champ info. */

	int GetInfo(void) const
	{return Info;};

/*! \brief Affichage d'un noeud. */

	virtual void Print(void);

/*! \brief Compte le nombre d'arcs simples. */

	int NbSimpleArcs(void);

/*! \brief Compte le nombre d'arcs doubles. */

	int NbDoubleArcs(void);
};

// ********************** Transition definition **********************

/*! \brief Macro utilisée lors de l'évaluation de la précondition d'une 
	transition.

	NOTINVOLVED est utilisée si la place testée n'est pas précondition de la
	transition. 
	\sa Transition::IsPrecondValid */

#define NOTINVOLVED 0

/*! \brief Macro utilisée lors de l'évaluation de la précondition d'une 
	transition.

	VALID est utilisée si la place testée contient suffisamment de jetons pour
	le franchissement de la transition. 
	\sa Transition::IsPrecondValid */

#define VALID       1

/*! \brief Macro utilisée lors de l'évaluation de la précondition d'une 
	transition.

	INVALID est utilisée si la place testée ne contient pas suffisamment de 
	jetons pour le franchissement de la transition. 
	\sa Transition::IsPrecondValid */

#define INVALID     2

/*! \brief Cette classe contient la description d'une transition. */

class Transition:public Node
{
public:

/*! \brief Constructeur de la classe.

	Initialise Node::Id 
	\param Id Identificateur de la transition. */

	Transition(const unsigned int Id):Node(Id, TR) 
	{};

/*! \brief Permet de tester si une place contient suffisamment de jetons pour 
	permettre le franchissement de la transition.
	
	\param N Le noeud décrivant la place.
	\param Val Nombre de jetons dans la place \a N.
	\return L'une des valeurs suivantes :
		- #NOTINVOLVED,
		- #VALID,
		- #INVALID. */

	char IsPrecondValid(const Node *N, const unsigned int Val);

/*! \brief Retourne le nombre de préconditions de la transition. */

	unsigned int NbPrecond(void)
	{return In.NbElements();};
};

// ********************** Place definition **********************

/*! \brief Cette classe contient la description d'une place. */

class Place:public Node
{
private:

/*! \var unsigned int Initial
	\brief Marquage initial. */

	unsigned int Initial;

/*! \var unsigned int Position
	\brief Variable non utilisée. 
	
	A l'origine, cette bibliothèque était conçue pour une application de calcul
	des états accessibles en utilisant les DDDs. Le but de cette variable était
	de stocker la position de la place dans la liste des places (en vue de 
	trouver le meilleur ordre possible sur ces places, ce projet n'a jamais
	vu le jour sous cette forme). */

	unsigned int Position;

public:

/*! \brief Constructeur de la classe.

	Initialise les variables Node::Id et Place::Initial.
	
	La variable Place::Initial est initialisée à 0.
	\param Id Identificateur de la nouvelle classe. */

	Place(const unsigned int Id):Node(Id, PL) {Initial = 0;};

/*! \brief Modifie la valeur du marquage initial.
	\param i Nouvelle valeur du marquage initial. */

	void SetInitial(const unsigned int i)
	{Initial = i;};

/*! \brief Retourne la valeur du marquage initial. */

	unsigned int GetInitial(void) const
	{return Initial;};

/*! \brief Non utilisé.
	\sa Place::Position */

	void SetPosition(const unsigned int Pos)
	{Position = Pos;};

/*! \brief Non utilisé.
	\sa Place::Position */

	unsigned int GetPosition(void) const
	{return Position;};

void Print(void);

};

}

#endif
