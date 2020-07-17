/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   XML.hpp
 * Author: paviot
 *
 * Created on 19 avril 2019, 13:39
 */

#ifndef PNML_HPP
#define PNML_HPP

#include <string>
#include <vector>

#include <sys/stat.h>

/** \brief La classe de base permettant de lire efficacement un XML
 *
 */

class XML {
	int				fd;		/**< \brief Le descripteur du fichier XML */
	size_t 			length; /**< \brief Taille du fichier */
	char*			mf;		/**< \brief Tableau contenant le fichier XML */
	unsigned int	head;	/**< \brief Position courante dans le tableau mf */
public:
    XML(const char *f);		/**< \brief Le constructeur */
    virtual ~XML();			/**< \brief Le destructeur */
    bool		isFinished() const;	/**< \brief Indique si la lecture est terminée ou non */
    std::string getNextElement();	/**< \brief On se positionne au début du prochain élément et on retourne son nom */
    void		getNextAttribute(std::string& name, std::string& value);	/**< \brief On se positionne au début du prochain attribut et on retourne son nom et sa valeur */
    std::string getToken();	/**< \brief Lecture de la prochaine chaîne */
};

#endif /* PNML_HPP */

