#include "microshell.h"

int	count_commands(char **argv, int array)
{
	int count = 1;

	for (int j = 0; argv[j]; j++){
		if (!strcmp(argv[j], "|") || strcmp(argv[j], ";"))
			count++;
			if (array)
				break;
		}
	return (count);
}

int	ft_strlen(char *str) {
	int	i;

	i = 0;
	while (str && str[i])
		i++;
	return (i);
}

void	set_pipe(t_vars *vars, int i)
{
	if (vars->position[i] != LAST) 
	{
		dup2(vars->fd[1], STDOUT_FILENO);
		close(vars->fd[1]);
		close(vars->fd[0]);
	}
}


void	exe(t_vars *vars, char **envp)
{
	pid_t	pid;

	for (int i = 0; vars->cmds[i]; i++) {
		if (vars->position[i] != LAST)
			pipe(vars->fd);
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
			vars->cmds[cmd_list++] = &argv[++i];
		if (cmd_list == 1 || !strcmp(argv[i - 1], ";"))
			vars->position[cmd_list - 1] = FIRST;
		if (!strcmp(argv[i - 1], ";"))
			vars->position[cmd_list - 2] = LAST;
	}
	vars->position[cmd_list - 1] = LAST;
	vars->cmds[cmd_list] = NULL;
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
	exe(&vars, envp);
	free(vars.cmds);
	free(vars.position);
	return (0);
}