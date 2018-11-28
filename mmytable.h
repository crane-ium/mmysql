#ifndef MMYTABLE_H
#define MMYTABLE_H

#include <vector>
#include <stdarg.h>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>
#include "../bplustree_mmap/multimap.h"
#include "../bplustree_mmap/map.h"

extern char DELIMITER;

enum class bugflag {none, light, medium, heavy};
static const bugflag debug = bugflag::none;

/**
 * @brief Initializes a table-file if dne
 */
struct writer{
    writer(const string& filename,
           const vector<string>& fields,
           const char delimiter=DELIMITER)
    {
        ofstream writestream(filename); //try to open
        if(writestream.is_open()){
            for(size_t i = 0; i < fields.size();i++){
                //Insert into the file for field formats
                writestream << delimiter; //choose delimiter here
                writestream << fields[i];
            }
            writestream << delimiter << "\n"; //insert endline delimiter for file format
        }else{
            if(debug == bugflag::none) cout << "[ERROR] Writer: Not able to open\n";
        }
        writestream.close();
    }
    vector<string> _fields;
};

struct record{
    //CTORS
    //we can assume filename is given to mmytable as it's __name
    record(const string& filename="none", const char delimiter=DELIMITER)
        :_open(false), _file(filename), _delimiter(delimiter){
        _fields=0;
        _lastrecord="";
        _lastrecordflag = false;
    }

    //MEMBER FUNCS
    ~record(){_ifstream.close();} //guarantee it closes, hopefully
    void set_file(const string& filename){_file=filename;}

    string& next(){ //Return the next record from file
        if(_lastrecordflag && open()){
            _lastrecord = "";
            for(size_t i = 0; i < _fields; i++){
                //for each field, get those
                /** @todo optimize here **/
                string temp;
                getline(_ifstream, temp);
                if(!_ifstream){
                    _open = false;
                    _ifstream.close();
                    _lastrecord = "\0";
                    _lastrecordflag = true; //don't let more grabs
                    return _lastrecord;
                }
                _lastrecord += temp;
            }
            _lastrecordflag = true;
            return _lastrecord;
        }else if(!_lastrecordflag){
            _lastrecordflag = true;
            return _lastrecord;
        }
//            char* line = _fstream.getline()
        return _lastrecord; //failed to ge tnext;
    }
    bool more(){
        next();
        if(_lastrecord != "\0") //verify that you're not allowing an ungrabble data
            _lastrecordflag = false;
        return (!_lastrecordflag) | (bool)_ifstream;
    }
    bool open(){
        if(!_open){
            _open= true;
            _ifstream.open(_file);
            if(_ifstream.is_open()){
                //We need to grab the field names!
                string allfields;
                getline(_ifstream, allfields);
                _delimiter = allfields[0]; //definition of delimiter is first char in file

                _fieldbytes = allfields.length();
                size_t i_start = 1;
                size_t i;
                for(i = 1; allfields[i] != '\0';i++){
                    if(allfields[i] == _delimiter){
                        //take this as a field
                        _fields++;
                        _fieldnames.push_back(allfields.substr(i_start, i-i_start));
                        i_start = i+1;
                    }
                }
                for(size_t i = 0; i < _fieldnames.size(); i++){
                    cout << _fieldnames[i] << " ";
                }
                //the stream is now on the starting line
                return true;
            }
            _open = false;
            return false;
        }
        return _open;
    }
    size_t field_count() const{return _fields;}

    //VARS
    bool _open;
    string _file;
    ifstream _ifstream; //for initial reading to create table
    fstream _fstream; //for reading AND writing
    //read controls, which are set from reading from file header
    size_t _fields; //# of fields in a file
//        char** _fieldnames; //names of parsed fields
    vector<string> _fieldnames;
    size_t _fieldbytes; //bytes of starting field string
    char _delimiter; //delimiter that seperates fields
    string _lastrecord;
    bool _lastrecordflag; //checks if it's been retrieved yet or not
};

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
    mmytable(const string& table_name="", const string& dir="../");

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
    string __dir; //name of directory
    size_t __table_count;
    /**
     * @brief Read/write file and give line information
     * file header format:
     * [field0]|[field1]|...|[fieldn]
     * id# = bytes to get to start of this line
     * table[field0]
     */

    record rec; //My record
//    void add_field(const string& strs, ...); //REMOVED FEATURE
};

#endif // MMYTABLE_H
