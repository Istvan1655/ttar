/**
	\file 	src/options.c
	\brief 	Allows you to manage options
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include "../../includes/options/options.h"
#include "../../includes/options/usage.h"

/*********************************************

		Useful functions 

**********************************************/

/**
	\fn print_option_error(const char c)
	\param c Unknown option
*/

void
print_option_error (const char c)
{
  printf ("The option '%c' doesn't exist. Please, fix the command line !\n",
	  c);
  exit (EXIT_FAILURE);
}

// path to the option help file
static const char * opt_help_file = "opt_help/";

/**
   \fn print option file help on stdout
   \param the option name
 */
void
display_opt_help (char * opt)
{
  //construct complete path to the option help file
  char opt_file_path[50];
  opt_file_path[0] = '\0';
  strcat(opt_file_path, opt_help_file);
  strcat(opt_file_path, opt);
  FILE * f_opt = fopen(opt_file_path,"r");

  //check existence of wanted file, printing it then exiting programm
  if (f_opt == NULL)
    printf("prout\n");
  else
    {
      int current_char;
      while ((current_char = fgetc(f_opt)) != EOF)
	printf("%c",(char)(current_char));
    }
  printf("\n");
  usage(EXIT_FAILURE);
}

/**
   \fn check if there is no collision with user's option list.
   \param the option preemptive set
 */
void
check_opt_integrity (uint32 opts)
{
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

void
get_options_valid (int argc, char **argv)
{
  /*int optch;

  opterr = 0;

  while ((optch = getopt (argc, argv, "vh")) != -1)
    {
      switch (optch)
	{
	case 'v':
	  printf ("Verbose\n");
	  break;
	case 'h':
	  printf ("Help me !\n");
	  break;
	case '':
	  break;
	default:
	  print_option_error (optopt);
	  break;
	}
	}*/
  if (argc < 2)
    usage(EXIT_FAILURE);
  
  static struct option long_options[] = {
    {"help",     2, 0, 'h'},
    {"verbose",  0, 0, 'v'},
    {"create",   1, 0, 'c'},
    {"add",      1, 0, 'a'},
    {"delete",   1, 0, 'd'},
    {"update",   0, 0, 'u'},
    {"protocol", 1, 0, 'p'},
    {"listing",  1, 0, 'l'},
    {0, 0, 0, 0}
  };

  int c;
  const char * opt_mask = "h::vc:a:d:u:p:l:";
  while ((c = getopt_long(argc, argv, opt_mask, long_options, NULL)) != -1)
    {
      switch (c)
	{
	  //help option, the programm will exit after display it.
	case 'h':
	  // if there is no argument printing general help.
	  if (optarg == NULL)
	    usage(EXIT_SUCCESS);
	  // else construct the path file tto print it on stdout.
	  else
	    {
	      if (optarg[0] == '=')
		optarg += 1;
	      display_opt_help(optarg);
	    }
	  break;

	case 'v':
	  verbose = true;
	  printf("verbose activé.\n");
	  break;

	case 'c':
	  printf("création demandée.\n");
	  break;

	case 'a':
	  break;

	case 'd':
	  break;

	case 'u':
	  break;

	case 'p':
	  break;

	case 'l':
	  break;
	default:
	  printf("option non reconnue.\n");
	  usage(EXIT_FAILURE);
	  break;
	}
    }
}
