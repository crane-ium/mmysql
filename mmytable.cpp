/**
  * @file mmytable.cpp
  * @header mmytable.h
  */
#include "mmytable.h"

char DELIMITER = '|';

mmytable::mmytable(){
    assert(true); /** @todo **/
}
mmytable::mmytable(const string& filelocation)
    : __name(filelocation), __dir(""){

    if(!read_file())
        cout << "Can't read\n";
}

mmytable::mmytable(const string& table_name, const string &dir)
    : __name(table_name), __dir(dir)
{
    //Based on the table_name, open that file and read all the entries
    // into index tables, so that queries are able to effectively find
    // intersections/unions of those index requirements

    read_file();
}

//Reads the file into our table
bool mmytable::read_file(){
    if(!rec.set_file(__dir+__name))
        return false;

    __delimiter = rec.get_delimiter();

    field_add_all(rec.get_fieldnames());

    //put the file into __itable
    while(rec.more()){
        string temp = rec.next();
        parse(temp, __delimiter);
    }
    return true;
}
void mmytable::init(const string &table_name){
    __name = table_name;
}

void mmytable::field_add_all(const vector<string>& fieldnames){
    for(size_t i = 0; i < fieldnames.size(); i++){
        field_add(fieldnames[i]);
    }
}

void mmytable::field_add(const string &field_name){
    size_t fsize = __itables.size();
    __itables.insert(field_name);
    __fields[fsize] = field_name; //Associate the column to field. For file read/write
}

void mmytable::insert(vector<string>& fieldnames){
    rec.insert(fieldnames);
}

/**
 * @brief mmytable::parse   : Turn a given file-line into __itables data
 * @param fileline          : string to be parsed
 * @param length            : Length of string
 * @param delimiter         : Delimiter to seperate substrings
 */
void mmytable::parse(const string &fileline, const char delimiter){
    //Split by a delimiter
    //The first substring is the id-number. Take and turn into int for better comparisons later
    //eg. "123|foo|bar|    Stephen| Crane | all one field|"
    enum class lineflag {start, end};
    lineflag flag = lineflag::start;
    mmyint length = fileline.size();
    size_t i_start=0; //start/end of substring
    size_t count = 0;
    string subs;
    mmyint lineid; //id value prefixed at start of the string

    //read from file into tables
    cout << fileline << endl;
    for(mmyint i = 0; i < length; i++){
        if(fileline[i] == delimiter){
            subs = fileline.substr(i_start, i - i_start);
            //Put them into their fields
            if(count==0){
                lineid = stoul(subs);
            }else{
                __itables[__fields[count-1]][subs] += lineid;
            }
//            flag=lineflag::start; //change flag back
            i_start = i+1;
            count++;
            continue;
        }
    }
}

//Takes field constraints. Returns 2d table of relevant data.
multimap<mmyint, string>& mmytable::select(const vector<string>& fields,
                                           const string& constraints){
    multimap<mmyint, string> selection;

    return selection;
}
//Select * From ... where constraints
//Based on constraints. Returns all fields data.
multimap<mmyint, string>& mmytable::select(const string& constraints){
    multimap<mmyint, string> selection;
//    vector<mmyint> idnums;
    set<mmyint> idnums; //All initial idnums

//    //Apply constraints and filter out non-matches
//    for(size_t i = 0; i < __fields.size(); i++){
//        vector<mmyint> tempids;
//        //Get all matching ID#s into a vector
//        for(auto it = __itables[__fields[i]].begin();
//            it!= __itables[__fields[i]].end();
//            it++){

//            add_sorted(tempids, (*it).vec); //Make sure our set is sorted using my function

//        }
//        //Here do an intersection/union of idnums vs tempids
//        //Get the new idnums, which represents the inner
//    }

    return selection;
}

//Get a vector of all id#s that pass the comparison
vector<mmyint>& vector_filter(const string& fieldname,
                              const string& op,
                              const string& comp){

}
