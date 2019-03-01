/************************************************************************/
/*									*/
/*									*/
/*			Lexical analyzer				*/
/*									*/
/*									*/
/*									*/
/************************************************************************/

#include "defs.h"

int	linenum = 0;		/* Current input line number		*/
int	linepos = 0;		/* Current input position on the line	*/
int	state;			/* Current state in the lexical FSM	*/

int	nextchar = NEWLINE;	/* The next character to process	*/

struct	tok nexttok;		/* The next input token.  Set by lexan	*/
				/*	and used by the parser		*/
int	toklen;			/* Length of the token			*/

/* Declaration of the keyword table */

struct	keyentry {
	char	*keyword;	/* A keyword in the language		*/
	int	lextype;	/* The lexical type for the keyword	*/
};

struct	keyentry ktab[] = {
	{ "int",     LEXTYPE },
	{ "str",     LEXTYPE },
	{ "char",    LEXCHAR },
	{ "if",      LEXIF   },
	{ "else",    LEXELSE },
	{ "while",   LEXWHILE},
	{ "program", LEXPGM  },
	{ "read",    LEXREAD },
	{ "print",   LEXFCN  },
	{ "exit",    LEXFCN  }
};


/* Declarations for the FSM */

#include "fsm-defs"

/* Definition of character range */

#define	CHAR_RANGE	257	/* Range of character values: 0 to 255	*/
				/* 	plus one for "EOF"		*/
#define	EOF_CHAR	256

/* FSM actions */

#define	FSM_ACT_SKIP	0	/* Skip this character (no action)	*/
#define	FSM_ACT_SAVE	1	/* Save the current character		*/
#define	FSM_ACT_APP	2	/* Append the current character to the	*/
				/*	saved characters		*/
#define	FSM_ACT_RET	3	/* Have reached the end of a token, so	*/
				/*	return (convert string to	*/
				/*	integer value if LEXNUM)	*/
#define	FSM_ACT_SRET	4	/* Save and return.  Used with single-	*/
				/*	character token, such as comma	*/
#define	FSM_ACT_ARET	5	/* Append and return,  Used with multi-	*/
				/*	character token, such as +=	*/
#define	FSM_ACT_ERR	6	/* Invalid combination of state and	*/
				/*	input character			*/

/* One entry in the FSM array */

struct	fsmentry {
	int	fsm_nextst;	/* Next state				*/
	int	fsm_action;	/* Action for this transition		*/
	int	fsm_lextyp;	/* For FSM_RET, the token type to return*/
};

struct	fsmentry fsm[STATES][CHAR_RANGE];

/************************************************************************/
/*									*/
/* lexinit  --  initialize the lexical analyzer				*/
/*									*/
/************************************************************************/

void	lexinit(void) {
	int	s, c, k;

	/* Initialize the FSM */

	for (s=0; s<STATES; s++) {
		for (c=0; c<CHAR_RANGE; c++) {
			fsm[s][c].fsm_nextst = 0;
			fsm[s][c].fsm_action = FSM_ACT_ERR;
			fsm[s][c].fsm_lextyp = LEXUNUSED;
		}
	}

#include "fsm-code"


	/* Initialize nextchar */
	getch();
	if (nextchar == EOF_CHAR) {
		fprintf(stderr, "Input file is empty\n");
		exit(1);
	}
	state = FSM_STATE_INIT;
}


/************************************************************************/
/*									*/
/* getch  --  place the next input character in nextchar, mapping EOF	*/
/*		to EOF_CHAR 						*/
/*									*/
/************************************************************************/

void	getch(void) {
	int	ch;

	if (nextchar == NEWLINE) {
		linenum++;
		linepos = 0;
	}
	ch = fgetc(stdin);
	if (ch == EOF) {
		nextchar = EOF_CHAR;
	} else {
		nextchar = ch;
		linepos++;
	}
	return;
}


/************************************************************************/
/*									*/
/* gettoken  --  perform lexical analysis to set nexttok to the next	*/
/*			 token						*/
/*									*/
/************************************************************************/

void	gettoken(void) {

	struct	fsmentry *fsmptr;	/* Ptr to current FSM entry	*/
	int	ch;			/* Current input character	*/
	char	*pch;			/* Walks through token string	*/
	int	dig;			/* Next digit			*/
	unsigned int tmp;		/* Used to accumulate a number	*/

	while (1) {
		ch = nextchar;
		fsmptr = &fsm[state][ch];
		state = fsmptr->fsm_nextst;

		switch (fsmptr->fsm_action) {
		    case FSM_ACT_SKIP:
			if (ch != EOF_CHAR) {
				getch();
			}
			break;

		    case FSM_ACT_SAVE:
			nexttok.tok_str[0] = ch;
			toklen = 1;
			getch();
			break;

		    case FSM_ACT_APP:
			if (toklen >= MAXTOK-1) {
				errmsg("Token is too long");
			}
			nexttok.tok_str[toklen++] = ch;
			getch();
			break;

		    case FSM_ACT_RET:
			nexttok.tok_str[toklen] = '\0';
			nexttok.tok_typ = fsmptr->fsm_lextyp;

			/* Convert numeric values and look up names */

			if (fsmptr->fsm_lextyp == LEXNUM) {
			    /* Convert to numeric value and check */
			    if (nexttok.tok_str[1] == 'x') { /* Hex */
				pch = &nexttok.tok_str[2];
				for (tmp=0; *pch != '\0'; pch++) {
				    if ((*pch>='0')&&(*pch<='9')) {
					dig = *pch-'0';
				    } else if ((*pch>='A')&&(*pch<='F')) {
					dig = 10 + (*pch - 'A');
				    } else {
					dig = 10 + (*pch - 'a');
				    }
				    tmp = (tmp<<4) | dig;
				    if (tmp > MAXINT) {
					errmsg("Invalid hex constant");
				    }
				}
				nexttok.tok_val = tmp;
			    } else { /* Decimal */
				pch = &nexttok.tok_str[0];
				for (tmp=0; *pch != '\0'; pch++) {
				    dig = *pch-'0';
				    tmp = 10*tmp + dig;
				    if (tmp > MAXINT) {
					errmsg("Invalid decimal constant");
				    }
				}
				nexttok.tok_val = tmp;
			    }
			} else if (fsmptr->fsm_lextyp == LEXNAME) {
				lookup();
			}
			return;

		    case FSM_ACT_SRET:
			nexttok.tok_str[0] = nextchar;
			toklen = 1;
			nexttok.tok_str[toklen] = '\0';
			nexttok.tok_typ = fsmptr->fsm_lextyp;
			getch();
			return;

		    case FSM_ACT_ARET:
			if (toklen >= MAXTOK-1) {
				errmsg("Token is too long");
			}
			nexttok.tok_str[toklen++] = ch;
			nexttok.tok_str[toklen] = '\0';
			nexttok.tok_typ = fsmptr->fsm_lextyp;
			getch();
			return;

		    case FSM_ACT_ERR:
			errmsg("Invalid input (not a valid token)");
		}
		
	}
	return;
}

/************************************************************************/
/*									*/
/* lookup  --  lookup the name inthe current token to see if it is a	*/
/*			keyword, and if som], replace the token type	*/
/*									*/
/************************************************************************/

void	lookup(void) {

	int	k;		/* Index in the keyword table		*/
	int	nkeys;		/* Number of keywords in the table	*/

	nkeys = sizeof(ktab)/sizeof(struct keyentry);

	for (k=0; k < nkeys; k++) {
		if (strcmp(ktab[k].keyword, nexttok.tok_str) == 0) {
			/* Match */
			nexttok.tok_typ = ktab[k].lextype;
			break;
		}
	}
	return;
}
