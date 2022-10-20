#ifndef MICROSHELL_H
# define MICROSHELL_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/wait.h>
# include <unistd.h>

# define OTHER	0
# define FIRST 	1
# define LAST	2

typedef struct s_vars
{
	int		fd[2];
	int		*position;
	char	***cmds;
} t_vars;

#endif