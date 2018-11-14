#include "defs.h"

/************************************************************************/
/*									*/
/* main  --  main program for the f9cc compiler				*/
/*									*/
/************************************************************************/

char	use[]	= "Use is  compiler [-S] [-o output_file] [filename.f9]\n";
char	err[]	= "f9cc compiler error: save the source file for Comer\n";
char	cmd[1024];

int	main(
		int	argc,
		char	*argv[]
	    )
{
	int	retcode;		/* Return code from 'system'	*/
	char	genasm = 0;		/* Generate asm or link?	*/
	char	*ofile = (char *)NULL;	/* Output file name or NULL	*/

	argc--;
	argv++;

	/* Handle arguments, if any are present */

	for (; argc>0 && *argv[0] == '-'; argc--,argv++) {
	    if (strcmp("-S", argv[0]) == 0) {
		if (genasm > 0) {
		    fprintf(stderr,"error: -S option is repeated\n");
		    exit(1);
		}
		genasm = 1;
	    } else if (strcmp("-o",argv[0]) == 0) {
		if (ofile != (char *)NULL) {
		    fprintf(stderr,"error: -o option is repeated\n");
		    exit(1);
		}
		if (argc <= 1) {
		    fprintf(stderr,"error: -o must be followed by a file name\n");
		    exit(1);
		}
		argc--;
		argv++;
		ofile = argv[0];
	    } else {
		fprintf(stderr, "%s", use);
		exit(1);
	    }
	}

	if (argc > 1) {
        	fprintf(stderr, "%s", use);
		exit(1);
        }

        if (argc > 0) {
		if (freopen(argv[0],"r",stdin) == NULL) {
			fprintf(stderr,"Can't open %s\n",argv[0]);
			exit(1);
		}
	}

	if (freopen(".tmpf9.c","w",stdout) == NULL) {
		fprintf(stderr,"Can't reopen stdout\n");
		exit(1);
	}

	/* Run the compiler to produce a C file */

	parse();
	fflush(stdout);

	/* Run gcc to produce an object flie */
	
	sprintf(cmd,"gcc -o %s %s .tmpf9.c 2>.tmpf9err",
		ofile==(char *)NULL?"a.out":ofile,
		genasm>0?"-S":"");
	retcode = system(cmd);
	unlink(".tmpf9.c");

	if (retcode != 0) {
		fprintf(stderr, "%s", err);
		unlink(".tmpf9err");
		exit(1);
	}

	if (system("test -s .tmpf9err") == 0 ) {
		fprintf(stderr, "%s", err);
		unlink(".tmpf9err");
		exit(1);
	}
	unlink(".tmpf9err");
	exit(0);
}


/************************************************************************/
/*									*/
/* errmsg  --  print an error message and quit				*/
/*									*/
/************************************************************************/

void	errmsg(char *msg) {

	fprintf(stderr, "error on line %d at position %d: %s\n\n",
		linenum, linepos, msg);
	exit(1);
}

