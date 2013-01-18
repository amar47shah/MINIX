iShell, by Amar Shah
--------------------

iShell is a simple shell environment for the MINIX operating system.

Input
-----

Line input is entered at the "iShell>" prompt.
iShell accepts commands in the following syntax:

command [arg1 ... argn] [< filename] [> filename] 
  or
command [arg1 ... argn] [< filename] | command [arg1 ... argn] [> filename]

It is not necessary to surround '<', '>', and '|' tokens with spaces.
That is, the following commands are equivalent:

        cat in.txt|wc>out.txt
        cat in.txt | wc > out.txt 

Unrecognized commands are answered with an error message.
        
Built-in Commands
-----------------

iShell implements a number of native commands.

       * history  - lists the last 10 commands the user has entered.
       * cd       - changes the current working directory
                        and updates the environment variable PWD.
       * env      - lists the environment variables.
       * setenv   - changes an environment variable value 
                        or creates a new environment variable.
                        syntax: setenv variable value
       * unsetenv - removes a variable from the environment.
                        syntax: unsetenv variable
       * exit     - exits the shell.

iShell handles other MINIX commands by forking another process.

       If a command is not recognized as one of the built-in commands
       above, iShell looks in the directories of the PATH variable for
       the command.  If it is found, it is executed in another process.
       Else, an error message is returned.

I/O Redirection
---------------

iShell handles I/O redirection of stdin and stdout.  

The following commands are legal in iShell:

iShell> ls > tmp.txt
     /* Redirects stdout to the file tmp.txt. */ 
iShell> wc < tmp.txt
     /* Redirects stdin from the file tmp.txt. */
iShell> wc < in.txt > out.txt
     /* Redirects stdin from in.txt, stdout to out.txt. */  

If 'filename' does not already refer to a file, the file is created.
Output redirection is legal with the built-in commands 'history' and 'env'.

Pipes
-----

A single pipe is allowed between two commands. Examples:

iShell> ls -l | grep txt
     /* Redirect output of "ls -l" to input of "grep txt" */
iShell> cat < words.txt | wc > wordcount.txt
     /* Redirect input of "cat" to words.txt,
        redirect output of "cat" to input of "wc",
        and redirect output of wc to wordcount.txt */  
   

