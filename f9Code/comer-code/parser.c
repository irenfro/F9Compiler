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
	gettoken();


	if (nexttok.tok_typ != LEXLB) {
		errmsg("expecting a left brace");
	}
	gettoken();

	/* Parse declarations */

	parsedecls();

	/* Parse statements */

	parsestmts();

	/* Finish the program */

	if (nexttok.tok_typ != LEXRB) {
		errmsg("expecting right brace");
	}
	gettoken();
	return;
}


/************************************************************************/
/*									*/
/* parsedecls  --  parse the declarations				*/
/*									*/
/************************************************************************/

void	parsedecls(void) {
	int	symindex;

	/* While declarations exist, parse them */

	while(nexttok.tok_typ == LEXTYPE) {
		gettoken();

		if (nexttok.tok_typ != LEXNAME) {
			errmsg("expecting a variable name");
		}
		symindex = symstore(nexttok.tok_str, SYM_TYPE_INT, 0);
		gettoken();

		while(nexttok.tok_typ == LEXCOMMA) {
			gettoken();

			if (nexttok.tok_typ != LEXNAME) {
				errmsg("expecting a variable name");
			}
			symindex = symstore(nexttok.tok_str, SYM_TYPE_INT, 0);
			gettoken();
		}
		if (nexttok.tok_typ != LEXSEMI) {
			errmsg("expecting a semicolon");
		}
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

	/* While additional statements exist, parse them */

	/* An optimization: normally the code would check for the first	*/
	/*   token type of each possible statement (if, while, function,*/
	/*   identifier).  However, stmts is always followed by a right	*/
	/*   brace, so we can use that to stop the iteration.  However,	*/
	/*   we do *not* call gettoken() to move past the right brace.	*/
	/*   Instead we leave the right brace as the next token when we	*/
	/*   return and let other productions check it.			*/

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
		parseif();
		break;
		
	case	LEXWHILE:
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

	/* Move past the 'if' */
	gettoken();

	if (nexttok.tok_typ != LEXLP) {
		errmsg("expecting left paren after 'if'");
	}
	gettoken();

	parseexpr();
	
	if (nexttok.tok_typ != LEXRP) {
		errmsg("expecting right paren after expression");
	}
	gettoken();

	if (nexttok.tok_typ != LEXLB) {
		errmsg("expecting left brace");
	}
	gettoken();

	parsestmts();
	if (nexttok.tok_typ != LEXRB) {
		errmsg("expecting right brace");
	}
	gettoken();

	/* Check for optional 'else' */

	if (nexttok.tok_typ != LEXELSE) {
		return;
	}
	gettoken();

	if (nexttok.tok_typ != LEXLB) {
		errmsg("expecting left brace");
	}
	gettoken();

	parsestmts();
	if (nexttok.tok_typ != LEXRB) {
		errmsg("expecting right brace");
	}
	gettoken();

	return;
}



/************************************************************************/
/*									*/
/* parsewhile  --  parse a 'while' statement				*/
/*									*/
/************************************************************************/

void	parsewhile(void) {

	/* Move past the 'while' */
	gettoken();

	if (nexttok.tok_typ != LEXLP) {
		errmsg("expecting left paren after 'while'");
	}
	gettoken();

	parseexpr();
	
	if (nexttok.tok_typ != LEXRP) {
		errmsg("expecting right paren after expression");
	}
	gettoken();

	if (nexttok.tok_typ != LEXLB) {
		errmsg("expecting left brace");
	}
	gettoken();

	parsestmts();
	if (nexttok.tok_typ != LEXRB) {
		errmsg("expecting right brace");
	}
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
	gettoken();

	if (nexttok.tok_typ != LEXASNOP) {
		errmsg("expecting an assignment operator");
	}
	/* Record actual assignment operator */
	strcpy(tmptok, nexttok.tok_str);
	gettoken();

	/* Check for call to the read function */
	if (nexttok.tok_typ == LEXREAD) {
		if (strcmp(tmptok, "=") != 0) {
			errmsg("read requires the '=' asignment operator");
		}
		parseread();
	} else {

		/* Normal asignment */

		parseexpr();
		if (nexttok.tok_typ != LEXSEMI) {
			errmsg("expecting a semicolon");
		}
		gettoken();
	}
	return;
}



/************************************************************************/
/*									*/
/* parseread  --  parse a read() function				*/
/*									*/
/************************************************************************/

void	parseread() {

	int	morevars;	/* Non-zero => more variables exist	*/

	/* Move past the 'read' */
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


	if (strcmp(fname, "exit") == 0) {
		gettoken();
		if (nexttok.tok_typ != LEXLP) {
			errmsg("expecting left paren after function call");
		}
		gettoken();
		parseexpr();
		if (nexttok.tok_typ != LEXRP) {
			errmsg("expecting right paren after exit call");
		}
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
			gettoken();
		} else {
			parseexpr();
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

	int	index;		/* Index of variable in symbol table	*/

	switch (nexttok.tok_typ) {
	case LEXLP:
		gettoken();
		parseexpr();
		if (nexttok.tok_typ != LEXRP) {
			errmsg("expecting a right parenthesis");
		}
		gettoken();
		break;

	case LEXNAME:
		index = symlookup(nexttok.tok_str);
		gettoken();
		break;

	case LEXNUM:
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
		int	size,		/* 0 for scalar, array size for	*/
					/*   an array			*/
		int	type		/* SYM_TYPE of the variable	*/	
	)
{
	int	i;			/* Table index			*/

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
