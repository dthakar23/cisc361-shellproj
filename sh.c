#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <wordexp.h>
#include "sh.h"

int sh(int argc, char **argv, char **envp)
{
    extern char** environ;
    char *prompt = calloc(PROMPTMAX, sizeof(char));
    char *commandline = calloc(MAX_CANON, sizeof(char));
    char *command, *arg, *commandpath, *p, *pwd, *owd;
    char **args = calloc(MAXARGS, sizeof(char *));
    int uid, i, status, argsct, go = 1;
    struct passwd *password_entry;
    char *homedir;
    struct pathelement *pathlist;

    char buffer[MAXBUFFER];
    char pBuffer[PROMPTMAX];

    uid = getuid();
    password_entry = getpwuid(uid);   /* get passwd info */
    homedir = password_entry->pw_dir; /* Home directory to start out with*/

    if ((pwd = getcwd(NULL, PATH_MAX + 1)) == NULL)
    {
        perror("getcwd");
        exit(2);
    }
    owd = calloc(strlen(pwd) + 1, sizeof(char));
    memcpy(owd, pwd, strlen(pwd));
    prompt[0] = ' ';
    prompt[1] = '\0';

    /* Put PATH into a linked list */
    pathlist = get_path();

    while (go)
    /* runs through a loop and checks for built in commands */
    {
        /* print your prompt */
        printf("\n%s [%s]> ", prompt, pwd);

        if (fgets(buffer, MAXBUFFER, stdin) != NULL)
        {
            int ln = strlen(buffer);
            if (buffer[ln - 1] == '\n')
            {
                buffer[ln - 1] = 0;
            }
            strcpy(commandline, buffer);
        }

        /* get command line and process */
        int a = 0;
        char *checker = strtok(commandline, " ");
        command = checker;
        memset(args, '\0', MAXARGS * sizeof(char *));
        while (checker)
        {
            args[a] = checker;
            checker = strtok(NULL, " ");
            a++;
        }

        if (command != NULL) //checks if valid command
        {
            /* check for built in "exit" command and implement */
            if (strcmp(command, "exit") == 0)
            {
                printf("\nExecuting built-in [%s]\n", command);
                exit(0);
                break;
            }

            /* check for built in "which" command and implement */
            else if (strcmp(command, "which") == 0)
            {
                printf("\nExecuting built-in [%s] \n", command);
                for (int a = 1; args[a] != NULL; a++)
                {
                    commandpath = which(args[a], pathlist); // runs which function defined in this file
                    printf("\n%s", commandpath);
                    free(commandpath);
                }
            }

            /* check for built in "where" command and implement */
            else if (strcmp(command, "where") == 0)
            {
                printf("\nExecuting built-in [%s] \n", command);
                for (int a = 1; args[a] != NULL; a++)
                {
                    commandpath = where(args[a], pathlist);
                }
            }

            /* check for built in "cd" command and implement */
            else if (strcmp(command, "cd") == 0)
            {
                printf("\nExecuting built-in [%s]", command);
                if (args[1] == NULL) {
                    strcpy(owd, pwd);
                    strcpy(pwd, homedir);
                    chdir(pwd);
                }
                else if (strcmp(args[1], "-") == 0) {
                    char *temp = pwd;
                    pwd = owd;
                    owd = pwd;
                    pwd = getcwd(NULL, PATH_MAX+1);
                    strcpy(pwd, owd);
                    strcpy(owd, temp);
                    chdir(pwd);
                }
                else if (args[1] != NULL && args[2] == NULL) {
                    if (chdir(args[1]) == -1) {
                        perror("Error");
                    }
                    else {
                        memset(owd, '\0', strlen(owd));
                        memcpy(owd, pwd, strlen(pwd));
                        getcwd(pwd, PATH_MAX+1);
                    }
                }
            }

            /* check for built in "pwd" command and implement */
            else if (strcmp(command, "pwd") == 0)
            {
                printf("\nExecuting built-in [%s] \n", command);
                printf("\npwd: %s", pwd);
            }

            /* check for built in "list" command and implement */
            else if (strcmp(command, "list") == 0) 
            {
                printf("\nExecuting built-in [%s] \n", command);
                if (args[1] == NULL)
                { 
                    list(pwd);
                }
                else
                {
                    int i = 1;
                    while (args[i])
                    {
                        if (access(args[i], X_OK) == -1)
                        {
                            perror("\nError ");
                        }
                        else
                        {
                            printf("\n%s:\n", args[i]);
                            list(args[i]);
                        }
                        i++;
                    }
                }
            }

            /* check for built in "pid" command and implement */
            else if (strcmp(command, "pid") == 0)
            {
                printf("\nExecuting built-in [%s] \n", command);
                printf("%d\n", getpid());
            }

            /* check for built in "kill" command and implement */
            else if (strcmp(command, "kill") == 0)
            {
                printf("\nExecuting built-in [%s] \n", command);
                if (args[1] == NULL) {
                    printf("\n Specify signal to kill");
                }
                else if (args[2] == NULL) { // given just a pid
                    int pid = atoi(args[1]);
                    kill(pid, SIGTERM); //sends signal to kill
                }
                else if (args[3] == NULL) {
                    int pid = atoi(args[2]);
                    int sig = atoi(args[1]);
                    sig = sig * -1;         // signal number with - in front 
                    kill(pid, sig);
                }
                else {
                    printf("\nInvalid arguments"); // prints this message if too many or too little arguments
                }
            }

            /* check for built in "prompt" command and implement */
            else if (strcmp(command, "prompt") == 0)
            {
            printf("\nExecuting built-in [%s] \n", command);

                if (args[1] == NULL) {
                    printf("\nType your prefix: ");
                    if (fgets(pBuffer, PROMPTMAX, stdin) != NULL) {
                        int len = strlen(pBuffer);
                        if (pBuffer[len-1] == '\n') {
                            pBuffer[len-1] = 0;
                        }
                        strtok(pBuffer, " ");
                        strcpy(prompt, pBuffer);
                    }
                }
                else {
                    strcpy(prompt, args[1]);
                }
            }

            /* check for built in "printenv" command and implement */
            else if (strcmp(command, "printenv") == 0)
            {
                printf("\nExecuting built-in [%s] \n", command);

                if (args[1]==NULL){
                    for (int i=0; environ[i] !=NULL; i++){
                        printf("%s\n", environ[i]);
                    }
                } else {
                    printf("%s\n", getenv(args[1]));
                }
            }

            /* check for built in "setenv" command and implement */
            else if (strcmp(command, "setenv") == 0)
            {
                printf("\nExecuting built-in [%s] \n", command);
                if (args[1] == NULL) {
                    printenv(environ);  // given no arguments acts like printenv
                }
                else if (args[2] == NULL) {
                    // one argument set as an empty environment variable 
                    if (strcmp(args[1],"PATH") == 0 || strcmp(args[1],"HOME") == 0) {
                        printf("\nWARNING: Take special care when PATH and HOME are changed");
                    }
                    else if (setenv(args[1], "", 1) == -1) {
                        perror("Error!");
                    }
                }
                else if (args[3] == NULL) {
                    if (setenv(args[1], args[2], 1) == -1) {
                        perror("Error!");
                    }
                    else {
                        if (strcmp(args[1], "PATH") == 0) {
                            pathlist = NULL;
                        }
                        else if (strcmp(args[1], "HOME") == 0) {
                            homedir = args[2];
                        }
                    }
                }
                else {
                    fprintf(stderr, "setenv: Too many arguments.\n");
                }
            }
            /*  else  program to exec */
            else
            {
                /* else program executes non-built-in functions */
                /* find it */
                /* do fork(), execve() and waitpid() */
                status = 0; 
                pid_t pid;
                if ((pid = fork()) < 0)
                {
                    perror("\nError");
                }
                else if (pid == 0)
                {
                    /* check for "./ or /" absolute path and implement */
                    char* p=which(command,pathlist);
                    if (!p){
                        p=calloc(MAXBUFFER,sizeof(char));
                        strcpy(p,command);
                    }    
                    /* check for "*" "?" wild card */
                    if (strchr(command, '*') != NULL || strchr(command, '?') != NULL) {
                        printf("in the loop\n");
                        wordexp_t w;
                        char **wildcard;
                        int index; 
                        wordexp(command, &w, 0);
                        wildcard = w.we_wordv;
                        for (index = 0; index < w.we_wordc; index++) {
                            printf("we made \n");
                            printf("%s\n", wildcard[index]);
                        }
                    }
                    else {
                        execve(p, args, environ);
                        free(p);
                        printf("Command Not Found\n");
                        exit(-1);
                    }
                }
                else
                {
                    status=0;
                    waitpid(pid, &status, 0);
                    
                }
                /* else */
            }
        }
    }
    return 0;
} /* sh() */

char *which(char *command, struct pathelement *pathlist)
{
    /* loop through pathlist until finding command and return it.  Return
    NULL when not found. */

    char buffer[MAXBUFFER];
    int check;
    check = 0;

    while (pathlist)
    {
        sprintf(buffer, "%s/%s", pathlist->element, command);

        if (access(buffer, X_OK) == 0)
        {
            int ln = strlen(buffer);
            char *space = calloc(ln + 1, sizeof(char));
            strncpy(space, buffer, ln);
            return space;
        }
        pathlist = pathlist->next;
    }
    return (char *)NULL;
} /* which() */

char *where(char *command, struct pathelement *pathlist)
{
    /* similarly loop through finding all locations of command */
    char buffer[MAXBUFFER];
    int check;
    check = 0;

    while (pathlist)
    {
        sprintf(buffer, "%s/%s", pathlist->element, command);

        if (access(buffer, X_OK) == 0)
        {
            printf("\n%s", buffer);
        }
        pathlist = pathlist->next;
    }
    return (char *)NULL;
} /* where() */

void list(char *dir)
{
    /* see man page for opendir() and readdir() and print out filenames for
    the directory passed */    
    DIR* mydir = opendir(dir);
    struct dirent* myfile;
    if(mydir){
        while((myfile = readdir(mydir)) != NULL){
            printf("%s\n", myfile->d_name);
        }   
    }
    closedir(mydir);
} /* list() */

void printenv(char **envi){
    /* helper function that prints the environment */
    int i=0;
    for(i=0; envi[i]!=NULL; i++){
        printf("%s\n",envi[i]);
    }
} /* printenv() */
