# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/wait.h>
# include <unistd.h>

# define FIRST 	1
# define LAST	2

typedef struct s_vars
{
	int		fd[2];
	int		*position;
	char	***cmds;
} t_vars;

int	count_commands(char **argv)
{
	int count = 1;

	for (int j = 0; argv[j]; j++){
		if (!strcmp(argv[j], "|") || strcmp(argv[j], ";"))
			count++;
		}
	return (count);
}

int	ft_strlen(char *str) {
	int	i = 0;

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

int	check_cd(t_vars *vars, int i)
{
	if (!strcmp(vars->cmds[i][0], "cd")) {
		if (!vars->cmds[i][1] || vars->cmds[i][2])
			write(STDERR_FILENO, "error: cd: bad arguments\n", 25);
		else if (chdir(vars->cmds[i][1]) != 0) {
			write(STDERR_FILENO, "error: cd: cannot change directory to ", 38);
			write(STDERR_FILENO, vars->cmds[i][1], ft_strlen(vars->cmds[i][1]));
			write(STDERR_FILENO, "\n", 1);
		}
		return (1);
	}
	return (0);
}

void	exe(t_vars *vars, char **envp)
{
	pid_t	pid;
	int		ret = 0;

	for (int i = 0; vars->cmds[i]; i++) {
		ret = check_cd(vars, i);
		if (!ret && vars->position[i] != LAST)
			pipe(vars->fd);
		if (!ret)
			pid = fork();
		if (!ret && pid == 0) {
			set_pipe(vars, i);
			execve(vars->cmds[i][0], vars->cmds[i], envp);
			write(STDERR_FILENO, "error: cannot execute ", 22);
			write(STDERR_FILENO, vars->cmds[i][0], ft_strlen(vars->cmds[i][0]));
			write(STDERR_FILENO, "\n", 1);
			exit(127);
		}
		if (!ret && vars->position[i] != LAST)
		{
			dup2(vars->fd[0], STDIN_FILENO);
			close(vars->fd[0]);
			close(vars->fd[1]);
		}
		if (!ret)
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
	int cmd_list = 0;

	for (int i = 0; argv[i]; i++) {
		if (i == 0 || !strcmp(argv[i], "|") || !strcmp(argv[i], ";")) {
			while (argv[i + 1] && !strcmp(argv[i + 1], ";"))
				i++;
			if (!argv[i + 1])
				break ;
			vars->cmds[cmd_list++] = &argv[++i];
		}
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
	vars.position = malloc(sizeof(int) * count_commands(argv));
	vars.cmds = malloc(sizeof(char **) * count_commands(argv));
	set_commands(&vars, argv);
	set_stops(&vars);
	exe(&vars, envp);
	free(vars.cmds);
	free(vars.position);
	return (0);
}
