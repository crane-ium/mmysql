/**
  * @file mmysql.h
  */
#ifndef MMYSQL_H
#define MMYSQL_H

#include <sstream>
#include <iostream>
#include "mmytable.h" //read/writes from/to database. SQL gives commands

using namespace std;

/**
 * @brief Interact with database
 */
class mmysql
{
public:
    //CTOR
    mmysql();

    //MEMBER FUNCTIONS

    //return a 2d table with with requested fields
//    vector<vector<string> >& select(vector<string>& fields);
private:
//    simple_map<string, mmytable> __tables; //Multitable access
    simple_map<string, mmytable> __database;
    string __line;
    string __constraint;
    vector<string> __history; //History of user's data
};

class mmysql_user{
public:
    mmysql_user();
private:
    mmysql __sql;
    string __history;
};

#endif // MMYSQL_H
