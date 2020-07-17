/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   XML.cpp
 * Author: paviot
 * 
 * Created on 19 avril 2019, 13:39
 */

#include "XML.h"

#include <vector>
#include <string>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "unistd.h"


#define RETURN 10
#define BUFSIZE 1024

using namespace std;

XML::XML(const char * f): head(0) {
    fd = open(f, O_RDWR);
    
    if (fd == -1)
    {
    	cerr << "File: " << f << endl;
        perror("open");
        exit(1);
    }
    
    // Obtenir la taille du fichier
    
    struct stat st;
    
    if (fstat(fd, &st) == -1)
    {
        perror("fstat");
        exit(2);
    }
    
    length = st.st_size;

    // Mettre le fichier en mémoire

    mf = static_cast<char *>(mmap(NULL, length, PROT_READ|PROT_WRITE,
                                  MAP_PRIVATE,fd, 0u));
    
    if (mf == MAP_FAILED)
    {
        perror("mmap");
        exit(3);
    }
}

XML::~XML() {
	if (close(fd) == -1)
	    {
	        perror("close");
	        exit(4);
	    }

	    if (munmap(mf, length) == -1)
	    {
	        perror("munmap");
	        exit(5);
	    }
}

bool XML::isFinished() const
{
	return (head >= length);
}

string XML::getNextElement()
{
	while ((head < length) &&
		   ((mf[head] != '<') ||
		    ((mf[head] == '<') && (mf[head+1] == '/'))))
		++head;

	++head;

	return getToken();
}

string XML::getToken()
{
	string 	result;
	char*	base;
	int		s = 0;

	while ((head < length) && ((mf[head] == ' ') || (mf[head]=='\t') || (mf[head] == '<') || (mf[head] == '>')  || (mf[head] == '"') || (mf[head] == RETURN)))
		++head;

	base = mf + head;

	while ((head < length) && (mf[head] != ' ') && (mf[head]!='\t') && (mf[head] != '<') && (mf[head] != '>')  && (mf[head] != '"') && (mf[head] != RETURN))
	{
		++s;
		++head;
	}

	result.assign(base, s);

	return result;
}

void XML::getNextAttribute(string& name, string& value)
{
	bool equal = false;
	char *base;
	int s = 0;

	while ((head < length) &&
		   ((mf[head]==' ') || (mf[head]=='\t') || (mf[head]==RETURN) || (mf[head]=='"')))
		++head;

	if (mf[head]=='>')
	{
		cerr << "No attribute found !" << endl;
		exit(6);
	}

//	name = getToken();
	base = mf + head;

	while ((head < length) && (mf[head]!='='))
	{
		++head;
		++s;
	}

	name.assign(base,s);

	while ((head < length) &&
		   ((mf[head]==' ') || (mf[head]=='=') || (mf[head]=='\t') || (mf[head]==RETURN) ||  (mf[head]=='"')))
	{
		equal = ((equal) || (mf[head]=='='));
		++head;
	}

	if ((!equal) || (mf[head]=='>'))
	{
		cerr << "No value found for attribute " << name << endl;
		exit(7);
	}
	value = getToken();
}
