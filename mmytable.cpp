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

    rec.set_file(filelocation);
    __delimiter = rec.get_delimiter();

    field_add_all(rec.get_fieldnames());

    //put the file into __itable
    while(rec.more()){
        string temp = rec.next();
        parse(temp, __delimiter);
    }
}

mmytable::mmytable(const string& table_name, const string &dir)
    : __name(table_name), __dir(dir)
{
    //Based on the table_name, open that file and read all the entries
    // into index tables, so that queries are able to effectively find
    // intersections/unions of those index requirements

    rec.set_file(dir+table_name);
    __delimiter = rec.get_delimiter();
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

void mmytable::insert(const vector<string>& fieldnames){
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

//    while(i_start < length){
//        i_end = fileline.find(delimiter, i_start);
//        if(i_end > length) //meaning it's long_int_max
//            i_end = length;
//        if(count==0){
//            //Take that substring as an int
//            subs = fileline.substr(i_start, i_end-i_start);
//        }
//        count++;
//    }
//    for(size_t i = 0; i < length; i++){
//        //This way I can easily trim whitespace without taking any extra steps
//        if(flag==lineflag::start){
//            //on second thought, don't trim whitespace, leave as is
//            if(/*isspace(fileline[i]) || */fileline[i]==delimiter){
//                i_start++;
//                continue;
//            }else{
//                flag=lineflag::end; //change flag
//                i_end++;
//            }
//        }else{
//            if(fileline[i] == delimiter){
//                subs = fileline.substr(i_start, i_end-i_start);
//                //Put them into their fields
//                if(count==0){
//                    lineid = stoi(subs);
//                }else{
//                    __itables[__fields[count-1]][subs] += lineid;
//                }
//                flag=lineflag::start; //change flag back
//                i_start = i+1;
//                count++;
//                continue;
//            }
//            if(isspace(fileline[i]))
//                continue;
//            i_end = i+1;
//        }
//    }
}

//LET'S NOT USE VARIADIC HERE, SINCE CANNOT PASS OBJECTS EFFectivley
//void mmytable::create(const string& table_name, const string &strs, ...){
//    __name = table_name;
//    __itables.insert(table_name); //Create the table
//    if(strs != ""){
//        if(debug>=bugflag::medium)
//            cout << "[D] mmytable create: adding field: " << strs << endl;
//        add_field(strs);
////        add_field(...);
//    }
//}
//void mmytable::add_field(const string &strs, ...){
//    bool inserted = __itables.insert(strs);
//    if(debug>=bugflag::heavy && !inserted) cout << "[DEBUG] " << __FUNCTION__ << ": failed insert\n";
//}
