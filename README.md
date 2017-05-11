COMMAND RSHELL
===

AUTHORS:
---
Chester Lau 

Kevin Chen

CONTENT:
---
This is our assignment 2 program which mimics the behavior of a command shell prompt. It has the ability to execute any commands that are found in /bin,/usr/bin. Commands such as mkdir,echo,ls etc will work in our program, however; any commands from bash such as cd will not work. Our program is designed to handle any number of commands and comments with connectors. Our program can handle single commands the same way as the bash, simply list "ls -a" or "echo blablabla". The program handles multiple commands by having connectors (; || &&) in between e.g. "echo hello; echo byebye". Just like the bash ";" states the command will run no matter what, "&&" states the command will execute if the previous command executed succesfully, "||" states the command will execute if the previous command failed to execute, and "#" determines a comment. Once finished using the bash, simply type "q" and the program will end.

BUGS: 
---
* Connectors need to follow the orthodox rules from bash.
* Commands built into bash will not execute. 
* Echo with any of the connectors "# ; || &&" will not execute properly due to the tokenizer.
* "q" must be entered by itself when trying to exit. (Am not sure if the program had to handle "q" in a line of commands. (5/10/17 11:54pm)

LANGUAGE: 
---
C++
