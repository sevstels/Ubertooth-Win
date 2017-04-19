/*
POSIX getopt for Windows
AT&T Public License
Code given out at the 1985 UNIFORUM conference in Dallas.  
*/

#ifndef __GNUC__

#include "getopt.h"
#include <stdio.h>
#include <string.h>
#include "console_color.h"

#define NULL  0
#define EOF	(-1)

char *optarg;

static int opterr = 1;
static int optind = 1;
static int optopt;
static int argc;
static char **argv, *opts;

void ERR(const char *s, char c)
{
  char errbuf[]= {0,0,0};
  errbuf[0] = c; errbuf[1] = '\n';
  
  SetColor(RED,1,BLACK,0);
  printf("\n%s%s\n", s, errbuf);
  SetColorBack();
}

int getopt(int argc, char **argv, char *opts)
{
	static int sp = 1;
	register int c;
	register char *cp;

	if(sp == 1)
		if(optind >= argc ||
		   argv[optind][0] != '-' || argv[optind][1] == '\0')
			return(EOF);
		else if(strcmp(argv[optind], "--") == NULL) {
			optind++;
			return(EOF);
		}
	optopt = c = argv[optind][sp];
	if(c == ':' || (cp=strchr(opts, c)) == NULL) {
		ERR("Error, illegal option -", c);
		if(argv[optind][++sp] == '\0') {
			optind++;
			sp = 1;
		}
		return('?');
	}
	if(*++cp == ':') {
		if(argv[optind][sp+1] != '\0')
			optarg = &argv[optind++][sp+1];
		else if(++optind >= argc) {
			ERR("Error, option requires an argument -", c);
			sp = 1;
			return('?');
		} else
			optarg = argv[optind++];
		sp = 1;
	} else {
		if(argv[optind][++sp] == '\0') {
			sp = 1;
			optind++;
		}
		optarg = NULL;
	}
	return(c);
}

#endif  /* __GNUC__ */
