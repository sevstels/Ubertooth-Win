/*
POSIX getopt for Windows
AT&T Public License
*/

#ifdef __GNUC__
#include <getopt.h>
#endif

#ifndef __GNUC__

#ifndef _WIN_GETOPT_H_
#define _WIN_GETOPT_H_

#ifdef __cplusplus
extern "C" {
#endif

extern int opterr;
extern int optind;
extern int optopt;
extern char *optarg;
extern int getopt(int argc, char **argv, char *opts);

#ifdef __cplusplus
}
#endif

#endif  /* _GETOPT_H_ */
#endif  /* __GNUC__ */
