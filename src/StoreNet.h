/*
 *  Net.h
 *  Ordre
 *
 *  Created by Emmanuel Paviot-Adet (EPA) on Sat May 18 2002.
 *  Copyright (c) 2001-2008 LIP6. All rights reserved.
 *
 */
 
 /*! \mainpage Bibliothèque CamiProdRead
 	
 	Le but de cette bibliothèque est de fournir une structure de données 
 	permettant de stocker en mémoire un modèle réseau de Petri ordinaire, que 
 	celui-ci ait été décrit en CAMI ou PROD.
 	
 	\section HowToRead Comment lire ce document
 	La classe StoreNet permet de stocker un modèle. L'initialisation de cette 
 	classe correspond à la lecture et au stockage d'un modèle. Elle est située
 	dans le fichier StoreNet.h qui contient quelques définitions de types 
 	supplémentaires permettant la manipulation de la classe. Des itérateurs 
 	permettent de parcourir les places (StoreNet::PlaceIterator et 
 	StoreNet::PlaceConstIterator) et les transitions 
 	(StoreNet::TransitionIterator et StoreNet::TransitionConstIterator).
 	
 	Les places et transitions sont décrits dans deux classes : CamiProdRead::Place et 
 	CamiProdRead::Transition qui héritent d'une même classe CamiProdRead::Node qui décrit des 
 	généralités sur les noeuds d'un graphe (nom, arcs entrants, arcs sortants...). Ces trois 
 	classes sont stockées dans le fichier Node.h.
 	
 	Enfin, les arcs sont décrits dans la classe CamiProdRead::Arc. Un container d'arcs permet la 
 	manipulation d'ensembles d'arcs : CamiProdRead::ArcsContainer. Ces classes se trouvent dans 
 	le fichdier Arcs.h.
 	
 	D'autres fonctionnalités accompagnent le stockage d'un réseau de Petri comme la lecture de 
 	propriétés linéaires via la fonction ParseProperty.
 	
 	Deux classes de stockage de transitions pour effectuer des franchissements existent 
 	aussi : DepthFirst et BreadthFirst dans le fichier Waiting.h. Il en existe une autre :
 	StructuralOrder, mais elle semble buggée.
 	
 	\section Historique
 	- V1.1.7.1 : Modification pour permettre la compatibilité C++11 (avec clang 3.4)
 	- V1.1.7 :	Suppression du support PROD.
 	- V1.1.6 :	Ajout des fonctions de stockage de transitions pour les franchissements
 				(DepthFirst et BreadthFirst).
 	- V1.1.5 :	Correction d'un bug d'initialisation dans le parser.
 	- V1.1.4 :	Petites modifications après un passage sur valgrind.
 	- V1.1.3 :	Réécriture de la classe CamiProdRead::ArcsIterator pour être conforme aux
 				standards.
 	- V1.1.2 :	
 				- Modularisation du parser de propriétés linéaires,
 				- Création de la fonction ParseSum qui permet de lire une somme coefficientée de 
 				places.
 	- V1.1.1 : 
 				- Correction de bugs dans la partie impression vers Cami (classe classe PrintCami),
 				- Préparation à la modularisation du parser de propriétés linéaires. 
 	- V1.1.0 : 	Ajout de la lecture d'une propriété linéaire. 
 	- V1.0.7 :	
 				- Sortie de l'impression de la classe StoreNet pour créer la classe PrintCami.
 				- Manipulation de string en plus des char * pour la recherche des éléments par nom.
 	- V1.0.6 :	Ajout des méthodes CamiProdRead::Node::NbSimpleArcs et 
 	  			CamiProdRead::Node::NbDoubleArcs.
 	- V1.0.5 :	Rectification d'un bug dans la classe CamiProdRead::ArcsIterator (j'avais oublié
 				de trier les Pré et Post avant d'énumérer).
 	- V1.0.4 :	Ajout de la méthode StoreNet::Elements::GetElementNode.
 	- V1.0.3 : 	
 				- Les arcs sont triés dans les noeuds.
 				- Ajout de la classe CamiProdRead::ArcsIterator qui permet d'énumérer les arcs
 				  Pre et Post et de détecter les arcs doubles.
 				- Ajout du champ info dans la classe CamiProdRead::Node qui offre un champ 
 				  libre d'utilisation à l'utilisateur.
 	- V1.0.2 : Node devient une classe abstraite : son constructeur est défini comme protected.
 	- V1.0.1 : Correction d'un bug d'allocation des places et transitions.
 	- V1.0.0 : Modification de l'interface et du code. Les arcs peuvent aussi être accédés 
 				via un container. Ceci termine la refonte de la bibliothèque.
 	- V0.1.3 : Les noms sont maintenant stockés sous forme de string (et non plus de char *
 				à la C). Du coup, les problèmes de fragilités de la bibliothèque (on faisait
 				des copies de pointeurs et pas des copies de chaine de caractères) devraient
 				être résolues de ce côté là. Remplacé yyin par cpr_in (oublié dans la version 
 				précédente).
 	- V0.1.2 : Modifications pour préfixer par cpr_ les noms des fonctions du parser.
 				De cette façon la bibliothèque peut être utilisée par une application
 				utilisant elle-même un parser.
 	- V0.1.1 : Ajout du namespace CamiProdRead dans le fichier Node.h pour éviter 
 				des conflits avec une application de Denis.
 	- V0.1.0 : 	Modification de l'interface par ajout d'itérateurs sur les 
 				places et les transitions. Du coup, un certain nombre de 
 				méthodes deviennent obsolètes. Elles sont signalées dans la doc 
 				et sont gardées tant que toutes les applications n'ont pas été 
 				refondues. Suppression de la méthode permettant de récupérer la
 				liste des arcs.
 	- V0.0.1 :	Ajout d'une documentation.
 	- V0.0.0 : 	Stockage d'un modèle réseau de Petri à partir de CAMI et PROD. 
 				La partie PROD est basée sur un travail de D. Poitrenaud.
 	
 	\section Pb Ce qui ne va pas
 	- Il y a 2 définitions de containers qui devraient hériter d'une même classe.
 	- La méthode CamiProdRead::Place::Print() n'est pas prise en compte par la 
 		méthdoe StoreNet::Print().
 */

/* EPA Mon Nov 17 2003: Extracting net reading part in a library */

#ifndef __STORENET__

#define __STORENET__

#include <map>
#include <vector>
#include <utility>
#include <fstream>
#include "Node.h"

/*! \brief Numéro de version de la bibliothèque. */

#define CAMIPRODREADVERSION "1.1.5"

using namespace std;

/*! \brief Fait correspondre l'identifiant d'un noeud (numéro macao) avec ce 
	noeud. 
	
	ATTENTION : on utilise le fait qu'une map range ses éléments par ordre 
	croissant */

typedef map<const unsigned int, CamiProdRead::Node *> NodeList;

/*! \brief Type utilisé pour stocker un ensemble de places ou de transitions. */

typedef vector<CamiProdRead::Node *> NodeVector;

/*! \brief Un arc est composé de deux identifiants de noeuds. */

typedef pair<unsigned int, unsigned int> Arc;

/*! \brief Fait correspondre l'identifiant d'un arc avec cet arc. */

typedef map<unsigned int, Arc> ArcList;

/*! \brief Cette classe stocke un modèle réseau de Petri et permet d'y accéder 
	et de le stocker dans un fichier CAMI.

	Cette classe effectue aussi le décodage des fichiers CAMI. Pour le 
	décodage des fichiers PROD, un analyseur utilisant flex et bison est 
	utilisé.
	
	ATTENTION : on utilise le fait qu'une map range ses éléments par ordre 
	croissant */

class StoreNet
{
private:
	class Elements
	{
	private:
	
	friend class StoreNet;
	
	/*! \var NodeList _Elements
		\brief Liste des places ou des transitions. Les places sont référencées
		par leur identifiant. */

		NodeVector _Elements;

	/*! \brief Ajout d'un élément à la liste des éléments.

		\param P est le pointeur sur l'élément à ajouter.
		
		Cette méthode est utilisée par StoreNet::AddPlace et 
		StoreNet::AddTransition. Elle ne doit pas être utilisée autrement. */

		void insert(CamiProdRead::Node *P)
		{_Elements.push_back(P);};
	
	/*! \brief On retire un élément de la liste des éléments.

		\param P est le pointeur sur l'élément à retirer.
		
		Cette méthode est utilisée par StoreNet::RemovePlace et 
		StoreNet::RemoveTransition. Elle ne doit pas être utilisée 
		autrement. */

		void erase(const CamiProdRead::Node *P);

		template<class T>
		NodeVector::const_iterator GetElementNodeHelper(T Name) const;

	public:
	
	/*! \brief Constructeur. */
	
		Elements () {};

	/*! \brief Iterateur constant sur les éléments.

		\sa begin, end */

		typedef NodeVector::const_iterator const_iterator;

	/*! \brief Iterateur sur les places.
	
		\sa begin, end */

		typedef NodeVector::iterator iterator;

	/*! \brief Initialisation de l'itérateur sur les éléments. */

		iterator begin(void)
		{return _Elements.begin();};

	/*! \brief Initialisation de l'itérateur constant sur les éléments. */

		const_iterator begin(void) const
		{return _Elements.begin();};

	/*! \brief Valeur de fin de l'itérateur sur les éléments. */

		iterator end(void)
		{return _Elements.end();};

	/*! \brief Valeur de fin de l'itérateur constant sur les éléments. */

		const_iterator end(void) const
		{return _Elements.end();};

	/*! \brief Retourne le nombre de places dans le modèle. */

		unsigned int NbElements(void) const
		{return _Elements.size();};

	/*! \brief Retourne le noeud représentant un élément à partir de son nom.

		\param Name Le nom de l'élément.
		\return Le noeud représentant la place cherchée. */

		CamiProdRead::Node *GetElementNode(const char *Name) const;

	/*! \brief Retourne le noeud représentant un élément à partir de son nom.

		\param Name Le nom de l'élément.
		\return Le noeud représentant la place cherchée. */

		CamiProdRead::Node *GetElementNode(const string &Name) const;

	/*! \brief Retourne l'identificateur d'un élément à partir de son nom.

		\param Name Le nom de l'élément.
		\return Un entier qui est l'identificateur de la place cherchée. */

		unsigned int GetElementId(const char *Name) const
		{return (this->GetElementNode(Name))->GetId();};

	/*! \brief Retourne l'identificateur d'un élément à partir de son nom.

		\param Name Le nom de l'élément.
		\return Un entier qui est l'identificateur de la place cherchée. */

		unsigned int GetElementId(const string &Name) const
		{return (this->GetElementNode(Name))->GetId();};

	/*! \brief Accès direct à la nième place. */

		CamiProdRead::Node *operator[](unsigned int i) const
		{return _Elements[i];};
	};

public:

/*! \var Elements Places
	\brief Le container des places. */
	
	Elements Places;

/*! \var Elements Transitions
	\brief Le container des transitions. */
	
	Elements Transitions;

/*! \var ArcsContainer  Arcs
	\brief Liste de tous les arcs. */

	  CamiProdRead::ArcsContainer Arcs;

protected:

/*! \var NodeList Nodes
	\brief Liste de toutes les noeuds (places et transitions). Les noeuds sont 
	référencés par leur identifiant qui est unique. */

	  NodeList Nodes;

/*! \brief Traitement d'une commande CN (Create Node) lors de la lecture du 
	CAMI. */

		void ReadCN(ifstream &File);

/*! \brief Traitement d'une commande CT (Create Text) lors de la lecture du 
	CAMI. */

		void ReadCT(ifstream &File);

/*! \brief Traitement d'une commande CA (Create Arc) lors de la lecture du 
	CAMI. */

		void ReadCA(ifstream &File);

/*! \brief Lecture jusqu'à la fin de la ligne courante lors de la lecture du
	CAMI. */

		void FlushLine(ifstream &File);
		
/* \brief la fonction d'initialisation qui fait tout le travail. */
		
		void init(const int argc, const char **argv);

/*! \brief Fonction principale d'initialisation de la structure si le modèle est
	donné sous forme d'un fichier CAMI. */

		void CAMIInit(const char *FileName);

/* \brief Fonction principale d'initialisation de la structure si le modèle est
	donné sous forme d'un fichier PROD. */

//		void ProdInit(const char *FileName);

/*! \brief Destruction de tous les arcs liés à un noeud. 
	
	Cette méthode est utilisée lors de la destruction d'un noeud afin de ne pas
	avoir d'arc pendant et de garder la structure cohérente. */

		void IsolateNode(unsigned int Id, bool Place);
		
public:

		// Constructor

/*! \brief Constructeur principal de la classe. 

	Il prend en paramètre, les paramètres de la ligne de commande. Il réalise 
	donc la lecture du modèle. La syntaxe sur la ligne de commande est la 
	suivante : <tt> [-prod] <InputFile></tt>. Par défaut, le fichier lu est un 
	fichier CAMI. S'il s'agit d'un modèle décrit en PROD, un modificateur 
	\c -prod doit figurer devant le nom du fichier. */

		StoreNet(const int argc, const char **argv);

/*! \brief Constructeur de la classe à partir d'un nom de fichier. 

	il prend en paramètre un nom de fichier et in booléen qui indique s'il 
	s'agit d'un fichier CAMI ou PROD. */

		StoreNet(const char *fileName, bool cami);

		// Destructor

/*! \brief Le destructeur de la classe. */

		~StoreNet();

		// Methods
		
		// Nodes manipulations

/*! \brief Donner un nom à un noeud en fonction de son identifiant. 

	\param Id L'identificateur du neoud.
	\param Name Le nom à donner au noeud. */

		void SetName(const unsigned int Id, char *Name);
		
/*! \brief Récupère le nom d'un noeud en fonction de son identifiant. 

	\param Id Identifiant du noeud.
	\return Une chaîne de caractères contenant le nom du noeud. */

		const string GetName(const unsigned int Id) const;

		// Places manipulations

/*! \brief Nombre de places. */

		unsigned int NbPlaces(void) const
		{return Places.NbElements();};

/*! \brief Ajout d'une place. 

	\param Id Identificateur de la place ajoutée. 
	
	Attention, aucune vérification de l'unicité de l'identificateur 
	n'est effectuée lors de l'insertion. */

		void AddPlace(const unsigned int Id);

/*! \brief Destruction d'une place.

	\param Id Identificateur de la place retirée. */

		void RemovePlace(const unsigned int Id);

/*! \brief Retourne l'instance de la classe décrivant la place ayant pour 
	identificateur \a Id. */

		const CamiProdRead::Place &GetPlace(const unsigned int Id)
		{return (const CamiProdRead::Place &)(*(Nodes[Id]));};

/*! \brief Donne une nouvelle valeur à une place désignée par son 
	identificateur.
	
	Par défaut, une place contient initialement 0 jetons.

	\param Id Identificateur de la place.
	\param Val Nouvelle valeur du marquage initial de la place. */

		void ModifyInitialMarking(const unsigned int Id, const int Val)
		{((CamiProdRead::Place *)Nodes[Id])->SetInitial(Val);};

/*! \brief Retourne le marquage initial d'une place désignée par son 
		identificateur. */

		unsigned int GetInitialMarking(const unsigned int Id) 
		{return ((CamiProdRead::Place *)Nodes[Id])->GetInitial();};
		
		// Transitions manipulations

/*! \brief Nombre de places. */

		unsigned int NbTransitions(void) const
		{return Transitions.NbElements();};

/*! \brief Ajout d'une transition. 

	\param Id Identificateur de la transition ajoutée. 
	
	Attention, aucune vérification de l'unicité de l'identificateur 
	n'est effectuée lors de l'insertion. */

		void AddTransition(const unsigned int Id);

/*! \brief Destruction d'une transition.

	\param Id Identificateur de la transition retirée. */

		void RemoveTransition(const unsigned int Id);

/*! \brief Retourne l'instance de la classe décrivant la transition ayant pour 
	identificateur \a Id. */

		const CamiProdRead::Transition &GetTransition(const unsigned int Id)
		{return (const CamiProdRead::Transition &)(*(Nodes[Id]));};
		
		// Arcs manipulation

/*! \brief Ajout d'un arc. 

	Pour utiliser cette méthode il faut fournir un identificateur libre pour le 
	nouvel arc créé. Aucune vérification n'est effectuée sur l'unicité de 
	l'identificateur. Cette méthode est surtout utilisée lors de la lecture d'un
	modèle.

	\param IdInit Identificateur du noeud de départ de l'arc.
	\param IdDest Identificateur du noeud d'arrivé de l'arc.
	\param IdArc Identificateur du nouvel arc. */

		void AddArc(const unsigned int IdInit, const unsigned int IdDest,
								const unsigned int IdArc);

/*! \brief Modification de la valeur associée à l'arc.

	ATTENTION !! Aucune vérification n'est faite sur la validité de 
	l'identificateur. En cas d'erreur, un nouvel enregistrement contenant 
	n'importe quoi est créé. Cela peut introduire des incohérences dans la 
	structure.

	\param IdArc Identificateur de l'arc. 
	\param Val Nouvelle valeur associée à l'arc. */

		void ModifyArc(const unsigned int IdArc, const int Val)
		{Arcs.SelectId(IdArc)->SetValue(Val);};

		// Print facilities

/*! \brief Affichage à l'écran des places et transitions contenus dans la 
	structure. 
	
	Il faudrait penser à remplacer cette méthode par une surcharge de 
	l'opérateur <<. */

		void Print(void);

/* \brief Stockage de la structure dans un fichier CAMI. */

//		void PrintCAMI(const char *FileName);
};

#endif
