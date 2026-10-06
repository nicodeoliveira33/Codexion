#include "codexion.h"

int	main(int ac, char **av)
{
	t_rules	rules;
	t_coder	*coders;

	memset(&rules, 0, sizeof(t_rules));
	if (ac != 9 || parser(ac, av, &rules) == 1)
	{
		write(2, "\nInvalid input!\n", 15);
		print_invalid_args();
		return (1);
	}
	coders = NULL;
	if (init_simulation(&rules, &coders) != 0)
	{
		write(2, "\nInit Error!\n", 12);
		return (1);
	}
	start_simulation(&rules, coders);
	cleanup(&rules, &coders);
	return (0);
}
