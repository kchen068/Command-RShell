#ifndef COMMAND_H
#define COMMAND_H
#include <iostream>
#include <string>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
using namespace std;

class Command
{
    private:
        string token;
        bool status;
        int sign;

    public:
        Command();
        Command(string s);
        bool statusCheck();
        string tokenCheck();
        int signCheck();
        void addstatus(bool st);
        void addtoken(string tk);
        void addsign(int s);
};


#endif