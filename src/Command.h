#ifndef COMMAND_H
#define COMMAND_H
#include <iostream>
#include <string>
//#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
using namespace std;

class Command
{
    private:
        string token;
        bool status;
        int sign;
        bool test;
        int precsize;

    public:
        Command();
        Command(string s);
        bool statusCheck();
        string tokenCheck();
        int signCheck();
        void addstatus(bool st);
        void addtoken(string tk);
        void addsign(int s);
        bool istest();
        void addtest(bool tst);
        void addprec(int i);
        int getprec();
};


#endif
