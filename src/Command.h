#ifndef COMMAND_H
#define COMMAND_H
#include <iostream>
#include <string>
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
        int redirect;
        int pipe;

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
        void addredirect(int a);
        int getredirect();
        void addpipe(int a);
        int getpipe();
};


#endif