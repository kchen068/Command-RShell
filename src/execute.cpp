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
#include <fcntl.h>

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
int Execute::append(string second, vector<char*> firstconvert)
{           
           
   
    int out = open(to_char(second), O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    int fd = dup(1);
    dup2(out, 1);
    close(out);
        
    char** arg = &firstconvert.at(0);
    //int check = execvp(arg[0],arg);
    int check = forking(firstconvert);
    if(check < 0)
    {
        perror("$ Error: >> commamd did not succesfully run");
        //_exit(-127);
    }
    
    if(dup2(fd,1) == -1)
    {
        perror("$ Error: dup2 ");
        //_exit(-127);
    }
    return check;
}

int Execute::truncate(string second, vector<char*> firstconvert)
{
    //fflush(stdout); 
    int sd = dup(1);
    int fd = open(to_char(second), O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    dup2(fd, 1);
    close(fd);
        
    char** arg = &firstconvert.at(0);
    //int check = execvp(arg[0],arg);
    int check = forking(firstconvert);    
    if(check < 0)
    {
        cout << "$ Error: > command did not succesfully run" << endl;
        _exit(-127);
    }
    
    if(dup2(sd,1) == -1)
    {
        perror("$ Error: dup2 ");
        //_exit(-127);
    }
    //fflush(stdout);
    return check;
}

int Execute::redirection(Command* r)
{
    int check;
    vector<string> firsthalf;                                        // stores string preceding the < > <<
    vector<string> secondhalf;                                   //stores string following the < > <<
    vector<char*> firstconvert;
    vector<char*> secondconvert;
    string temp = r->tokenCheck();
    int x = 0;                                                      // stores size of the implicit parameter ends
    for(unsigned i = 0; i < temp.size(); i++)                   // we will remove the < > and << to just have the names alone
    {
        if(temp.substr(i,4) == " >> ")
        {
            x = i;
            temp.erase(temp.begin()+i, temp.begin()+i+4);
        }
        if(temp.substr(i,3) == " < " || temp.substr(i,3) == " > ")
        {
            x = i;
            temp.erase(temp.begin()+i,temp.begin()+i+3);
            
        }
    }
    // cout << temp << x << endl;
    
    r->addtoken(temp);   //not sure if necessary to adjust the actual token or to just use temp
    string first = temp.substr(0,x);
    string second = temp.substr(x);
     
    //now i can call the parsecmd and to_char function so I can use execvp
    parsecmd(first, firsthalf);
    parsecmd(second, secondhalf);
    
    for(unsigned i = 0; i < firsthalf.size(); i++)
    {
        firstconvert.push_back(to_char(firsthalf[i]));
    }
    
    for(unsigned i = 0; i < secondhalf.size(); i++)
    {
        secondconvert.push_back(to_char(secondhalf[i]));
    }
    
    int in, out;
    if(r->getredirect() == 1)  // 1 = <, 2 = >>, 3 = >
    {
        int fd = dup(0);
        in = open(to_char(second), O_RDONLY);  //second hold filename in this case 
        dup2(in,0);  //replace standard input with the file
        close(in);
        
        char** arg = &firstconvert.at(0);
        //check = execvp(arg[0],arg);
        check = forking(firstconvert);
        if(check < 0)
        {
            perror("$ Error: < command did not succesfully run" );
           // _exit(-127);
        }
        
        if(dup2(fd, 0) == -1)
        {
            perror("$ Error: dup2");
        }
        
       
        
    }
    if(r->getredirect() == 2)
    {
        
        check = append(second, firstconvert);
       
    }
    
   if(r->getredirect() == 3)
    {
       check = truncate(second, firstconvert);
       // _exit(0);
        
    }
    
    return check;
}

int Execute::piping(vector<Command*>& pips)
{
    //cout << "made it to piping" << endl;
    vector<string> one;
    vector<string> two;
    vector<char*> oneone;
    vector<char*> twotwo;
    
    string ones = pips.at(0)->tokenCheck();
    string twos = pips.at(1)->tokenCheck();
    
    //cout << ones << "|||||" << twos << endl;
    parsecmd(ones, one);
    parsecmd(twos, two);
    
    for(unsigned i = 0; i < one.size(); i++)
    {
       if(one.at(i) != "<" && one.at(i) != ">" && one.at(i) != ">>" )
        {
            oneone.push_back(to_char(one.at(i)));
        }
    }
    oneone.push_back(NULL);
    
    for(unsigned i = 0; i < two.size(); i++)
    {
        twotwo.push_back(to_char(two[i]));
    }
    twotwo.push_back(NULL);
    
    char** arg0 = &oneone.at(0);
    char** arg1 = &twotwo.at(0);
   
   
    const int PIPE_READ = 0;
    const int PIPE_WRITE = 1;

    int check1;
    int check2;
    int fds[2];
    int r;
    int sav = dup(1);
    int xx = -1;
    
    
    r = pipe(fds);
    if(r == -1)
    {
        perror("pipe");
        return -1;
    }
    dup2(fds[PIPE_WRITE], 1);
    check1 = forking(oneone);
    close(fds[PIPE_WRITE]);
    
    int savs = dup(0);
    if(dup2(fds[PIPE_READ], STDIN_FILENO) == -1)
    {
        perror("dup2");
    }
    dup2(sav, STDOUT_FILENO);
    check2 = forking(twotwo);
    if(check2 == 0)
    {
        xx = 0;
    }
    close(fds[PIPE_READ]);
    dup2(savs, STDIN_FILENO);
    close(savs);
    close(sav);
    //close(fds[PIPE_WRITE]);   //added
    //close(fds[PIPE_READ]);
    return xx;
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
    // > = overwrites a file or creates a file if it doesn't exist
    // < = input redirection
    // >> = appends to a file or creates file if it doesn't exist
    // | = piping  
    
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
                
                if(tokens.at(index)->getpipe() == 2)
                {
                    vector<Command*> p;
                    // p.push_back(tokens.at(index));
                    // p.push_back(tokens.at(index+1));
                    // piping(p);
                    int y = index;
                    while(tokens.at(y)->getpipe() != 3)
                    {
                        //cout << tokens.at(y)->tokenCheck() << endl;
                        p.push_back(tokens.at(y));
                        y++;
                    }
                    //cout << tokens.at(y)->tokenCheck() << endl;
                    p.push_back(tokens.at(y));
                    // for(auto i : tokens)
                    // {
                    //     cout << i->tokenCheck() << " " << endl;
                    // }
                    if(piping(p) != -1)
                    {
                        tokens.at(index)->addstatus(true);
                    }
                    
                    index += p.size()-1;
                }
                
                else
                {
                
                    if(tokens.at(index)->getredirect() == 1 || tokens.at(index)->getredirect() == 2 || tokens.at(index)->getredirect() == 3 )
                    {
                        check = redirection(tokens.at(index));
                    }
        
                    else
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
            
        if(tokens.at(index)->signCheck() == 2)  // || runs only if first fails
            {
                
                
                if(tokens.at(index-1)->statusCheck() == false)
                    {
                         if(tokens.at(index)->getpipe() == 2)
                            {
                                vector<Command*> p;
                                // p.push_back(tokens.at(index));
                                // p.push_back(tokens.at(index+1));
                                // piping(p);
                                int y = index;
                                while(tokens.at(y)->getpipe() != 3)
                                {
                                    //cout << tokens.at(y)->tokenCheck() << endl;
                                    p.push_back(tokens.at(y));
                                    y++;
                                }
                               // cout << tokens.at(y)->tokenCheck() << endl;
                        p.push_back(tokens.at(y));
                                //piping(p);
                                //index += p.size()-1;
                                if(piping(p) != -1)
                                {
                                    tokens.at(index)->addstatus(true);
                                }
                                index += p.size()-1;
                              
                        
                        if(tokens.at(index)->getredirect() == 1 || tokens.at(index)->getredirect() == 2 || tokens.at(index)->getredirect() == 3 )
                            {
                                check = redirection(tokens.at(index));
                            }

                        else
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
            
        if(tokens.at(index)->signCheck() == 3)  // && runs only if first passes
            {
                if(index >= 2 && tokens.at(index-1)->signCheck() == 2)  // have to deal with precedence issues due to multiple parameters
                {
                    if(tokens.at(index-1)->statusCheck() == true || tokens.at(index-2)->statusCheck() == true)
                    {
                        if(tokens.at(index)->getpipe() == 2)
                        {
                                vector<Command*> p;
                            //     p.push_back(tokens.at(index));
                            //     p.push_back(tokens.at(index+1));
                            //     piping(p);
                            // }
                            int y = index;
                            while(tokens.at(y)->getpipe() != 3)
                            {
                               // cout << tokens.at(y)->tokenCheck() << endl;
                                p.push_back(tokens.at(y));
                                y++;
                            }
                            //cout << tokens.at(y)->tokenCheck() << endl;
                        p.push_back(tokens.at(y));
                            //piping(p);
                            //index += p.size()-1;
                            if(piping(p) != -1)
                            {
                                tokens.at(index)->addstatus(true);
                            }
                            index += p.size()-1;
                        }
                        
                        if(tokens.at(index)->getredirect() == 1 || tokens.at(index)->getredirect() == 2 || tokens.at(index)->getredirect() == 3 )
                            {
                                check = redirection(tokens.at(index));
                            }
                        
                        else
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
                
                else
                {
                    if(tokens.at(index-1)->statusCheck() == true)
                    {
                        if(tokens.at(index)->getpipe() == 2)
                            {
                                vector<Command*> p;
                                int y = index;
                                while(tokens.at(y)->getpipe() != 3)
                                {
                                    //cout << tokens.at(y)->tokenCheck() << endl;
                                    p.push_back(tokens.at(y));
                                    y++;
                                }
                                //cout << tokens.at(y)->tokenCheck() << endl;
                                p.push_back(tokens.at(y));
                               // piping(p);
                                if(piping(p) != -1)
                                {
                                    tokens.at(index)->addstatus(true);
                                }
                                index += p.size()-1;
                            }
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
        if(tokens.at(index)->signCheck() == 5)
        {
            vector<Command*> p;
            // p.push_back(tokens.at(index));
            // p.push_back(tokens.at(index+1));
            // piping(p);
            int y = index;
            while(tokens.at(y)->getpipe() != 3)
            {
                //cout << tokens.at(y)->tokenCheck() << endl;
                p.push_back(tokens.at(y));
                y++;
            }
            //cout << tokens.at(y)->tokenCheck() << endl;
            p.push_back(tokens.at(y));
            //piping(p);
            //index += p.size()-1;
            if(piping(p) != -1)
            {
                tokens.at(index)->addstatus(true);
            }
            index += p.size()-1;
            
        }
        counter --;
    
        if(allTrue == false && percon == true && counter == 0)
        {
            if(idex + tokens.at(idex)->getprec() != tokens.size()-1)
            {
                x+= tokens.at(idex)->getprec();
                index += tokens.at(idex)->getprec();
            }
        }
        
        this->index++;      // keeps track of which command is being executed
        
    }
    
    
    }
    
}

