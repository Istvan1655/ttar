/**
	\file 	src/options.c
	\brief 	Allows you to manage options
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>

#include "../../includes/options/options.h"


/*********************************************

		Useful functions 

**********************************************/

/**
	\fn print_option_error(const char c)
	\param c Unknown option
*/

void print_option_error(const char c)
{
	printf("The option '%c' doesn't exist. Please, fix the command line !\n", c);
	exit(EXIT_FAILURE);
}

/*********************************************

		Functions to manage options

**********************************************/


/**
	\fn get_options_valid(int argc, char ** argv)
	\brief Analyze the command line to treat program's options and check their validity
	\param argc Argument counter
	\param argv Argument values
*/

void get_options_valid(int argc, char** argv)
{
	int optch;

	opterr = 0;

	while((optch = getopt(argc, argv, "vh")) != -1)
	{
		switch(optch)
		{
			case 'v' :
				printf("Verbose\n");
			break;
			case 'h' : 
				printf("Help me !\n");
			break;
			case '':
			break;
			default:				
				print_option_error(optopt);
			break;
		}
	}
}