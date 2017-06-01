#include "Command.h"
#include <iostream>
#include <string>
#include <sstream>
using namespace std;
using namespace boost;


Command::Command()
{
    this->status = false;
    this->test = false;
}

Command::Command(string s)
{
    this->status = false;
    this->token = s;
    this->test = false;
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
bool Command::istest(){
    return this->test;
}
void Command::addtest(bool tst){
    this->test = tst;
}
void Command::addprec(int i){
    this->precsize = i;
}
int Command::getprec(){
    return this->precsize;
}