#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <dirent.h>
#include <wordexp.h> //keep?
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "sh.h"

int sh(int argc, char **argv, char **envp)
{
    char *prompt = calloc(PROMPTMAX, sizeof(char));
    char *commandline = calloc(MAX_CANON, sizeof(char));
    char *command, *arg, *commandpath, *p, *pwd, *owd;
    char **args = calloc(MAXARGS, sizeof(char *));
    int uid, i, status, argsct, go = 1;
    struct passwd *password_entry;
    char *homedir;
    struct pathelement *pathlist;

    char buffer[MAXBUFFER];

    uid = getuid();
    password_entry = getpwuid(uid);   /* get passwd info */
    homedir = password_entry->pw_dir; /* Home directory to start
                        out with*/

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
                printf("\nExecuting built-in %s", command);
                //free pathlist and pathlist->element
                exit(0);
                break;
            }
            /* check for built in "which" command and implement */
            else if (strcmp(command, "which") == 0)
            {
                printf("\nExecuting built-in %s", command);
                for (int a = 1; args[a] != NULL; a++)
                {
                    commandpath = which(args[a], pathlist); // runs which function defined in this file
                    printf("\n%s", commandpath);
                }
            }
            /* check for built in "where" command and implement */
            else if (strcmp(command, "where") == 0)
            {
                printf("\nExecuting built-in %s", command);
                for (int a = 1; args[a] != NULL; a++)
                {
                    commandpath = where(args[a], pathlist);
                }
            }
            /* check for built in "cd" command and implement */ // how to do this one???
            else if (strcmp(command, "cd") == 0)
            {
                if (args[1] != NULL && args[2] == NULL)
                {
                    if (chdir(args[1]) == -1)
                    {
                        perror("Too many arguments ");
                    }
                    else
                    {
                        memset(owd, '\0', strlen(owd));
                        memcpy(owd, pwd, strlen(pwd));
                        getcwd(pwd, PATH_MAX + 1);
                    }
                } else {
                    printf("\nExecuting built-in %s", command); //only prints executing cd if there aren't errors
                    if (args[1] == NULL){
                        strcpy(owd, pwd);
                        strcpy(pwd, homedir);
                        chdir(pwd);
                    }
                    else if (strcmp(args[1], "-") == 0){
                        p = pwd;
                        pwd = owd;
                        owd = p;
                        chdir(pwd);
                    }
                }
            }
                /* check for built in "pwd" command and implement */
            else if (strcmp(command, "pwd") == 0)
            {
                printf("\nExecuting built-in %s", command);
                printf("\npwd: %s", pwd);
            }
            /* check for built in "list" command and implement */
            else if (strcmp(command, "list") == 0) //check this
            {
                printf("\nExecuting built-in %s", command);
                if (args[1] == NULL)
                { // 0 holds command
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
                printf("\nExecuting built-in %s", command);
                printf("%d\n", getpid());
            }
            /* check for built in "kill" command and implement */
            else if (strcmp(command, "kill") == 0)
            {
                printf("\nExecuting built-in %s", command);

            }

            /* check for built in "prompt" command and implement */
            else if (strcmp(command, "prompt") == 0)
            {
                printf("\nExecuting built-in %s", command);

            }
            /* check for built in "printenv" command and implement */
            else if (strcmp(command, "printenv") == 0)
            {
                printf("\nExecuting built-in %s", command);
                //printenv()

            }
            /* check for built in "setenv" command and implement */
            else if (strcmp(command, "setenv") == 0)
            {
                printf("\nExecuting built-in %s", command);

            }
            /* check for "./ or /" absolute path and implement */ //use access(2) to check
            else if ((strcmp(command, "/")==0) || (strcmp(command, "./"))==0 || (strcmp(command, "../"))){
                if (access(command, X_OK) == -1)
                {
                    printf("\nUnable to read command: %s", command);
                    perror("Error ");
                }
                else
                {
                    printf("\nExecuted path %s\n", command);
                    //how to get path to print??? and run an executeable
                }
            }

            /* check for "*" "?" wild card */
            else if ((strchr(command, "*")!=NULL) || (strchr(command, "?")!=NULL)){ //checks for first occurence of wildcard characters
                wordexp_t w; //pulled from wordexp.h library
                char **wildcard;
                int ind;
                wordexp(arg, &w, 0);
                wild = a.we_wordv;
                for (ind = sizeof(args); ind < w.we_wordc; ind++)
                {
                    printf("%s\n", wildcard[ind]);
                }
                //wordfree(&w);
            }

            /*  else  program to exec */
            {
                /* find it */
                /* do fork(), execve() and waitpid() */

                /* else */
                /* fprintf(stderr, "%s: Command not found.\n", args[0]); */
            }
        }
        //return 0;
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
            int ln = strlen(buffer);
            char *space = calloc(ln + 1, sizeof(char));
            strncpy(space, buffer, ln);
            //return space;
        }
        pathlist = pathlist->next;
    }
    return (char *)NULL;
} /* where() */

void list(char *dir)
{
    /* see man page for opendir() and readdir() and print out filenames for
    the directory passed */    
    DIR* adir = opendir(dir);
    struct dirent* afile;
    if(adir){
        while((afile = readdir(adir)) != NULL){
            printf("%s\n", afile->d_name);
        }   
    }
    closedir(adir);
} /* list() */

void printenv(char **envp){
    int i=0;
    for(i=0; envp[i]!=NULL; i++){
        printf("%s\n",envp[i]);
    }
}
