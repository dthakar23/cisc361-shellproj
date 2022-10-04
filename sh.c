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
#include "sh.h"

int sh( int argc, char **argv, char **envp )
{
  char *prompt = calloc(PROMPTMAX, sizeof(char));
  char *commandline = calloc(MAX_CANON, sizeof(char));
  char *command, *arg, *commandpath, *p, *pwd, *owd;
  char **args = calloc(MAXARGS, sizeof(char*));
  int uid, i, status, argsct, go = 1;
  struct passwd *password_entry;
  char *homedir;
  struct pathelement *pathlist;

  char buffer[MAXBUFFER];

  uid = getuid();
  password_entry = getpwuid(uid);               /* get passwd info */
  homedir = password_entry->pw_dir;		/* Home directory to start
						  out with*/
     
  if ( (pwd = getcwd(NULL, PATH_MAX+1)) == NULL )
  {
    perror("getcwd");
    exit(2);
  }
  owd = calloc(strlen(pwd) + 1, sizeof(char));
  memcpy(owd, pwd, strlen(pwd));
  prompt[0] = ' '; prompt[1] = '\0';

  /* Put PATH into a linked list */
  pathlist = get_path();

  while ( go )
  {
    /* print your prompt */
    printf("\n%s [%s]> ", prompt, pwd);

    if(fgets(buffer, MAXBUFFER, stdin)!=NULL){
        int ln = strlen(buffer);
        if(buffer[ln-1]== '\n'){
            buffer[ln-1] = 0;
        }
        strcpy(commandline, buffer);
    }

    int a=0;
    char *checker = strtok(commandline, " ");
    command = checker;
    memset(args, '\0', MAXARGS*sizeof(char*));
    while(checker){
        args[a] = checker;
        checker = strtok(NULL, " ");
        a++;
    }

    /* get command line and process */
    if (strcmp(command, "which") == 0){ //checking to see if which is the command typed in
        for(int a=1; args[a] != NULL; a++){
            commandpath = which(args[a], pathlist); //runs which function defined in this file
            printf("\n%s", commandpath);
            //free(commandpath); //freeing up memory for valgrind
        }
    }

    /* check for each built in command and implement */

     /*  else  program to exec */
    {
       /* find it */
       /* do fork(), execve() and waitpid() */

      /* else */
        /* fprintf(stderr, "%s: Command not found.\n", args[0]); */
    }
  }
  return 0;
} /* sh() */

char *which(char *command, struct pathelement *pathlist )
{
   /* loop through pathlist until finding command and return it.  Return
   NULL when not found. */

    char buffer[MAXBUFFER];
    int found;
    found = 0;

    while (pathlist)
    {
        sprintf(buffer, MAXBUFFER, "%s/%s", pathlist->element, command);

        if (access(buffer, X_OK) == 0)
        {
            found = 1;
            break;
        }
        pathlist = pathlist -> next;
        if (found)
        {
           int ln = strlen(buffer);
           char *space = calloc(ln + 1, sizeof(char));
           strncpy(space, buffer, ln);
           return space;
        }
        else {
            return (char *) NULL;
        }
   }

} /* which() */

char *where(char *command, struct pathelement *pathlist )
{
  /* similarly loop through finding all locations of command */
} /* where() */

void list ( char *dir )
{
  /* see man page for opendir() and readdir() and print out filenames for
  the directory passed */
} /* list() */

