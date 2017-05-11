#include "Command.h"
#include "execute.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
using namespace std;
using namespace boost;

void parse(string text,vector<Command*>& tokens)
{
    
    char_separator<char> sep("|;&#");
    tokenizer< char_separator<char> > tok(text, sep);
      
     BOOST_FOREACH (const string& t, tok) {
        Command* a = new Command();
        a->addtoken(t);
        tokens.push_back(a);
             
     }
         
         tokens.at(0)->addsign(1);          //because the first token has no sign
         int numb = 1;                      //keep track of vector
         
    for(unsigned i = 0; i< text.size()-1; i++)
    {
        if(text.substr(i,2) == "||")        //int vector where 0 = #
        {                                   // 1 = ;
            tokens[numb]->addsign(2);       // 2 = ||
            numb ++;
        }                                   // 3 = &&
        if(text.at(i) == ';')
        {
            tokens[numb]->addsign(1);        
            numb ++;
        }
        if(text.substr(i,2) == "&&")        // second parameter of substr() is length of substring
        {
            tokens[numb]->addsign(3);        
            numb ++;
        }
        if(text.at(i) == '#')
        {
            tokens[numb]->addsign(0);       
            numb ++;
        }
    }
    for(unsigned i = 0; i<tokens.size(); i++)
    {
        if(tokens[i]->tokenCheck().at(0) == ' ')
        { 
            string temp = tokens[i]->tokenCheck();
            temp.erase(temp.begin(), temp.begin()+1);
            tokens[i]->addtoken(temp);
        }
        if(tokens[i]->tokenCheck().at(tokens[i]->tokenCheck().size()-1) == ' ')
        { 
            string temp = tokens[i]->tokenCheck();
            temp.erase(temp.end()-1, temp.end());
            tokens[i]->addtoken(temp);
        }
    }
           
}


int main() {
    
    //command prompt
    //user enters commands-cin
    //q-quit command prompt
    
    //tokenizer class to break up the getline
    // use execvp, ... to run the commands fork waitpid
    //vector <string> tokens;
    //vector <int> sign;
    vector<Command*> com;
    string text;
    bool quit = false;
    
    while(quit == false){
        com.clear();
        cout << "$ type command(s) or q to quit" << endl;
        cout << "$ ";
        getline(cin, text);
       //cout << endl;
        if(text == "q")return 0;
        
        parse(text,com);                //call token function to parse text and also to remove any comments
        
        Execute* ex = new Execute();    
        ex->terminal(com);
        delete ex;
    }
    return 0;
}