#include "codexion.h"

static int	parse_config(int ac, char **av, t_rules *config)
{
	(void)ac;

	config->number_of_coders = atoi(av[1]);
	config->time_to_burnout = atoi(av[2]);
	config->time_to_compile = atoi(av[3]);
	config->time_to_debug = atoi(av[4]);
	config->time_to_refactor = atoi(av[5]);
	config->number_of_compiles_required = atoi(av[6]);
	config->dongle_cooldown = atoi(av[7]);
	if (config->number_of_coders < 1 || config->time_to_burnout < 1
		|| config->time_to_compile < 1 || config->time_to_debug < 1
		|| config->time_to_refactor < 1
		|| config->number_of_compiles_required < 1
		|| config->dongle_cooldown < 0)
		return (1);
	return (0);
}

static int	is_digit_string(char *string)
{
	int	i;
	if (!string || !string[0])
		return (1);
	i = 0;
	while (string[i])
	{
		if (string[i] < '0'|| string[i] > '9')
			return (1);
		i++;
	}
	if (i > 9)
		return (1);
	return (0);
}

static int	parse_scheduler(char *str, t_rules *config)
{
	if (strcmp(str, "fifo") == 0)
	{
		config->scheduler = POLICY_FIFO;
		return (0);
	}
	if (strcmp(str, "edf") == 0)
	{
		config->scheduler = POLICY_EDF;
		return (0);
	}
	return (1);
}

int parser(int ac, char **av, t_rules *config)
{
	int		i = 1;
	(void)ac;

	while (i < 8)
	{
		if (is_digit_string(av[i]) == 1)
			return (1);
		i++;
	}

	if (parse_config(ac, av, config) != 0)
		return (1);

	if (parse_scheduler(av[8], config) != 0)
		return (1);
	return (0);
}
