#include "header.h"
static char * getword(char *begin, char **end_ptr)
{
	char *end =begin;
	while(*begin == ' ')
	begin++;

	end = begin;
	while(*end != '\0' && *end != '\n' && *end != ' ')
	end++;

	if(end == begin)
		return NULL;

	*end ='\0';
	*end_ptr = end;

	return begin;
}

static void getargs(char cmd[], int *argcp, char *argv[]) {
	char *cmdp = cmd;
	char *end;
	int i=0;

	if(fgets(cmd,MAXLINE,stdin) == NULL && feof(stdin)) {
		printf("\n Couldn't read from standard input. End of file? Exiting....\n");
		exit(1); }

	while((cmdp = getword(cmdp,&end)) != NULL && *cmdp != '#') {
		if(*cmdp == '|') is_pipe = true;
		
		if(*cmdp == '<' || *cmdp == '>') is_redirect = true;

		argv[i++] = cmdp;
		cmdp = end +1;
		}
		argv[i] = NULL;
		*argcp = i;
}
	

static void parse_pipe(char *left[],char *right[],char *argv[]) {
	int a,b;
	for(a=0; *argv[a]!='|'; a++) left[a] = argv[a];
	left[a++] = '\0';

	for(b=0; *argv[a] != '|', argv[a] != NULL; a++,b++) right[b] = argv[a];
		right[b] = '\0';
}		

int handle_pipe(char *argv[]) {
	int pipe_fd[2];
	int fd;

	pid_t child1,child2;
	char *left[MAXARGS],*right[MAXARGS];

	parse_pipe(left, right, argv);
	fflush(stdout);

	if(-1 == pipe(pipe_fd) )
		perror("pipe");

	child1=fork();
	if(child1 > 0) child2 = fork();
	if(child1 == 0) {
		if(-1 == close(STDOUT)) perror("close");
		
	fd = dup(pipe_fd[1]);
	if(-1 == fd) perror("dup");

	if(fd != STDOUT) fprintf(stderr, "Pipe output not at STDOUT.\n");
	close(pipe_fd[0]);
	close(pipe_fd[1]);

	if(-1== execvp(left[0],left)) 
		perror("execvp");

} else if(child2 == 0) {
	if(-1 == close(STDIN) ) perror("close");
	fd = dup(pipe_fd[0]);

	if(-1 == fd) perror("dup");
	if(fd != STDIN) fprintf(stderr, "pipe input not atSTDIN.\n");

	close(pipe_fd[0]);
	close(pipe_fd[1]);

	if(-1 == execvp(right[0], right)) perror("execvp");
	} else {
		
	int status;
	
	close(pipe_fd[0]);
	close(pipe_fd[1]);

	if(-1 == waitpid(child1, &status, 0) ) perror("waitpid");

	if(WIFEXITED(status) == 0)
		printf("child1 returned w/ error code %d\n",WEXITSTATUS(status));

	if(-1 == waitpid(child2,&status,0) ) perror("waitpid");

	if(WIFEXITED(status) == 0)
		printf("child2 returned w/ error code %d \n",WEXITSTATUS(status));
}
	return 0;
}
	
static void execute_redirect(char *argv[])
{
	char *command[MAXARGS];
	int i,count=0,tokens=0,input,output,append;

	for(i=0;argv[i]!=NULL; i++) {
		if(*argv[i] == '<' || *argv[i] == '>')
			i++;
		else{
			command[tokens]=argv[i];
			tokens++;
		}
	}
	command[tokens]=NULL;

	while(argv[count]!=NULL) {
	if(*argv[count] == '<') {
		input = open(argv[count+1],O_RDONLY);
		pid_t pid = fork();

		if(pid == 0) {
			close(STDIN_FILENO);
			dup2(input, 0);
			execvp(command[0],command);
		}
		wait(NULL);
		}

	else if(*argv[count] == '>') {
		output = open(argv[count +1],O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
		if(output == -1)
			perror("open for reading");

		pid_t pid = fork();
		if(pid == 0) {
			close(STDOUT_FILENO);
			dup2(output,1);
			execvp(command[0],command);
		}
	}
			 	 
	
	wait(NULL);
	count++;
	}
}



static void execute(int argc, char *argv[]) {
	if (is_pipe) {
		handle_pipe(argv);
		is_pipe=false;
	}

	else if(is_redirect) {
	execute_redirect(argv);
	is_redirect=false;
	}


	else {
		pid_t childpid;
		childpid = fork();
	
	if(childpid == -1) {
		perror("fork");
		printf(" (failed to execute command) \n");
	}

	if(childpid==0) {
	
	if(execvp(argv[0],argv) == -1) {
		perror("execvp");
		printf(" (couldn't find command)\n");
	}
		exit(1);
	} else
		waitpid(childpid,NULL,0);
	}
	return;
}

void handle_signal(int signum) {
	return; }

int main(int argc, char *argv[]) {

	signal(SIGINT, handle_signal);
	char cmd[MAXLINE];
	char *childargv[MAXARGS];

	int childargc;

	while(1) {
		if(argc < 2)
		printf("$ ");

			fflush(stdout);
			getargs(cmd, &childargc, childargv);
			
					
		execute(childargc,childargv);
	}
}  

