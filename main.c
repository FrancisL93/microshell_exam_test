#include "microshell.h"

//receive command line as argument
//do not build the path
//implement | and ;
//built in cd with path (if err arguments  print on stderr "error: cd: bad arguments\n")
//if cd not access write "error: cd: cannot change directory to path_to_change\n"
void	set_pipe(t_vars *vars, int i)
{
	if (vars->position[i] != LAST) 
	{
		dup2(vars->fd[1], STDOUT_FILENO);
		close(vars->fd[1]);
		close(vars->fd[0]);
	}
}

void	set_fds(t_vars *vars, int i)
{
	if (vars->position[i] != LAST)
	{
		pipe(vars->fd);
	}
}

void	exe(t_vars *vars, char **envp)
{
	pid_t	pid;
	int	i;

	i = 0;
	while (vars->cmds[i])
	{
		set_fds(vars, i);
		pid = fork();
		if (pid == 0) {
			set_pipe(vars, i);
			execve(vars->cmds[i][0], vars->cmds[i], envp);
			write(STDERR_FILENO, "error: cannot execute ", 22);
			write(STDERR_FILENO, vars->cmds[i][0], ft_strlen(vars->cmds[i][0]));
			write(STDERR_FILENO, "\n", 1);
			exit(127);
		}
		if (vars->position[i] != LAST)
		{
			dup2(vars->fd[0], STDIN_FILENO);
			close(vars->fd[0]);
			close(vars->fd[1]);
		}
		wait(&pid);
		i++;
	}
}

void	set_stops(t_vars *vars) {
	for (int i = 0; vars->cmds[i]; i++) {
		for (int j = 0; vars->cmds[i][j]; j++) {
			if (!strcmp(vars->cmds[i][j], "|") || !strcmp(vars->cmds[i][j], ";"))
				vars->cmds[i][j] = NULL;
		}
	}
}

void	set_commands(t_vars *vars, char **argv)
{
	int	cmd_list;

	cmd_list = 0;
	for (int i = 0; argv[i]; i++) {
		if (i == 0 || !strcmp(argv[i], "|") || !strcmp(argv[i], ";"))
		{
			vars->cmds[cmd_list] = &argv[++i];
			if (i == 1 || !strcmp(argv[i - 1], ";"))
				vars->position[cmd_list] = FIRST;
			else if (argv[i + 1] == NULL || !strcmp(argv[i + 1], ";"))
				vars->position[cmd_list] = LAST;
			else
				vars->position[cmd_list] = OTHER;
			cmd_list++;
		}
	}
	vars->position[cmd_list - 1] = LAST;
	vars->cmds[cmd_list] = NULL;
}

void	print_commands(t_vars *vars) {
	for (int i = 0; vars->cmds[i]; i++) {
		printf("Command %d: ", i + 1);
		for (int j = 0; vars->cmds[i][j]; j++) {
			printf("%s ", vars->cmds[i][j]);
		}
		printf(" Position: %d\n", vars->position[i]);
	}
}

int main(int argc, char **argv, char **envp)
{
	t_vars	vars;

	if (argc == 1)
		return (0);
	vars.cmds = malloc(sizeof(char **) * count_commands(argv, 0));
	vars.position = malloc(sizeof(int) * count_commands(argv, 0));
	set_commands(&vars, argv);
	set_stops(&vars);
	print_commands(&vars);
	exe(&vars, envp);
	free(vars.cmds);
	free(vars.position);
	return (0);
}
