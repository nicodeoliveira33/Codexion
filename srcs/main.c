#include "codexion.h"


int main(int ac, char **av)
{
	t_rules	rules;
	t_coder	*coders;

	if (ac != 9 || parser(ac, av, &rules) == 1)
	{
		write(2, "Invalid input!\n", 15);
		return (1);
	}
	coders = NULL;
	if (init_simulation(&rules, &coders) != 0)
	{
		write(2, "Init Error!\n", 12);
		cleanup(&rules, coders);
		return (1);
	}
	start_simulation(&rules, coders);
	cleanup(&rules, coders);
	return (0);
}
