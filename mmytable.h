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
        ofstream writestream(filename+".bin", ios::binary); //try to open
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

/**
 * @brief Read/write file and give line information
 * file header format:
 * [field0]|[field1]|...|[fieldn]
 * id# = bytes to get to start of this line
 * table[field0]
 */
struct record{
    //CTORS
    //we can assume filename is given to mmytable as it's __name
    record(const char delimiter=DELIMITER)
        :_open(false), _init(false), _delimiter(delimiter){
        _fields=0;
        _fieldbytes = 0;
        _lastrecord="";
        _lastrecordflag = false;
    }
    record(const string& filename, const char delimiter=DELIMITER)
        :_open(false), _init(false), _file(filename), _delimiter(delimiter){
        _fields=0;
        _fieldbytes = 0;
        _lastrecord="";
        _lastrecordflag = false;
        init(); //initialize record from file
    }
    //read iterator
    /** @todo **/
    friend class readit;
    class readit{
    public:
        readit();
    private:

    };
    //MEMBER FUNCS
    ~record(){_ifstream.close();} //guarantee it closes, hopefully
    char get_delimiter() const{
        if(_init)
            return _delimiter;
        else
            return '\0';
    }
    vector<string>& get_fieldnames(){
        return _fieldnames;
    }
    size_t get_fields() const{
        return _fields;
    }
    void set_file(const string& filename){
        if(_init)
            return;
        _file=filename;
        init();
    }
    bool insert(){
        //insert empty data
        vector<string> emptyvec{""};
        return insert(emptyvec);
    }
    bool insert(vector<string>& svec){
//        init();
        //Verify that the svec fits the dataframe
//    if(svec.size() == _fields - 1){
        fstream fs(_file+".bin", ios::binary | ios::out | ios::app | ios::ate);
        unsigned long fsize = fs.tellg(); //get file size
        //I will manually add +".bin" because we are also only declaring
        //binary flag at the same location.
        if(!fs.is_open()){
            if(debug == bugflag::none) cout << "[ERROR] record.insert: file dne\n";
            return false;
        }
        //should not have an id associated, yet. We will give it that
        fs << fsize << _delimiter;
        //then insert new data at end
        vector<string>::iterator it = svec.begin();
        for(size_t i = 0; i < _fields; i++){
            if(it != svec.end()){
                fs << (*it);
                it++;
            }
            fs << _delimiter;
        }
        fs << '\n'; //endline
        fs.close();
        return true; //inserted successfully
//    }
    }
    string& next(){ //Return the next record from file
        if(open() && _lastrecordflag){
            _lastrecord = "";
            if(debug==bugflag::medium) cout << "Fields: " << _fields << endl;
//            for(size_t i = 0; i < _fields; i++){
//                //for each field, get those
//                /** @todo optimize here **/
//                string temp;
//                getline(_ifstream, temp, _delimiter); //binary file naturally no '\0'

//                _lastrecord += temp;
//            }

            getline(_ifstream, _lastrecord);
            if(!_ifstream){
                _open = false;
                _ifstream.close();
                _lastrecord = "\0";
                _lastrecordflag = true; //don't let more grabs
                return _lastrecord;
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
    void init(){
        if(_init) //protect from re-init
            return;
        if(debug >= bugflag::light) cout << "record init()\n";
        ifstream initstream(_file+".bin", ios::binary);
        if(initstream.is_open()){
            //We need to grab the field names!
            string allfields;
            getline(initstream, allfields);
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
            //the stream is now on the starting line
            //know how much to skip immediately
            _fieldbytes = initstream.tellg();
            cout << "set fbytes: " << _fieldbytes << endl;
            _init = true;
            initstream.close();
        }else{
            if(debug == bugflag::none) cout << "[FAIL] record.init cannot open\n";
        }
    }
    bool open(){
        if(!_open){
            init(); //initialize record from data retrieved from file
            cout << "fbytes: " << _fieldbytes << endl;
            _open= true;
            _ifstream.open(_file+".bin", ios::binary);
            if(_ifstream.is_open()){
                _ifstream.ignore(_fieldbytes); //ignore initial bytes
                return true;
            }
            _open = false;
            return false;
        }
        return _open;
    }
    size_t field_count() const{return _fields;}

    //VARS
    bool _open, _init; //checks if file is open, or already initialized
    string _file;
    ifstream _ifstream; //for initial reading to create table
    fstream _fstream; //for reading AND writing
    //read controls, which are set from reading from file header
    size_t _fields; //# of fields in a file
//        char** _fieldnames; //names of parsed fields
    vector<string> _fieldnames;
    unsigned long _fieldbytes; //bytes of starting field string
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
    mmytable();
    mmytable(const string& filelocation);
    mmytable(const string& table_name, const string& dir);

    void init(const string& table_name); //define table to access
    void field_add_all(const vector<string>& fieldnames);
    void field_add(const string& field_name); //Add field to __itable
    void parse(const string& fileline, const char delimiter='|');

    void insert(const vector<string>& fieldnames);

//    void create(const string& table_name, const string &strs, ...); //REMOVED FEATURE
//private:
    //__itables[fieldname][targetname]
    //OR set it to use a hash to store the multiple tables
    simple_map<string, multimap<string, mmyint> > __itables; //Index Tables

    simple_map<mmyint, string> __fields; //field-column data to read from-to string

    bool __init; //already initialized table flag
    string __name; //Name of the table it is storing
    string __dir; //name of directory
    size_t __table_count;
    char __delimiter;

    record rec; //My record
//    void add_field(const string& strs, ...); //REMOVED FEATURE
};

#endif // MMYTABLE_H
