COMMAND RSHELL
===

AUTHORS:
---
Chester Lau 

Kevin Chen

CONTENT:
---
This is our assignment 2 program which mimics the behavior of a command shell prompt. It has the ability to execute any commands that are found in /bin,/usr/bin. Commands such as mkdir,echo,ls etc will work in our program, however; any commands from bash such as cd will not work. Our program is designed to handle any number of commands and comments with connectors. Our program can handle single commands the same way as the bash, simply list "ls -a" or "echo blablabla". The program handles multiple commands by having connectors (; || &&) in between e.g. "echo hello; echo byebye". Just like the bash ";" states the command will run no matter what, "&&" states the command will execute if the previous command executed succesfully, "||" states the command will execute if the previous command failed to execute, and "#" determines a comment. Once finished using the bash, simply type "q" and the program will end.

ASSIGNMENT 3 ADDED FUNCTIONALITIES:
---
In this update to our Rshell we have added a Test command and Precedence operator feature. Our program can now handle tests commands, simply type "test" followed by a flag ("-e": checks if the file/directory exists, "-f": checks if the file/directory exists and is a regular file, "-d": checks if the file/directory exists and is a directory) and then the file/directory location. You can also instead of typing test; encapsulate the test command with "[" and "]". e.g.("[ -e test/file/patch ]"). Next, we added a precedence functionality with the use of "(" and ")" operators. e.g. the command "echo a && echo b || echo c && echo d" will print:

a

b

d

However now with this new feature, the command "(echo a && echo b) || (echo c && echo d)" will print:

a

b


BUGS: 
---
* Connectors need to follow the orthodox rules from bash.
* Commands built into bash will not execute. 
* Echo with any of the connectors "# ; || &&" will not execute properly due to the tokenizer.
* "q" must be entered by itself when trying to exit. (Am not sure if the program had to handle "q" in a line of commands. (5/10/17 11:54pm)
* Echo multiple words/sentences will sometimes result in the output: "Error: echo command not found"
* When using file redirection you must have a space before and after the "<",">", and ">>". E.G.("cat > input.txt"  and " cat >> input.txt") not following this format may cause an error. 

LANGUAGE: 
---
C++
