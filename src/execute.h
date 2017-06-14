#ifndef EXECUTE_H
#define EXECUTE_H
#include "Command.h"

using namespace std;

class Execute{
    private:
        int index;//keeps track of which argument is being executed
    
    public:
        Execute();
        int forking(const vector<char*>&temp);
        void terminal(const vector<Command*>& tokens);
        void parsecmd(string convert, vector<string>&parse);
        char* to_char(const string &token);
        void runtest(Command* test);
        
};

#endif