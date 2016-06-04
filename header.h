#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include<sys/stat.h>

#define MAXLINE 200  
#define MAXARGS 20
#define STDIN 0
#define STDOUT 1

bool is_pipe = false;
bool is_redirect = false;

