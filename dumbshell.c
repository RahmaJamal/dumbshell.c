#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>
#include <libgen.h>
#include <pwd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>
#include <time.h>

#define FORMAT "%6s %s\t%8s %s\n"
#define MAXCOM 1000 // max number of letters to be supported
#define MAXLIST 100 // max number of commands to be supported
#define      DEBUG        1
#define      MAXLINELEN   4096
#define      MAXARGS             128
#define      END_OF_LINE  0
#define      SEQ_OP       ';'
#define      SEQUENCE     1
#define clear() printf("\033[H\033[J") // Clearing the shell using escape sequences
#define _GNU_SOURCE

char  *line;
  void init_shell()
{
	clear();	
	printf("\n\n\n\n\n\n\t-USE AT YOUR OWN RISK-");
	
	char* username = getenv("USER");
	printf("\n\n\n         USER is: @%s", username);
	printf("\n\n\n\n");
	printf("\n\n\n\n");
	printf("\n");
	sleep(2);
	clear();
}


struct cmd {

struct cmd   *next;
int          terminator;
char         *exe_path;
int          nargs;
char         *arg[MAXARGS];
};

void   *ck_malloc(size_t   size)
{
void   *ret = malloc(size);
if (!ret) {
perror("dumbshell:ck_malloc");
exit(1);
}
return ret;
}

char   *skip_to_non_ws(char       *p)
{
int    ch;
while (ch = *p) {
if (ch != ' ' && ch != '\t' && ch != '\n') return p;
++p;
}
return 0;
}

char   *skip_to_ws_or_sep(char    *p)
{
int    ch;
while (ch = *p) {
if (ch == ' ' || ch == '\t' || ch == '\n') return p;
if (ch == SEQ_OP) return p;
++p;
}
return 0;
}

struct cmd *parse_commands(char  *line)
{
char         *ptr; int          ix; struct cmd   *cur;

ptr = skip_to_non_ws(line);
if (!ptr) return 0;
cur = ck_malloc(sizeof *cur);
cur->next = 0;
cur->exe_path = ptr;
cur->arg[0] = ptr;
cur->terminator = END_OF_LINE;
ix = 1;
for (;;) {
ptr = skip_to_ws_or_sep(ptr);
if (!ptr) {
break;
}
if (*ptr == SEQ_OP) {
*ptr = 0;
cur->next = parse_commands(ptr+1);
if (cur->next) {
cur->terminator = SEQUENCE;
}
break;
}
*ptr = 0;
ptr = skip_to_non_ws(ptr+1); 
if (!ptr) {
break;
}
if (*ptr == SEQ_OP) {
/* found a sequence operator */
cur->next = parse_commands(ptr+1);
if (cur->next) {
cur->terminator = SEQUENCE;
}
break;
}
cur->arg[ix] = ptr;
++ix;
}
cur->arg[ix] = 0; cur->nargs = ix; return cur;
}

//history function
int history_code(char **history, int count)
{
    int i;
    
    if(count == 1) {
        printf("history is empty!\n");
        return 1;
    }
    
    for (i=0; i < count-1; i++) { 
        printf("%s\n",history[i]);
    }
    
    return 1;
}

int print_history(char **history, int count, char *args)
{
    
    if (count == 0) {
        printf("no previous command!\n");
        return 1;
    }
    else {
        if (strcmp(history[count-1], list[0]) == 0) {
            cd_code(args);
        }
        if (strcmp(history[count-1], list[1]) == 0) {
            pwd_code();
        }
        if (strcmp(history[count-1], list[2]) == 0) {
            history_code(history, count);
        }
        
    }
    return 1;
}
void   execute(struct cmd  *clist)
{

//cd command
if( strcmp(clist->exe_path,"cd")==0){
  if (clist->nargs == 1) {
        chdir(getenv("HOME"));// go to home
    } else {
        char* file = clist->arg[1];

        if (!strcmp(file, "~") || !strcmp(file, " ") || !strcmp(file, "")) {
            char* home = getenv("HOME");
            chdir(home);
        } else {
            chdir(clist->arg[1]);// change to another dir or the previos
        }

 
 
}

}

else{


int    pid, npid, stat;

pid = fork();
if (pid == -1) {
perror("dumbshell:fork");
exit(1);
}


if (!pid) {

// pwd command
 char s[1024];
   if( strcmp(clist->exe_path,"pwd")==0){
 printf("%s\n", getcwd(s, sizeof(s)));
}
    else{
    perror("getcwd() error");
}


//kill command
if( strcmp(clist->exe_path,"kill")==0){
 if( clist->nargs==5){
  kill(atoi(clist->arg[2]),atoi(clist->arg[1]));
  kill(atoi(clist->arg[3]),atoi(clist->arg[1]));
  kill(atoi(clist->arg[4]),atoi(clist->arg[1]));
 }
 else
    kill(atoi(clist->arg[1]),SIGKILL);
    perror("kill");
 } 
 /*
 if( strcmp(clist->exe_path,"kill")){
 int pid;
 if( strcmp(clist->arg[1],pid)){
  kill(atoi(clist->arg[1]),SIGTERM);
 }
 else if (strcmp(clist->arg[2]," -l")==0){
 printf("SIGHUP, 1\n");
printf("SIGTERM, 15\n");
printf("SIGKILL, 9\n");
 }
 else{

if((clist->arg[1] == SIGTERM) || (clist->arg[1] == 15)){
kill(pid, SIGTERM);
return(0);

}
else{
if((clist->arg[1]== SIGHUP) || (clist->arg[1] == 1)){
kill(pid, SIGHUP);
printf("Killed.\n");
system("ps");
return(0);
}
else{
if((clist->arg[1] == SIGKILL) || (clist->arg[1] == 9)){
kill(pid, SIGKILL);
}
}
    
 }}} */
 
 
else{
  execvp(clist->exe_path,clist->arg);
}

exit(1); 
}


//ps command
if( strcmp(clist->exe_path,"ps")==0){
DIR *directory;
struct dirent *ent; //This is a structure type used to return information about directory entries 
  int i, fd_self, fd;
  unsigned long time, stime;
  char flag, *tty;
  char cmd[300], tty_self[300], path[300], time_s[300];
  FILE* file;
  

  directory = opendir("/proc");/*function opens a directory stream corresponding to the directory name, 
  and returns a pointer to the directory stream. 
  The stream is positioned at the first entry in the directory.*/
  
  fd_self = open("/proc/self/fd/0", O_RDONLY); //read only for dir
  sprintf(tty_self, "%s", ttyname(fd_self));
  

  while ((ent = readdir(directory)) != NULL)
  /*returns a pointer to a dirent structure representing the next directory entry in the directory stream pointed to by dirp. 
  It returns NULL on reaching the end of the directory stream or if an error occurred.*/
  {
  flag = 1; //statement as true
  for (i = 0; ent->d_name[i]; i++)
  if (!isdigit(ent->d_name[i]))
  { 
   flag = 0; //statement as false
   break;
  }

  if (flag)
  {
  
  fd = open(path, O_RDONLY);
  tty = ttyname(fd);

  if (tty && strcmp(tty, tty_self) == 0)
  {//reduce score to show just the current active processes

   sprintf(path, "/proc/%s/stat", ent->d_name);
   file = fopen(path, "r");
   fscanf(file, "%d%s%c%c%c", &i, cmd, &flag, &flag, &flag);
   cmd[strlen(cmd) - 1] = '\0';

  for (i = 0; i < 11; i++)
  fscanf(file, "%lu", &time);
  fscanf(file, "%lu", &stime);
  time = (int)((double)(time + stime) / sysconf(_SC_CLK_TCK));
  sprintf(time_s, "%02lu:%02lu:%02lu",
  (time / 3600) % 3600, (time / 60) % 60, time % 60);
 
  printf(FORMAT, ent->d_name, tty + 5, time_s, cmd + 1);
  fclose(file);
  }
 close(fd);
}
}
close(fd_self);
}


//ps -A

if( strcmp(clist->exe_path,"ps -A")==0){

DIR *directory;
struct dirent *ent;
  int i, fd_self, fd;
  unsigned long time, stime;
  char flag, *tty;
  char cmd[300], tty_self[300], path[300], time_s[300];
  FILE* file;
  

  directory = opendir("/proc");
  fd_self = open("/proc/self/fd/0", O_RDONLY);
  sprintf(tty_self, "%s", ttyname(fd_self));
  

  while ((ent = readdir(directory)) != NULL)
  {
  flag = 1;
  for (i = 0; ent->d_name[i]; i++)
  if (!isdigit(ent->d_name[i]))
  { 
   flag = 0;
   break;
  }

  if (flag)
  {
  
  fd = open(path, O_RDONLY);//remove the if statment to show all the processes in system

   sprintf(path, "/proc/%s/stat", ent->d_name);
   file = fopen(path, "r");
   fscanf(file, "%d%s%c%c%c", &i, cmd, &flag, &flag, &flag);
   cmd[strlen(cmd) - 1] = '\0';

  for (i = 0; i < 11; i++)
  fscanf(file, "%lu", &time);
  fscanf(file, "%lu", &stime);
  time = (int)((double)(time + stime) / sysconf(_SC_CLK_TCK));
  sprintf(time_s, "%02lu:%02lu:%02lu",
  (time / 3600) % 3600, (time / 60) % 60, time % 60);
 
  printf(FORMAT, ent->d_name, time_s, cmd + 1);
  fclose(file);
 close(fd);
}
}
close(fd_self);
}


do {
npid = wait(&stat);
printf("Process %d exited with status %d\n",npid,stat); 
} while (npid != pid);
}
switch (clist->terminator) {
case SEQUENCE:
execute(clist->next);
}
}

void   free_commands(struct cmd   *clist)
{
struct cmd   *nxt;

do {
nxt = clist->next;
free(clist);
clist = nxt;
} while (clist);
}

char	*get_command(char   *buf, int	size, FILE	*in)
{
     char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    char **history;
 int count;
 char *args;

char prmptn [2024];
char* name = getenv("USER");
char logn [1024];
gethostname(logn, 1024);

snprintf (prmptn, sizeof (prmptn), "%s@%s:~/%s$ ",name,logn,basename(cwd));
//add_history(in);
add_history(print_history(history, count, in));
if (in == stdin) {
fputs(prmptn,stdout); /* prompt */

}

return fgets(buf,size,in);
}



void   main(void)
{
char **history;
 int count;
 char *args;
init_shell();
char         linebuf[MAXLINELEN];
struct cmd   *commands;

while (get_command(linebuf,MAXLINELEN,stdin) != NULL) {
commands = parse_commands(linebuf);

if (commands) {
execute(commands);
free_commands(commands);
}

}
}
