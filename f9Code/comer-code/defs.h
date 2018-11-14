#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


/********************************************************************************/
/*										*/
/*										*/
/*		  Basic definitions used throughout the compiler		*/
/*										*/
/*										*/
/********************************************************************************/

/* Lexical token types */

#define	LEXUNUSED	0	/* An unused value (should never be returned0	*/
#define	LEXNAME		1	/* Variable name				*/
#define	LEXNUM		2	/* A number					*/
#define	LEXTYPE		3	/* A Type name, such as 'int'			*/
#define	LEXCHAR		4	/* 'char'					*/
#define	LEXIF		5	/* 'if'						*/
#define	LEXELSE		6	/* 'else'					*/
#define	LEXWHILE	7	/* 'while'					*/
#define	LEXPGM		8	/* 'program'					*/
#define	LEXREAD		9	/* 'read'					*/
#define	LEXLP		10	/* Left parenthesis				*/
#define	LEXRP		11	/* Right parenthesis				*/
#define	LEXLB		12	/* Left brace					*/
#define	LEXRB		13	/* Right brace					*/
#define	LEXSEMI		14	/* Semicolon					*/
#define	LEXCOMMA	15	/* Comma					*/
#define	LEXADDOP	16	/* Addition operator ( + or - )			*/
#define	LEXMULOP	17	/* Multiplication operator ( * or / )		*/
#define	LEXCMPOP	18	/* Comparison operator ( < >  <= >= == != )	*/
#define	LEXLOGIC	19	/* Logical connector ( &&  or  || )		*/
#define	LEXASNOP	20	/* Assignment operator ( = += -= *= /=    )	*/
#define	LEXSTR		21	/* Quoted string				*/
#define	LEXFCN		22	/* Function call				*/
#define LEXEOF		23	/* End of file encountered			*/

#define	MAXTOK		1024	/* Maximum size of a token (possibly a string)	*/
#define	MAXVAR		24	/* Maximum variable name length			*/
#define	MAXINT		0x7fffffff  /* Maximum positive integer			*/
#define	MAXARGS		200	/* Maximum arguments to a function		*/

/* Symbol Table Size */

#define	MAXNAMES	256	/* Maxium number of names in the symbol table	*/

/* Globals shared by lexical analyzer and parser */

struct	tok {			/* A token as returned by the lexical analyzer	*/
	int	tok_typ;	/* One of the types defined above		*/
	char	tok_str[MAXTOK];/* The actual string				*/
	int	tok_val;	/* Value of the token is a number		*/
};

extern	struct	tok nexttok;	/* The next input token				*/

extern	int	linenum;	/* The current line number			*/
extern	int	linepos;	/* Position on the current line			*/

#define	NEWLINE	'\n'		/* Newline character				*/
#define	BLANK	' '		/* Blank character				*/
#define	TAB	'\t'		/* tab character				*/
#define	PLUS	'+'		/* Plus character				*/
#define	MINUS	'-'		/* MINUS character				*/
#define	SLASH	'/'		/* Slash character				*/
#define	STAR	'*'		/* Star character				*/

extern	void	gettoken(void);
extern	void	errmsg(char *);
extern	void	parse(void);
extern	void	parsepgm(void);
extern	void	parsestmts(void);
extern	void	parsestmt(void);
extern	void	parsedecls(void);
extern	void	parseif(void);
extern	void	parsewhile(void);
extern	void	parseasn(void);
extern	void	parsefcn(char *);
extern	void	parseexpr(void);
extern	void	parsecomp(void);
extern	void	parserel(void);
extern	void	parseadd(void);
extern	void	parsemul(void);
extern	void	parseterm(void);
extern	void	parseread(void);
extern	void	getch(void);
extern	void	lookup(void);
extern	int	symlookup(char *);
extern	int	symstore(char *, int, int);
extern	void	lexinit(void);
