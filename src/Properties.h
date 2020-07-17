/*
 * GetProperties.h
 *
 *  Created on: Jul 5, 2019
 *      Author: paviot
 */

#ifndef SRC_PROPERTIES_H_
#define SRC_PROPERTIES_H_

#include <vector>
#include <string>
#include <utility>

#include <boost/variant.hpp>

typedef boost::make_recursive_variant<
		int,
		std::string,
		std::vector<boost::recursive_variant_>
		>::type Formula;

typedef std::pair<std::string,Formula> Property;

class Properties {
private:
	std::vector<Property> prop;
public:
	Properties(const char *f);
	virtual ~Properties();
	void print() const;
	const std::string& getName(unsigned int id) {return prop[id].first;}
	const Formula& getFormula(unsigned int id) {return prop[id].second;}
private:
	void addTerminalFormula(std::vector<Formula> &f, const std::vector<Formula> &term);
};

#endif /* SRC_PROPERTIES_H_ */
