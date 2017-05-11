#include "execute.h"
#include "Command.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;


Execute::Execute(){
    this->index = 0;
}

void Execute::parsecmd(string text, vector<string>&parse) //this function converts the command into individual words
{
    parse.clear(); // clears the vector of any previous instructions
    boost::split(parse, text, boost::is_any_of(" "));
}

char* Execute::to_char(const string &token)// this function converts the individual words from parsecmd into char** so we can use execvp
{
	char* temp = new char[token.size() + 1];
	strcpy(temp, token.c_str());
	return temp;
}
int Execute::forking(const vector<char*>&convert)//this function takes car of executing commands and forking
{
    vector<char*>temp = convert;
    int termstat;
    char** arg = &temp.at(0);
    pid_t pid = fork();
     
    if(pid < 0)
    {
        perror("$ Error: fork failed");
        _exit(-127);
    }
        
    if(pid == 0) //child process
    {
        int check = execvp(arg[0],arg);
        if(check < 0)
        {
            cout << "$ Error: " << arg[0] <<" command not found" << endl;
            _exit(-127);
        }
           
        _exit(0);
    }
           
    waitpid(pid, &termstat, 0);
    return WEXITSTATUS(termstat);
    
    
}
void Execute::terminal(const vector<Command*>& tokens) //this function will run the commands 
{
    int check;
    vector<char*>convert;
    vector<string> parse;
    string text;
    // ; = next command is always executed
    // && = next command only executes if previous command succesfully executed
    // || = next command only executes if previous command didnt
    // # = comment 
    
    for(unsigned x = 0; x < tokens.size(); x++)
    {
        
        convert.resize(0);
        text = tokens.at(x)->tokenCheck();
        parsecmd(text,parse);
    
        for(unsigned i = 0; i < parse.size(); i++)
        {
            convert.push_back(to_char(parse[i]));
        }
        
        //check = forking(convert);
        
        if(tokens.at(index)->signCheck() == 0)  //comment - delete this token
            {
                tokens.at(index)->addstatus(true);
            }
            
        if(tokens.at(index)->signCheck() == 1)  // ; always runs
            {   
                check = forking(convert);
                if(check == 0)
                {
                    tokens.at(index)->addstatus(true);
                }
                
                else
                {
                    tokens.at(index)->addstatus(false);
                }
               
            }
            
        if(tokens.at(index)->signCheck() == 2)  // || runs only if first fails
            {
                
                if(tokens.at(index-1)->statusCheck() == false)
                    {
                    
                        check = forking(convert);
                        if(check == 0)
                            {
                                tokens.at(index)->addstatus(true);
                            }
                         else
                            {
                                tokens.at(index)->addstatus(false);
                            }
                    }
            }
            
        if(tokens.at(index)->signCheck() == 3)  // && runs only if first passes
            {
                if(tokens.at(index-1)->statusCheck() == true)
                {
                    check = forking(convert);
                    if(check == 0)
                    {
                        tokens.at(index)->addstatus(true);
                        
                    }
                    else
                    {
                        tokens.at(index)->addstatus(false);
                   
                    }
                }
            }
        
        
        this->index++;      // keeps track of which command is being executed
        
    }
    
    
}

