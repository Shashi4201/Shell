# Shell

// Shell Project 
>> Creating my own command shell using OS system calls to execute built in Linux Commands

>> The shell runs an infinite loop (which will only exit with the ‘exit’ command) and interactively process user commands.

>> For reading the complete input line, getline() function is used.
man page : http://manpages.ubuntu.com/manpages/bionic/man3/getline.3.html
 
>> For separating the multiple words (in case of multiple commands or command with multiple arguments) from the input line, strsep() function is used.
man page : http://manpages.ubuntu.com/manpages/bionic/man3/strsep.3.html

>> To execute the commands with a new process, fork, exec and wait system calls are used.

>> chdir() is used to implement the change directory command like cd <dir_path> and cd .. 
man page : http://manpages.ubuntu.com/manpages/bionic/man2/chdir.2.html

>> This shell supports multiple command execution for sequential execution as well as for parallel execution. ( && parallel and ## sequential )

>> Output Redirection : 
This shell redirects STDOUT for the commands using ‘>’ symbol. 
Example : ls > info.out

Some of the linux commands that are suppoerted in this shell are : 

1. ls, ls -l and ls -al
2. cd <dir_path> and cd ..
3. Empty command and Incorrect Command
4. Ctrl + C and Ctrl + Z should not exit the shell.
5. Parallel Commands -- ls && pwd 
6. Sequential Commands -- ls ## pwd
7. Output Redirection -- ls > info.out and cat command is also supported.
8. mkdir, rmdir, are also supported for creating directory and removing an empty directory. 
9. rm -r <dir_path> is also supported for deleting a non-empty directory.
10.Exit commands that exit the entire shell.
 
