/************************************************************************/
/*									*/
/*									*/
/*				Parser					*/
/*									*/
/*									*/
/*									*/
/************************************************************************/

#include "defs.h"

/* Symbol table variables */

#define	SYM_TYPE_INT	0	/* Variable is of type 'int'		*/
#define	SYM_TYPE_STR	1	/* Variable is of type 'str'		*/

int	symnames = 0;		/* Number of names in the symbol table	*/
struct	symentry {		/* Structure of a symbol table entry	*/
	char	sym_name[MAXTOK+1];	/* The name of a variable	*/
	int	sym_type;		/* The type of the variable	*/
	int	sym_asize;		/* 0=>scalar, >0=>array size	*/
};
struct	symentry symtab[MAXNAMES];	/* The symbol table		*/

/************************************************************************/
/*									*/
/* parse  --  initialize and then parse a program			*/
/*									*/
/************************************************************************/

void	parse(void) {

	lexinit();	/* Initialize the lexical analyzer */
	gettoken();	/* Get the first token */
	parsepgm();	/* Parse a program */

	/* Verify that no futher tokens occur after the program */

	if (nexttok.tok_typ != LEXEOF) {
		errmsg("additional token found after the program");
	}
	return;
}


/************************************************************************/
/*									*/
/* parsepgm  --  parse a complete program				*/
/*									*/
/************************************************************************/

void	parsepgm(void) {

	/* Check the program header */

	if (nexttok.tok_typ != LEXPGM) {
		errmsg("expecting 'program'");
	}
/*CODE*/printf("#include <stdio.h>\n");
/*CODE*/printf("#include <stdlib.h>\n");
	gettoken();


	if (nexttok.tok_typ != LEXLB) {
		errmsg("expecting a left brace");
	}
/*CODE*/printf("void	main(void) {\n");
	gettoken();

	/* Parse declarations */

	parsedecls();

	/* Parse statements */

	parsestmts();

	/* Finish the program */

	if (nexttok.tok_typ != LEXRB) {
		errmsg("expecting right brace");
	}
/*CODE*/printf("exit(0); }\n");
	gettoken();
	return;
}


/************************************************************************/
/*									*/
/* checkarry  --  check for an array declaration			*/
/*									*/
/************************************************************************/

void	checkarray(int symindex) {

	if (nexttok.tok_typ == LEXLBK) {
/*CODE*/	printf("[");
		gettoken();
		if (nexttok.tok_typ != LEXNUM) {
			errmsg("expecting an array size");
		}
		if (nexttok.tok_val <= 0) {
			errmsg("invalid array size");
		}
		symtab[symindex].sym_asize = nexttok.tok_val;
/*CODE*/	printf("%s", nexttok.tok_str);
		gettoken();
		if (nexttok.tok_typ != LEXRBK) {
			errmsg("expecting a rght bracket");
		}
/*CODE*/	printf("]");
		gettoken();
	}
	return;
}


/************************************************************************/
/*									*/
/* parsedecls  --  parse the declarations				*/
/*									*/
/************************************************************************/

void	parsedecls(void) {
	int	symindex;	/* Index in symbol table		*/
	int	typ;		/* Type for current declaration		*/

	/* While declarations exist, parse them */

	while(nexttok.tok_typ == LEXTYPE) {
		if (strcmp(nexttok.tok_str, "int") == 0) {
/*CODE*/		printf("	int	");
			typ = SYM_TYPE_INT;
		} else {
/*CODE*/		printf("	char	");
			typ = SYM_TYPE_STR;
		}
		gettoken();

		if (nexttok.tok_typ != LEXNAME) {
			errmsg("expecting a variable name");
		}
		if (typ == SYM_TYPE_STR) {
/*CODE*/		printf("*");
		}
/*CODE*/	printf("%s", nexttok.tok_str);
		symindex = symstore(nexttok.tok_str, typ, 0);
		gettoken();
		checkarray(symindex);
		while(nexttok.tok_typ == LEXCOMMA) {
			gettoken();
			if (nexttok.tok_typ != LEXNAME) {
				errmsg("expecting a variable name");
			}
/*CODE*/		printf(",%s%s", typ==SYM_TYPE_STR?" *":" ",nexttok.tok_str);
			symindex = symstore(nexttok.tok_str, typ, 0);
			gettoken();
			checkarray(symindex);
		}

		if (nexttok.tok_typ != LEXSEMI) {
			errmsg("expecting a semicolon");
		}
/*CODE*/	printf(";\n");
		gettoken();
	}
	return;
}


/************************************************************************/
/*									*/
/* parsestmts  --  parse a sequence of one or more statements		*/
/*									*/
/************************************************************************/

void	parsestmts(void) {

	/* Parse one statement */

	parsestmt();

	/* An optimization: normally the code would check for the first	*/
	/*   token type of each possible statement (if, while, function,*/
	/*   identifier).  However, stmts is always followed by a right	*/
	/*   brace, so we can use that to stop the iteration.  However,	*/
	/*   we do *not* call gettoken() to move past the right brace.	*/
	/*   Instead we leave the right brace as the next token when we	*/
	/*   return and let other productions check it.			*/

	/* While additional statements exist, parse them */

	while (nexttok.tok_typ != LEXRB) {
		parsestmt();
	}
	return;
}


/************************************************************************/
/*									*/
/* parsestmt  --  parse one statement					*/
/*									*/
/************************************************************************/

void	parsestmt(void) {
	char	fname[MAXTOK];

	switch (nexttok.tok_typ) {

	case	LEXIF:
		gettoken();
		parseif();
		break;
		
	case	LEXWHILE:
		gettoken();
		parsewhile();
		break;
		
	case	LEXNAME:
		parseasn();
		break;

	case	LEXFCN:
		strcpy(fname, nexttok.tok_str);
		parsefcn(fname);
		break;

	case	LEXSEMI:
		errmsg("did not expect a semicolon (Reminder: f9 is not C!)");

	default:
		errmsg("expected a statement");
	}

	return;
}


/************************************************************************/
/*									*/
/* parseif  --  parse an 'if' statement					*/
/*									*/
/************************************************************************/

void	parseif(void) {

	if (nexttok.tok_typ != LEXLP) {
		errmsg("expecting left paren after 'if'");
	}
/*CODE*/printf("if ( ");
	gettoken();

	parseexpr();
	
	if (nexttok.tok_typ != LEXRP) {
		errmsg("expecting right paren after expression");
	}
/*CODE*/printf(" )");
	gettoken();

	if (nexttok.tok_typ != LEXLB) {
		errmsg("expecting left brace");
	}
/*CODE*/printf(" {\n");
	gettoken();

	parsestmts();
	if (nexttok.tok_typ != LEXRB) {
		errmsg("expecting right brace");
	}
/*CODE*/printf("\n}");
	gettoken();

	/* Check for optional 'else' */

	if (nexttok.tok_typ != LEXELSE) {
/*CODE*/printf("\n");
		return;
	}
/*CODE*/printf(" else");
	gettoken();

	if (nexttok.tok_typ != LEXLB) {
		errmsg("expecting left brace");
	}
/*CODE*/printf(" {\n");
	gettoken();

	parsestmts();
	if (nexttok.tok_typ != LEXRB) {
		errmsg("expecting right brace");
	}
/*CODE*/printf(" }\n");
	gettoken();

	return;
}



/************************************************************************/
/*									*/
/* parsewhile  --  parse an 'while' statement				*/
/*									*/
/************************************************************************/

void	parsewhile(void) {

	if (nexttok.tok_typ != LEXLP) {
		errmsg("expecting left paren after 'while'");
	}
/*CODE*/printf("while ( ");
	gettoken();

	parseexpr();
	
	if (nexttok.tok_typ != LEXRP) {
		errmsg("expecting right paren after expression");
	}
/*CODE*/printf(" )");
	gettoken();

	if (nexttok.tok_typ != LEXLB) {
		errmsg("expecting left brace");
	}
/*CODE*/printf(" {\n");
	gettoken();

	parsestmts();
	if (nexttok.tok_typ != LEXRB) {
		errmsg("expecting right brace");
	}
/*CODE*/printf("\n}\n");
	gettoken();

	return;
}



/************************************************************************/
/*									*/
/* parseasn  --  parse an assignment statement				*/
/*									*/
/************************************************************************/

void	parseasn(void) {
	char	tmptok[MAXTOK];
	int	symindex;

	symindex = symlookup(nexttok.tok_str);
/*CODE*/printf("    %s ", nexttok.tok_str);
	gettoken();

	if (nexttok.tok_typ == LEXLBK) { /* Array reference */
/*CODE*/	printf("[");
		if (symtab[symindex].sym_asize <= 0) {
			errmsg("attempt to index a non-array variable");
		}
		gettoken();
		parseexpr();
		if (nexttok.tok_typ != LEXRBK) {
			errmsg("expecting a right bracket");
		}
/*CODE*/	printf("]");
		gettoken();
	}

	if (nexttok.tok_typ != LEXASNOP) {
		errmsg("expecting an assignment operator");
	}
	strcpy(tmptok, nexttok.tok_str);
/*CODE*/printf("%s ", nexttok.tok_str);
	gettoken();

	/* Check for call to the read function */
	if (nexttok.tok_typ == LEXREAD) {
		if (strcmp(tmptok, "=") != 0) {
			errmsg("read requires asignment operator =");
		}
		parseread();
	} else if (nexttok.tok_typ == LEXSTR) {
		if (symtab[symindex].sym_type != SYM_TYPE_STR) {
			errmsg("attempt to assign a string to a non-string variable");
		}
		if (strcmp(tmptok, "=") != 0) {
			errmsg("string asignment requires operator =");
		}
/*CODE*/	printf("%s", nexttok.tok_str);
		gettoken();
		if (nexttok.tok_typ != LEXSEMI) {
			errmsg("expecting a semicolon");
		}
/*CODE*/	printf(";\n");
		gettoken();
	} else {

		/* Normal, arithmetic asignment */
		if (symtab[symindex].sym_type == SYM_TYPE_STR) {
			errmsg("expecting a string constant");
		}
		parseexpr();
		if (nexttok.tok_typ != LEXSEMI) {
			errmsg("expecting a semicolon");
		}
/*CODE*/	printf(";\n");
		gettoken();
	}
	return;
}


/*CODE*/int	tmpoff = 0;	/* Offset for tmp variable names */

/************************************************************************/
/*									*/
/* parseread  --  parse a read() function				*/
/*									*/
/************************************************************************/

void	parseread() {

	int	morevars;	/* Non-zero => more variables exist	*/
/*CODE*/int	varindex;	/* Index returned by symlookup		*/
/*CODE*/int	argindex[MAXARGS]; /* Symbol table index of each var.	*/
/*CODE*/int	acnt;		/* Count of variables			*/
/*CODE*/int	ctr;		/* Iterates from 0 to ectr-1		*/

	gettoken();
	if (nexttok.tok_typ != LEXLP) {
		errmsg("expecting left paren after 'read'");
	}
	gettoken();

	/* See if the argument list is empty */

	if (nexttok.tok_typ == LEXRP) {
		errmsg("read requires at least one variable name");
	}

	morevars = 1;
	acnt = 0;
	while (morevars > 0) {

		/* Pick up the next variable name */

		if (acnt >= MAXARGS - 1) {
			errmsg("too many arguments to 'read'");
		}
		if (nexttok.tok_typ != LEXNAME) {
			errmsg("read expects a list of variable names");
		}
		varindex = symlookup(nexttok.tok_str);
/*CODE*/	argindex[acnt] = varindex;
		acnt++;
		gettoken();
		if (nexttok.tok_typ == LEXCOMMA) {
			gettoken();
		} else {
			morevars = 0;
		}
	}
	if (nexttok.tok_typ != LEXRP) {
		errmsg("expecting a right parenthesis at end of 'read'");
	}
/*CODE*/printf("scanf(\"");
/*CODE*/for(ctr=0;ctr<acnt;ctr++) {
/*CODE*/	printf("%%d");
/*CODE*/}
/*CODE*/printf("\"");
/*CODE*/for(ctr=0;ctr<acnt;ctr++) {
/*CODE*/	printf(",&%s", symtab[argindex[ctr]].sym_name);
/*CODE*/}
/*CODE*/printf(");");
/*CODE*/tmpoff += acnt;
	gettoken();

	return;
}



/************************************************************************/
/*									*/
/* parsefcn  --  parse a builtin function				*/
/*									*/
/************************************************************************/

void	parsefcn(char *fname) {	/* Argument is the name of the function */

	int	moreargs;	/* Non-zero => more arguments exist	*/
/*CODE*/int	argstr[MAXARGS];/* Non-zero if ith arg is a string	*/
/*CODE*/int	acnt;		/* Count of arguments			*/
/*CODE*/int	ctr;		/* Iterates from 0 to ectr-1		*/
/*CODE*/char	*pch;		/* Walks through a string during copy	*/
/*CODE*/int	symindex;	/* Symbol table index for variable	*/


	if (strcmp(fname, "exit") == 0) {
		gettoken();
		if (nexttok.tok_typ != LEXLP) {
			errmsg("expecting left paren after function call");
		}
/*CODE*/	printf("exit(");
		gettoken();
		parseexpr();
		if (nexttok.tok_typ != LEXRP) {
			errmsg("expecting right paren after exit call");
		}
/*CODE*/	printf(");");
		gettoken();
		return;
	}

	/* print function */
	gettoken();
	if (nexttok.tok_typ != LEXLP) {
		errmsg("expecting left paren after function call");
	}
	gettoken();

	/* See if the argument list is empty */

	if (nexttok.tok_typ == LEXRP) {
		errmsg("print requires at least one argument");
	}

	moreargs = 1;
	acnt = 0;
	while (moreargs > 0) {

		/* Pick up the next argument */

		acnt++;
		if (acnt >= MAXARGS) {
			errmsg("too many arguments to a function");
		}

		if (nexttok.tok_typ == LEXSTR) {
/*CODE*/		argstr[acnt] = 1;
/*CODE*/		printf("char *tmps%d = ", acnt+tmpoff);
/*CODE*/		for (pch = nexttok.tok_str; *pch != '\0'; pch++) {
/*CODE*/			printf("%c", *pch);
/*CODE*/		}
/*CODE*/		printf(";\n");
			gettoken();

		} else {
			if ( (nexttok.tok_typ == LEXNAME) &&
			     ((symindex=symlookup(nexttok.tok_str)) != -1) &&
			     (symtab[symindex].sym_type == SYM_TYPE_STR) ) {
/*CODE*/			argstr[acnt] = 1;
/*CODE*/			printf("char *tmps%d = %s", acnt+tmpoff,
/*CODE*/				nexttok.tok_str);
				gettoken();
				if (nexttok.tok_typ == LEXLBK) {
					/* array index */
/*CODE*/				printf("[");
					gettoken();
					parseexpr();
					if (nexttok.tok_typ != LEXRBK) {
						errmsg("expecting a right bracket");
					}
/*CODE*/				printf("]");
					gettoken();
				}
/*CODE*/			printf(";\n");
/*CODE*/		} else {
/*CODE*/			argstr[acnt] = 0;
/*CODE*/			printf("int tmpi%d = ", acnt+tmpoff);
				parseexpr();
/*CODE*/			printf(";\n");
			}
		}
		if (nexttok.tok_typ == LEXCOMMA) {
			gettoken();
		} else {
			moreargs = 0;
		}
	}
	if (nexttok.tok_typ != LEXRP) {
		errmsg("expecting a right parenthesis");
	}
/*CODE*/printf("printf(\"");
/*CODE*/for(ctr=1;ctr<=acnt;ctr++) {
/*CODE*/	if (argstr[ctr] > 0) {
/*CODE*/		printf("%%s");
/*CODE*/	} else {
/*CODE*/		printf("%%d");
/*CODE*/	}
/*CODE*/}
/*CODE*/printf("\"");
/*CODE*/for(ctr=1;ctr<=acnt;ctr++) {
/*CODE*/	if (argstr[ctr] > 0) {
/*CODE*/		printf(",tmps%d", ctr+tmpoff);
/*CODE*/	} else {
/*CODE*/		printf(",tmpi%d", ctr+tmpoff);
/*CODE*/	}
/*CODE*/}
/*CODE*/printf(");\n");
/*CODE*/tmpoff += acnt;
	gettoken();

	return;
}


/************************************************************************/
/*									*/
/* parseexpr  --  parse an expression with logical connectors		*/
/*									*/
/************************************************************************/

void	parseexpr(void) {
	parsecomp();
	while (nexttok.tok_typ == LEXLOGIC) {
/*CODE*/	printf(" %s ",nexttok.tok_str);
		gettoken();
		parsecomp();
	}
	return;
}


/************************************************************************/
/*									*/
/* parsecomp  --  parse an a comparison operator in an expression	*/
/*									*/
/************************************************************************/

void	parsecomp(void) {
	parseaddn();
	while (nexttok.tok_typ == LEXCMPOP) {
/*CODE*/	printf(" %s ",nexttok.tok_str);
		gettoken();
		parseaddn();
	}
	return;
}


/************************************************************************/
/*									*/
/* parseaddn  --  parse an addition operator in an expression		*/
/*									*/
/************************************************************************/

void	parseaddn(void) {

	parsemuln();
	while (nexttok.tok_typ == LEXADDOP) {
/*CODE*/	printf(" %s ",nexttok.tok_str);
		gettoken();
		parsemuln();
	}
	return;
}

/************************************************************************/
/*									*/
/* parsemuln  --  parse a multiplication operator in an expression	*/
/*									*/
/************************************************************************/

void	parsemuln(void) {

	parseterm();
	while (nexttok.tok_typ == LEXMULOP) {
/*CODE*/	printf(" %s ",nexttok.tok_str);
		gettoken();
		parseterm();
	}
	return;
}

/************************************************************************/
/*									*/
/* parseterm  --  parse a term in an expression				*/
/*									*/
/************************************************************************/

void	parseterm(void) {

	int	symindex;		/* Index of variable in symbol table	*/

	switch (nexttok.tok_typ) {
	case LEXLP:
/*CODE*/	printf(" ( ");
		gettoken();
		parseexpr();
		if (nexttok.tok_typ != LEXRP) {
			errmsg("expecting a right parenthesis");
		}
/*CODE*/	printf(" )");
		gettoken();
		break;

	case LEXNAME:
		symindex = symlookup(nexttok.tok_str);
/*CODE*/	printf("%s", nexttok.tok_str);
		gettoken();
		if (nexttok.tok_typ == LEXLBK) {
			if (symtab[symindex].sym_asize == 0) {
				errmsg("array reference to non-array");
			}
/*CODE*/		printf("[");
			gettoken();
			parseexpr();
			if (nexttok.tok_typ != LEXRBK) {
				errmsg("expecting a right bracket");
			}
/*CODE*/		printf("]");
			gettoken();
		}
		break;

	case LEXNUM:
/*CODE*/	printf("%s", nexttok.tok_str);
		gettoken();
		break;
	}
	return;
}


/************************************************************************/
/*									*/
/* symlookup  --  look up a name in the symbol table			*/
/*									*/
/************************************************************************/

int	symlookup(
		char	*name		/* Variable name to look up	*/
	)
{
	int	i;			/* Table index			*/
	char	errstr[MAXTOK+40];	/* String for error message	*/

	/* Find the name in the table */

	for (i=0; i<symnames; i++) {
		if (strcmp(name, symtab[i].sym_name) == 0) {
			return i;
		}
	}
	sprintf(errstr, "variable name not declared: %s", name);
	errmsg(errstr);
	return -1;
}


/***********************************************************************/
/*									*/
/* symstore  --  store a new variable name in the symbol table		*/
/*									*/
/************************************************************************/

int	symstore(
		char	*name,		/* Variable name to use		*/
		int	type,		/* SYM_TYPE of the variable	*/
		int	size		/* 0 for scalar, array size for	*/
					/*   an array			*/
	)
{
	int	i;			/* Table index			*/

//fprintf(stderr,"Storing %s type=%d\n", name, type);
	/* Verify the name is not already present */

	for (i=0; i<symnames; i++) {
		if (strcmp(name, symtab[i].sym_name) == 0) {
			errmsg("duplicate variable declaration");
		}
	}
	symnames++;
	if (symnames >= MAXNAMES) {
		errmsg("too many variables declared");
	}
	strcpy(symtab[i].sym_name, name);
	symtab[i].sym_type = type;
	symtab[i].sym_asize = size;
	return i;
}
