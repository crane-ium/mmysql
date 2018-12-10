#ifndef MMY_HELPER_FUNCTIONS_H
#define MMY_HELPER_FUNCTIONS_H

#include <algorithm>
#include <iostream>
#include <vector>
#include "mmy_constants.h"


//Get the type of the boolean given the string
booleantype get_booleantype(const std::string& s);

//put a string into lowercase for consistent comparisons
std::string get_lowercase(std::string s);

//turns the given string lowercase
std::string& transform_lowercase(std::string& s);

//put a string into lowercase for consistent comparisons
std::string get_uppercase(std::string s);

//turns the given string lowercase
std::string& transform_uppercase(std::string& s);



#endif // MMY_HELPER_FUNCTIONS_H
