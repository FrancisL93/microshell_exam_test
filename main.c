#include "microshell.h"

//receive command line as argument
//do not build the path
//implement | and ;
//built in cd with path (if err arguments  print on stderr "error: cd: bad arguments\n")
//if cd not access write "error: cd: cannot change directory to path_to_change\n"
void	exe(t_vars *vars, char **envp)
{
	pid_t	pid;

	for (int i = 0; vars->cmds[i]; i++) {
		pid = fork();
		if (pid == 0) {
			execve(vars->cmds[i][0], vars->cmds[i], envp);
			write(STDERR_FILENO, "error: cannot execute ", 22);
			for (int j = 0; vars->cmds[i][0][j] != '\0'; j++) {
				write(STDERR_FILENO, &vars->cmds[i][0][j], 1);
			}
			write(STDERR_FILENO, "\n", 1);
		}
		wait(&pid);
	}

}

void	set_commands(t_vars *vars, char **argv)
{
	int	cmd_list;

	cmd_list = 0;
	for (int i = 1; argv[i]; i++) {
		if (i == 1 || !strcmp(argv[i], "|") || !strcmp(argv[i], ";")) {
			vars->cmds[cmd_list] = &argv[i];
			if (i == 1 || !strcmp(argv[i - 1], ";")) {
				vars->position[cmd_list] = FIRST;
			}
			else if (argv[i + 1] == NULL || !strcmp(argv[i + 1], ";"))
				vars->position[cmd_list] = LAST;
			else
				vars->position[cmd_list] = OTHER;
			cmd_list++;
			if (!strcmp(argv[i], "|") || !strcmp(argv[i], ";"))
				argv[i] = NULL;
		}
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
	exe(&vars, envp);
	return (0);
}