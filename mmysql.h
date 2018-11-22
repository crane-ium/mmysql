/**
  * @file mmysql.h
  */
#ifndef MMYSQL_H
#define MMYSQL_H

#include <sstream>
#include <iostream>

using namespace std;

/**
 * @brief User-Text-Interface for the mmysql class
 *
 */

class mmysql
{
public:
    mmysql();
};


class mmysql_user{
public:
    mmysql_user();
private:
    mmysql __sql;
    string __history;
};

#endif // MMYSQL_H
