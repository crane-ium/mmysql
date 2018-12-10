#ifndef MMYTABLE_H
#define MMYTABLE_H

#include <vector>
#include <stdarg.h>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <iomanip>
#include "../bplustree_mmap/multimap.h"
#include "../bplustree_mmap/map.h"
#include "mmyshunting.h"
#include "sethelper.h"
#include "mmyrecord.h"

//HELPER FUNCTIONS
namespace mmyhelper{
    void stream_vecstring(fstream &filestream,
                          const vector<string>& vstr,
                          bool write_idflag=false);
    //Print a table given a 2d vector
    void print_table(const vector<vector<string> >& table,
                     const string& tablename,
                     const vector<size_t> &column_width);
    bool file_exists(const string& filename);
}

/**
 * @brief mmysql database, main-memory storage
 * Uses my multimaps
 * @author Stephen Crane
 * @
 */
class mmytable
{
public:
    mmytable();
    mmytable(const string& filelocation);
    mmytable(const string& table_name, const string& dir);

    //Initialize functionality
    void init(const string& table_name, const string &dir=""); //define table to access
    void field_add_all(const vector<string>& fieldnames);
    void field_add(const string& field_name); //Add field to __itable
    void parse(const string& fileline,
               const char delimiter='|');
    //Turns a line read from file into a vector of strings
    vector<string> vector_parse(const string& fileline,
                                bool wildcard=false, //flag for wildcard call
//                                const vector<mmyint> &ordering,
                                const vector<mmyint> &ordering
                                    =vector<mmyint>());
    bool read_file();
    //Creation
    bool create(bool overwrite=true);
    //Selection
    void select(fstream &filestream, const string &constraints,
                                     vector<string> fields=vector<string>()
                                    );
//    multimap<mmyint, string>& select(const string& constraints);

    void insert(vector<string> &fieldnames);
    friend vector<mmyint>& vector_filter(const string& fieldname,
                                         const string& op,
                                         const string& comp);
    void set_delimiter(const char new_delimiter){DELIMITER=new_delimiter;}
    inline char get_delimiter() const{return DELIMITER;}
//    void create(const string& table_name, const string &strs, ...); //REMOVED FEATURE
private:
    //__itables[fieldname][targetname]
    //OR set it to use a hash to store the multiple tables
    simple_map<string, multimap<string, mmyint> > __itables; //Index Tables

    simple_map<mmyint, string> __fields; //field-column data to read from-to string
//    simple_map<string, mmyint> __field_ids; //key: val pair of fieldname to id# order

    bool __init=false; //already initialized table flag
    string __name; //Name of the table it is storing
    string __dir; //name of directory
    size_t __table_count;
    char __delimiter;

    record rec; //My record
};

#endif // MMYTABLE_H
