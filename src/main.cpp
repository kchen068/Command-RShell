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
         
         
        
        
         
    for(unsigned i = 0; i< text.size()-1; i++) //this function will determine the sign(|| $$ && # | ) 
    {
        //int vector where 0 = #, 1 = ;, 2 = ||, 3 = &&, 4 = test or [], 5 = | pipe
        
        if(text.substr(i,2) == "||")        
        {                                   
            tokens[numb]->addsign(2);       
            numb ++;
        }                                   
       if(text.at(i) == ';')              
        {                                   
            tokens[numb]->addsign(1);       
            numb ++;                        
        }                                    
       if(text.substr(i,2) == "&&")        
        {
            tokens[numb]->addsign(3);        
            numb ++;
        }
       if(text.at(i) == '#')
        {
            tokens[numb]->addsign(0);       
            numb ++;
        }
        if(text.substr(i,3) == " | ")
        {
            
            tokens[numb]->addsign(5);
            tokens[numb]->addpipe(1);
           
            numb ++;
        }
        
    
        
      
       
    bool prec = false;                                  //---------for int redirect
    int in;                                             
    int sie = 1;                                        // second parameter of substr() is length of substring             
    for(unsigned i = 0; i<tokens.size(); i++)           //（echo a && echo b || echo c)
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
        if(prec == true)
        {
            sie++;
        }
        if(tokens[i]->tokenCheck().at(0) == '(')
        {
            string temp = tokens[i]->tokenCheck();
            temp.erase(temp.begin(), temp.begin()+1);
            tokens[i]->addtoken(temp);
            prec = true;
            in = i;
        }
        if(tokens[i]->tokenCheck().at(tokens[i]->tokenCheck().size()-1) == ')')
        {
            tokens[in]->addprec(sie);
            
            sie = 1;
            prec = false;
            string temp = tokens[i]->tokenCheck();
            temp.erase(temp.end()-1, temp.end());
            tokens[i]->addtoken(temp);
        }
    }  
    }
    for(unsigned i = 0; i<tokens.size()-1; i++)
    {
            
        if(tokens.at(i+1)->signCheck() == 5 && tokens.at(i)->signCheck() != 5 )
        {
            tokens.at(i)->addpipe(2);
        }
    }
    
    for(unsigned i = 0; i<tokens.size()-1; i++)
    {
        if((i + 1) == tokens.size()-1)
        {
            tokens.at(i+1)->addpipe(3);
        }
        else
        {
            if(tokens.at(i)->getpipe() == 1 && tokens.at(i+1)->getpipe() ==0)
            {
                tokens.at(i)->addpipe(3);
            }
        }
    }
    
     
    
    for(unsigned i = 0; i<tokens.size(); i++)  //we remove any whitespace in the token and will determine whether it is a test or not
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
        if((tokens[i]->tokenCheck().at(0) == '[' && tokens[i]->tokenCheck().at(tokens[i]->tokenCheck().size()-1) == ']') || tokens[i]->tokenCheck().substr(0,4) == "test") //this looks for test commands and will add the sign 4 
        {
            tokens[i]->addsign(4);                                                                   
            tokens[i]->addtest(true);
        }
        
        for(unsigned j = 0; j < tokens[i]->tokenCheck().size(); j++)   // 1 = <, 2 = >>, 3 = >
        {
            if(tokens[i]->tokenCheck().substr(j,3) == " > ")
            {
                tokens[i]->addredirect(3);
             
            }
            if(tokens[i]->tokenCheck().substr(j,2) == ">>")
            {
                tokens[i]->addredirect(2);
               
            }
            if(tokens[i]->tokenCheck().substr(j,3) == " < ")
            {
                tokens[i]->addredirect(1);
               
            }
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
       
        if(text == "q")return 0;
        
        parse(text,com);                //call token function to parse text and also to remove any comments
        
        Execute* ex = new Execute();    
        ex->terminal(com);
        delete ex;
    }
    return 0;
}