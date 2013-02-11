#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../../includes/options/options.h"


int main(int argc, char** argv)
{
	get_options_valid(argc, argv);
	return EXIT_SUCCESS;
}