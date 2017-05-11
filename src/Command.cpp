#include "Command.h"
#include <iostream>
#include <string>
#include <sstream>
using namespace std;
using namespace boost;


Command::Command()
{
    this->status = false;
}

Command::Command(string s)
{
    this->status = false;
    this->token = s;
}

bool Command::statusCheck(){
    return this->status;
}

string Command::tokenCheck(){
    return this->token;
}

int Command::signCheck(){
    return this->sign;
}

void Command::addsign(int s){
    this->sign = s;
}
void Command::addtoken(string t){
    this->token = t;
}
void Command::addstatus(bool st){
    this->status = st;
}
