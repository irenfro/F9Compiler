#include "defs.h"

/************************************************************************/
/*									*/
/* main  --  main program for the f9cc compiler				*/
/*									*/
/************************************************************************/

char	use[]	= "Use is  f9cc  [-S | -C] [-o output_file] [filename]\n";
char	err[]	= "f9cc compiler error: save the source file for Comer\n";
char	cmd[1024];

#define	MAXFNAME	512
int	main(
		int	argc,
		char	*argv[]
	    )
{
	int	retcode;		/* Return code from 'system'	*/
	char	genasm = 0;		/* Generate asm?		*/
	char	genc   = 0;		/* Generate C code?		*/
	char	*ofile = (char *)NULL;	/* Output file name or NULL	*/
	char	asmfname[MAXFNAME];	/* Output file name for -S	*/
	char	cfilename[MAXFNAME];	/* Output file name for -C	*/
	int	l;			/* Length of a file name	*/

	argc--;
	argv++;

	/* Handle arguments, if any are present */

	for (; argc>0 && *argv[0] == '-'; argc--,argv++) {

	    if (strcmp("-S", argv[0]) == 0) {
		if ((genasm > 0) || (genc > 0)) {
		    fprintf(stderr,"%s\n", use);
		    exit(1);
		}
		genasm = 1;
	    } else if (strcmp("-C",argv[0]) == 0) {
		if ((genasm > 0) || (genc > 0)) {
		    fprintf(stderr,"%s\n", use);
		    exit(1);
		}
		genc = 1;
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

	/* If output is a C file, mv the tempfile to the actual name */

	if (genc > 0) {

		if (ofile == (char*)NULL) {

			/* Create a file name for the C output */

			if (argc <= 0) { 
			/* There is no input file name */
				strcpy(cfilename, "output_file.c");
			} else {
				/* Remove .f9 and append .c to the input file name */
				l = strlen(argv[0]);
				if (l >= MAXFNAME-4) {
					fprintf(stderr, "input file name too long\n");
					exit(1);
				}
				strcpy(cfilename, argv[0]);
				if (strcmp(&cfilename[l-3], ".f9") == 0) {
					strcpy(&cfilename[l-3], ".c");
				} else {
					strcat(cfilename, ".c");
				}
			}
			ofile = cfilename;
		}
		sprintf(cmd,"mv .tmpf9.c %s", ofile);
		system(cmd);
		exit(0);
	}



	/* Run gcc to produce an object flie */

	if (genasm > 0) {

		/* If no output file name has been specified, generate one */

		if (ofile == (char*)NULL) {

			/* Create a file name for the asm output */

			if (argc <= 0) { 
			/* There is no input file name */
				strcpy(asmfname, "output_file.s");
			} else {
				/* Remove .f9 and append .s to the input file name */
				l = strlen(argv[0]);
				if (l >= MAXFNAME-4) {
					fprintf(stderr, "input file name too long\n");
					exit(1);
				}
				strcpy(asmfname, argv[0]);
				if (strcmp(&asmfname[l-3], ".f9") == 0) {
					strcpy(&asmfname[l-3], ".s");
				} else {
					strcat(asmfname, ".s");
				}
			}
			ofile = asmfname;
		}

		/* Build a command to generate asm output */

		sprintf(cmd,"gcc -S -o %s .tmpf9.c 2>.tmpf9err",
			ofile);
	} else {

		/* Build a command to generate an a.out */

		sprintf(cmd,"gcc    -o %s .tmpf9.c 2>.tmpf9err",
			ofile==(char *)NULL ? "a.out" : ofile);
	}
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

