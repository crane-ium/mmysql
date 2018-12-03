#include "mmyshunting.h"

debugger DBG = debugger::heavy;

mmynode* parse_to_tree(const string& s){
    /** @todo Use parse tree algorithm instead, MUCH faster **/
    mmynode* root = nullptr;
    mmynode* child = nullptr;
    mmynode* right = nullptr;
    mmynode* notchild = nullptr;
    //boolean operator indeces, and comparison operator indeces
    set<size_t> bool_indeces, op_indeces; //places where operators were found (starting index)
    size_t length = s.length();

    if(DBG >= debugger::light)
        cout << "[MMYSHUNTING] Starting parse_to_tree: "
             << s << endl;

    enum class state{empty, getbbool, getnextblock};

    state mystate = state::getnextblock;
    int spare_parentheses = 0;
    size_t p_left = 0; //left parentheses of that block
    size_t b_left = 0; //left block index
    for(size_t i = 0; i < length; i++){
        //traverse the string. Build a tree to represent the order of
        //operations of the boolean comparisons and unions/intersections
        if(spare_parentheses==0 && s[i] == '('){
            spare_parentheses++;
            p_left = i+1;
            continue; //skip the rest, we are only trying to find this block string
        }else if(spare_parentheses > 0){
            if(s[i] == '(')
                spare_parentheses++;
            else if(s[i] == ')')
                spare_parentheses--;
            b_left = i+1;
            mystate = state::getbbool; //after parentheses, expect bool
            if(spare_parentheses==0)
                if(!root){//if nullptr
                    if(DBG>=debugger::medium)
                        cout << "[MMYSHUNTING] P BLOCK: "
                             << s.substr(p_left, i-p_left) << endl;
                    child = parse_to_tree(s.substr(p_left, i-p_left));
                }else{
                    root->right = parse_to_tree(s.substr(p_left, i-p_left));
                    if(DBG>=debugger::medium)
                        cout << "[MMYSHUNTING] P BLOCK right root: "
                             << root->right->val << endl;
                }

            continue;
        }
        if(mystate == state::getnextblock){
            //Process the comparison operators
            string temp="";
            size_t index = 0;
            if((temp=parsetree::in_set(s, COMPARITORS, i))!=""){
                index = i;
                i += temp.size();
                string left = mmytrim(s, b_left, index);
                size_t i_left = i;
                bool hasword = false; //Move i up until the next word begins
                bool inquote = false;
                for(i=i; i < length;i++){
                    if(!hasword && isspace(s[i])){
                        i_left++;
                        continue;
                    }
                    hasword = true;
                    if(s[i] == '\"'){
                        inquote ^= true;
                        continue;
                    }
                    if(!inquote && parsetree::in_set(s[i], PREOPERATORS)){
                        break;
                    }
                }
                string right = mmytrim(s, i_left, i);
                mmynode* tempnode = new mmynode(temp);
                tempnode->ttype = tokentype::comparitor;
                tempnode->left = new mmynode(left);
                tempnode->right = new mmynode(right);
                tempnode->left->ttype = tokentype::fieldname;
                tempnode->right->ttype = tokentype::comparee;
                mystate = state::getbbool;
                if(root && !root->right)
                    root->right = tempnode;
                else
                    child = tempnode;
                if(DBG>=debugger::light)
                    cout << "[MMYSHUNTING] COMP BLOCK: " << left << ", "
                         << temp << ", " << right << endl;

//                continue;
            }
        }

        //Process the boolean operators
        string temp="";
        size_t index;
        if(parsetree::in_set(s[i],PREOPERATORS) &&
                (temp=parsetree::in_set(s, BBOOLEANS, i+1))!="" &&
                (i+temp.size() < length) &&
                parsetree::in_set(s[i+temp.size()+1], POSTOPERATORS)){
            index = i+1;
            i += temp.size();
            if(!root){//nullptr
                root = new mmynode(temp, child);
                child = nullptr;
            }else{
                mmynode* tempnode = new mmynode(temp, root);
                root = tempnode;
            }
            b_left = i+1;
            mystate = state::getnextblock;
            if(DBG>=debugger::light)
                cout << "[MMYSHUNTING] BOOL BLOCK: " << (root->left ? root->left->val:"None")
                     << ", " << root->val << ", "
                     << (root->right ? root->right->val:"None") << endl;
        }
    }
    if(!root){
        if(child)
            root = child;
        else {
            cout << "Failed myshunting return\n";
            assert(true);
        }
    }

    return root;
}

string mmytrim(const string& s, size_t left, size_t right){
    while(isspace(s[left]))
        left++;
    while(isspace(s[right-1]))
        right--;
    //Then let's remove any parentheses surrounding it all
//    size_t p_left = 0; //left parentheses count
//    size_t p_right = 0;
//    if(s[left] != '(' && s[right-1] != ')')
    return s.substr(left, right-left);
}
