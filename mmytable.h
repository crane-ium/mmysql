#ifndef MMYTABLE_H
#define MMYTABLE_H

#include <vector>
#include <stdarg.h>
#include <cstdlib>
#include "../bplustree_mmap/multimap.h"
#include "../bplustree_mmap/map.h"

enum class bugflag {none, light, medium, heavy};
static const bugflag debug = bugflag::none;
/**
 * @brief mmysql database, main-memory storage
 * Uses my multimaps
 * @author Stephen Crane
 * @
 */
class mmytable
{
public:
    typedef long mmyint;
    mmytable(const string& table_name="");

    void init(const string& table_name); //define table to access
    void field_add(const string& field_name); //Add field to __itable
    void parse(const string& fileline, const size_t length=128, const char delimiter='|');

//    void create(const string& table_name, const string &strs, ...); //REMOVED FEATURE
//private:
    //__itables[fieldname][targetname]
    //OR set it to use a hash to store the multiple tables
    simple_map<string, multimap<string, mmyint> > __itables; //Index Tables

    simple_map<mmyint, string> __fields; //field-column data to read from-to string

    string __name; //Name of the table it is storing
    size_t __table_count;


//    void add_field(const string& strs, ...); //REMOVED FEATURE
};

#endif // MMYTABLE_H
