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

