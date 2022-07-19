#include <stdio.h>
#include <string.h>                     // for str fucnctions like strsep() and strlen()
#include <stdlib.h>                     // exit()
#include <unistd.h>                     // fork(), getpid(), exec()
#include <sys/wait.h>                   // wait()
#include <signal.h>                     // signal()
#include <fcntl.h>                     // close(), open()

#ifndef MAX_BUF
#define MAX_BUF 1024
#endif

#ifndef MAXSIZE
#define MAXSIZE 100
#endif

// To parse Parallel Commands
int parseParallel(char *str,char **argv)
{
    int i;
    for(i=0;i< MAXSIZE;i++)
    {
        argv[i] = strsep(&str,"&&");
        if(argv[i] == NULL)
        {
            break;
        }
    }
    if(argv[1] == NULL)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

// To parse Sequential Commands
int parseSequential(char *str,char **argv)
{
    int i;
    for(i=0;i<MAXSIZE;i++)
    {
        argv[i] = strsep(&str,"##");
        if(argv[i] == NULL)
        {
            break;
        }
    }
    if(argv[1]==NULL)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

// To parse Command Redirection
int parseCommandRedirection(char *str,char **argv)
{
    int i;
    for(i=0;i<MAXSIZE;i++)
    {
        argv[i] = strsep(&str,">");
        if(argv[i] == NULL)
        {
            break;
        }
    }
    if(argv[1]==NULL)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

// To parse Space
void parseSpace(char *str,char **argv)
{
    int i;
    for(i=0;i<MAXSIZE;i++)
    {
        argv[i] = strsep(&str," ");
        if(argv[i]==NULL)
        {
            break;
        }
        if(strlen(argv[i])==0)
        {
            i--;
        }
    }
}

int parseInput(char *str,char **argv)
{
    /*This function will parse the input string into multiple commands
    or a single command with arguments depending on the delimiter (&&, ##, >, or spaces).*/
    char exit[] = "exit";
    // checking if it is exit
    if(strcmp(str,exit)==0)
    {
        return 0;
    }
    else if(parseParallel(str,argv)) // for parallel commands &&
    {
        return 1;
    }
    else if(parseSequential(str,argv)) // for sequential commands ##
    {
        return 2;
    }
    else if(parseCommandRedirection(str,argv)) // for redirection >
    {
        return 3;
    }
    else // normal execution
    {
        parseSpace(str,argv);
        return 4;
    }
}

void executeCommand(char **argv)
{
    if(argv[0]==NULL)
    {
        return;
    }
    // This function will fork a new process to execute a command
    pid_t rc = fork();
    if(rc<0)
    {
        printf("Process Creation Failed!\n");
        exit(0);
    }
    else if(rc==0)
    {
        signal(SIGINT,SIG_DFL);
        signal(SIGTSTP,SIG_DFL);
        if(strcmp(argv[0],"cd")!=0)
        {
            if(execvp(argv[0],argv)==-1)
            {
                printf("Shell: Incorrect command\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    else
    {
	    int status;
        /*  The status of any child processes specified by pid that are stopped, 
            and whose status has not yet been reported since they stopped, 
            shall also be reported to the requesting process. Eg : Ctrl + Z
        */
	    waitpid(rc,&status,WUNTRACED);
    }
}

void executeParallelCommands(char **argv)
{
    // This function will run multiple commands in parallel
    pid_t rc;
    size_t k;
    int i,j=0,l;
    int status;
    char *commandlist[MAXSIZE];
    pid_t pid[MAXSIZE];
    for(i=0;argv[i]!=NULL;i++)
    {
        if(strlen(argv[i])!=k)
        {
            parseSpace(argv[i],commandlist);
            if(commandlist[0]!=NULL&&strcmp(commandlist[0],"cd")==0)
            {
                if(chdir(commandlist[1])==-1)
                {
                    perror("Error");
                }
            }
            else if(commandlist[0]!=NULL)
            {
                rc = fork();
                if(rc<0)
                {
                    printf("Process Creation Failed\n");
                    exit(0);
                }
                else if(rc == 0)
                {
                    pid[j++] = getpid();
                    signal(SIGINT,SIG_DFL);
                    signal(SIGTSTP,SIG_DFL);
                    if(strcmp(commandlist[0],"cd")!=0)
                    {
                        if(execvp(commandlist[0],commandlist)==-1)
                        {
                            printf("Shell: Incorrect command\n");
                            exit(EXIT_FAILURE);
                        }
                    }
                }
            }
        }
    }
    for(l = 0;l<i;l++)
    {
        waitpid(pid[l],&status,WUNTRACED);
        if(!WIFEXITED(status))
        {
            kill(pid[l],SIGINT);
        }
    }
}

void executeSequentialCommands(char **argv)
{
    // This function will run multiple commands in Sequential order
    int i,status;
    pid_t rc;
    size_t k = 0;
    char *commandlist[MAXSIZE];
    for(i=0;argv[i]!=NULL;i++)
    {
        if(strlen(argv[i])!=k)
        {
            parseSpace(argv[i],commandlist);
            if(commandlist[0]!=NULL&&strcmp(commandlist[0],"cd")==0)
            {
                if(chdir(commandlist[1])==-1)
                {
                    perror("Error");
                }
            }
            else if(commandlist[0]!=NULL)
            {
                rc = fork();
                if(rc<0)
                {
                    printf("Process Creation Failed\n");
                    exit(0);
                }
                else if(rc==0)
                {
                    signal(SIGINT,SIG_DFL);
                    signal(SIGTSTP,SIG_DFL);
                    if(strcmp(commandlist[0],"cd")!=0)
                    {
                        if(execvp(commandlist[0],commandlist)==-1)
                        {
                            printf("Shell: Incorrect command\n");
                            exit(EXIT_FAILURE);
                        }
                    }
                }
                else
                {
                    waitpid(rc,&status,WUNTRACED);
                }
            }
        }
    }
}

void executeCommandRedirection(char **argv)
{
    // This function will run a single command with output redirected to an output file specificed by user
    char *commandlist[MAXSIZE];
    char *outputfilename[MAXSIZE];
    parseSpace(argv[1],outputfilename);
    parseSpace(argv[0],commandlist);
    int rc = fork();
    if(rc<0)
    {
        printf("Process Creation Failed!\n");
        exit(0);
    }
    else if(rc==0)
    {
        signal(SIGINT,SIG_DFL);
        signal(SIGTSTP,SIG_DFL);
        int fd;
        // after ">" contains path to open output file
        // create or ( writeonly or appen if exits) and give permissions to read, write and execute
        // return -1 if failed else new file descriptor
      	if((fd = open(outputfilename[0], O_CREAT | O_WRONLY | O_APPEND,S_IRWXU))==  -1 )
        {
            perror("Creation of file failed");
        }
        dup2(fd,1);
        close(fd);
        if(strcmp(commandlist[0],"cd")!=0)
        {
            if(execvp(commandlist[0],commandlist)==-1)
            {
                printf("Shell: Incorrect command\n");
            }
            exit(0);
        }
    }
    else
    {
	    int status;
        waitpid(rc,&status,WUNTRACED);
    }
}

int main()
{
    // Initial declarations
    char *argv[MAXSIZE];
    // Ignore ctrl + c and ctrl + z in parent process
    signal(SIGINT,SIG_IGN);
    signal(SIGTSTP,SIG_IGN);
    while(1)        // This loop will keep your shell running until user exits.
    {
        // Print the prompt in format - currentWorkingDirectory$
        char pwd[MAX_BUF];
        getcwd(pwd,MAX_BUF);
        printf("%s$",pwd);
        char *buffer;
        size_t bufsize = 32;
        int ch_len;

        buffer = (char *)malloc(bufsize * sizeof(char));
        if( buffer == NULL)
        {
            perror("Unable to allocate buffer");
            exit(1);
        }

        // accept input with 'getline()'
        ch_len = getline(&buffer,&bufsize,stdin);
        buffer[strlen(buffer)-1] = '\0';

        // Parse input with 'strsep()' for different symbols (&&, ##, >) and for spaces.
        int out = parseInput(buffer,argv);
        if(argv[0]==NULL)
        {
            continue;
        }
        if(out==0)    // When user uses exit command.
        {
            printf("Exiting shell...\n");
            break;
        }
        else if(out==1)
        {
            // This function is invoked when user wants to run multiple commands in parallel (commands separated by &&)
            executeParallelCommands(argv);
        }
        else if(out==2)
        {
            // This function is invoked when user wants to run multiple commands sequentially (commands separated by ##)
            executeSequentialCommands(argv);
        }
        else if(out==3)
        {
            // This function is invoked when user wants redirect output of a single command to and output file specificed by user
            executeCommandRedirection(argv);
        }
        else
        {
            // as cd is builtin command
            if(strcmp(argv[0],"cd")==0)
            {
                if(chdir(argv[1])==-1) // change working directory
                {
                    printf("Shell: Incorrect command\n");
                }
            }
            else
            {
                executeCommand(argv); // This function is invoked when user wants to run a single commands
            }
        }
    }
    return 0;
}
