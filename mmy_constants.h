#ifndef MMY_CONSTANTS_H
#define MMY_CONSTANTS_H

#include <set>
#include <string>

//sorted by precedence, from lowest to highest
enum class debugger{none, light, medium, heavy};
enum class tokentype {none, fieldname, comparee, comparitor, boolean, ids};
//booleantype identifiers are allcaps only because lowercase are reserved
enum class booleantype {NONE, OR, AND, XOR, NOT, INVALID, }; //precedence flags
enum class unarytype {NOT}; //not sure i'll use this, just in case <3
static const std::set<std::string> BBOOLEANS{"not", "xor", "and", "or"}; //binary
static const std::set<std::string> UBOOLEANS{"not"}; //unary
static const std::set<std::string, std::greater<std::string> > COMPARITORS{"==", "=", "!=", "<=", ">=", "<", ">"};
static const std::set<std::string> PREOPERATORS{")", " "};
static const std::set<std::string> POSTOPERATORS{"(", " "};
static const std::set<std::string> BLOCKSKIP{"\""};


#endif // MMY_CONSTANTS_H
