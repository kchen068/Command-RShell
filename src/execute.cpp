#include "execute.h"
#include "Command.h"
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
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
int Execute::forking(const vector<char*>&convert)//this function takes care of executing commands and forking
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

void Execute::runtest(Command* test)
{
    if(test->tokenCheck().at(0) == '[' && test->tokenCheck().at(test->tokenCheck().size()-1) == ']') // we will remove the brackets 
    {
        string temp = test->tokenCheck();
        temp.erase(temp.begin(), temp.begin()+1);
        temp.erase(temp.end()-1, temp.end());
        test->addtoken(temp);
    }
    
    if(test->tokenCheck().substr(0,4) == "test")
    {
        string temp = test->tokenCheck();
        temp.erase(temp.begin(), temp.begin()+4);
        test->addtoken(temp);
    }
    
    if(test->tokenCheck().at(0) == ' ')  //removing any preceding whitespace
    { 
        string temp = test->tokenCheck();
        temp.erase(temp.begin(), temp.begin()+1);
        test->addtoken(temp);
     }
        
    if(test->tokenCheck().at(test->tokenCheck().size()-1) == ' ')  //removing any proceeding whitespace
    { 
        string temp = test->tokenCheck();
        temp.erase(temp.end()-1, temp.end());
        test->addtoken(temp);
    }
        
    //now we will be checking for the flags since we removed all unnecessary syntax
    struct stat buf;
    int check;         //tells us which flag it is 1 = -e, 2 = -f, 3 = -d
    
    if(test->tokenCheck().substr(0,2) == "-e") //check if the file/directory exists
    {
        check = 1;
    }
    
    else if(test->tokenCheck().substr(0,2) == "-f") //check if the file/directory exists and is a regular file
    {
        check = 2;
    }
    
    else if(test->tokenCheck().substr(0,2) == "-d") //check if the file/directory exists and is a directory
    {
        check = 3;
    }
    else
    {
        check = 1;
    }
    
    string temp = test->tokenCheck();         
    
    if(test->tokenCheck().substr(0,2) == "-e" || test->tokenCheck().substr(0,2) == "-f" || test->tokenCheck().substr(0,2) == "-d")  //remove the flag from the token/commmand
    {
        temp.erase(temp.begin(), temp.begin()+2);
        test->addtoken(temp);
    }
    
    if(test->tokenCheck().at(0) == ' ')  //removing any preceding whitespace
    { 
        string temp = test->tokenCheck();
        temp.erase(temp.begin(), temp.begin()+1);
        test->addtoken(temp);
     }
        
    if(test->tokenCheck().at(test->tokenCheck().size()-1) == ' ')  //removing any proceeding whitespace
    { 
        string temp = test->tokenCheck();
        temp.erase(temp.end()-1, temp.end());
        test->addtoken(temp);
    }
    
    string flag = test->tokenCheck();
    
    
    if(check == 1) 
    {
        int sign = stat(flag.c_str(), &buf);
        
        if(sign == 0)
        {
            test->addstatus(true);
        }
        else
        {
            test->addstatus(false);
        }
    }
    
    if(check == 2)
    {
        int sign = stat(flag.c_str(), &buf);
        
        if(sign == 0 && S_ISREG(buf.st_mode))
        {
            test->addstatus(true);
        }
        else
        {
            test->addstatus(false);
        }
    }
    
    if(check == 3)
    {
        int sign = stat(flag.c_str(), &buf);
        
        if(sign == 0 && S_ISDIR(buf.st_mode))
        {
            test->addstatus(true);
        }
        else
        {
            test->addstatus(false);
        }
    }
    
    string status;
    if(test->statusCheck())
    {
        status = "(true)";
    }
    else
    {
        status = "(false)";
    }
    
    cout << status << endl;
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
    // [] or test = test
    
    bool percon = false;
    int counter = 0;
    int idex;
    bool allTrue = false;
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
        if(tokens.at(index)->getprec() >= 2)
        {
            
            percon = true;
            allTrue = true;
            idex = index;
            counter = tokens.at(index)->getprec();
        }
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
                    allTrue = false;
                    
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
                                allTrue = false;
                            }
                    }
            }
            
        if(tokens.at(index)->signCheck() == 3)  // && runs only if first passes
            {
                if(index >= 2 && tokens.at(index-1)->signCheck() == 2)  // have to deal with precedence issues due to multiple parameters
                {
                    if(tokens.at(index-1)->statusCheck() == true || tokens.at(index-2)->statusCheck() == true)
                    {
                         check = forking(convert);
                        if(check == 0)
                        {
                            tokens.at(index)->addstatus(true);
                        
                        }
                        else
                        {
                            tokens.at(index)->addstatus(false);
                            allTrue = false;
                        }
                    }
                }
                
                else
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
                            allTrue = false;
                   
                        }
                    }
                }
            }
        
        if(tokens.at(index)->signCheck() == 4) // test or [], will run the test command via our functionality
        {
            //call test function
            Command* tst = tokens.at(index);
            runtest(tst);
        }
        counter --;
        
	//skips the next token or tokens depending on whether or not there are parenthesis
	
        if(allTrue == false && percon == true && counter == 0)
        {
            if(unsigned(idex + tokens.at(idex)->getprec()) != tokens.size()-1)
            {
                x+= tokens.at(idex)->getprec();
                index += tokens.at(idex)->getprec();
            }
        }
        
        this->index++;      // keeps track of which command is being executed
        
    }
    
    
}

