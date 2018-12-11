#ifndef MMYRECORD_H
#define MMYRECORD_H

#include <fstream>
#include <vector>
#include "../bplustree_mmap/map.h"
#include "mmy_constants.h"

/**
 * @brief Initializes a table-file if dne
 *
 */
struct writer{
    writer(){}
    inline bool write(const string& filename,
                const vector<string>& fields,
                const char delimiter=DELIMITER)
    {
        bool success;
        ofstream writestream(filename+".bin", ios::binary); //try to open
        if(writestream.is_open()){
//            writestream << delimiter;
//            mmyhelper::stream_vecstring(writestream,fields);
            success = true;
            for(size_t i = 0; i < fields.size();i++){
                //Insert into the file for field formats
                writestream << delimiter; //choose delimiter here
                writestream << fields[i];
            }
            writestream << delimiter << "\n"; //insert endline delimiter for file format
        }else{
            if(debug == bugflag::none) cout << "[ERROR] Writer: Not able to open\n";
            success=false;
        }
        writestream.close();
        return success;
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
    //Returns the string for the line at bytes
    string& get_line(const mmyint bytes){
        if(open()){
            _ifstream.seekg(bytes);
            getline(_ifstream, _lastrecord);
//            cout << "LAST REC: " << bytes << ", " << _lastrecord << endl;
            return _lastrecord;
        }
    }
    bool set_file(const string& filename){
        if(_init)
            return false;
        _file=filename;
        init();
        return true;
    }
    bool insert(){
        //insert empty data
        vector<string> emptyvec{""};
        return insert(emptyvec);
    }
    unsigned long insert(vector<string>& svec){
        fstream fs(_file+".bin", ios::binary | ios::out | ios::app | ios::ate);
        unsigned long fsize = fs.tellg(); //get file size
        //I will manually add +".bin" because we are also only declaring
        //binary flag at the same location.
        if(!fs.is_open()){
            if(debug == bugflag::none) cout << "[ERROR] record.insert: file dne\n";
            return 0;
        }
        //should not have an id associated, yet. We will give it an ID#
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
        return fsize; //inserted successfully
//    }
    }
    string& next(){ //Return the next record from file
        if(open() && _lastrecordflag){
            _lastrecord = "";
            if(debug==bugflag::medium) cout << "Fields: " << _fields << endl;

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
    //Using record.next(), parse the given string and turn it into vector
    vector<string> next_vector(){
        next();
        //Calling next() gave us an updated _lastrecord
        //lit: left_it
        auto lit = _lastrecord.begin();
        size_t count = 0;
        size_t length = 0; //Let's keep track of length

        vector<string> result;
        for(auto it=_lastrecord.begin(); it!=_lastrecord.end();it++){
            length++;
            if((*it) == DELIMITER){
                if(count>0){
                    result.push_back(string(lit, it));
                    if(length-1 > _longest[count-1])
                        _longest[count-1] = length-1;
                }
                lit = it+1;
                count++;
                length=0;
            }
        }
        return result;
    }
    //Returns the length of the longest string in the table
    vector<size_t> longest() const{return _longest;}
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
            _init = true;
            initstream.close();
            _longest.resize(_fields);
        }else{
            if(debug == bugflag::none) cout << "[FAIL] record.init cannot open " << _file+".bin" << endl;
        }
    }
    bool open(){
        if(!_open){
            init(); //initialize record from data retrieved from file
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
    vector<size_t> _longest; //keep track of the longest word per field there is
};

#endif // MMYRECORD_H
