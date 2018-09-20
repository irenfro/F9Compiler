#include "defs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Lexical Analyzer Prototypes */
void    lexinit(void);
void    getch(void);
void    gettoken(void);
void    lookup(void);
/* End                          */


int     linenum = 1;        /*  Current input line number           */
int     linepos = 0;        /*  Current input position on the line  */
int     state;              /*  Current state in the lexical FSM    */

int     nextchar = NEWLINE; /*  The next character to process       */

token   nexttok;            /*  The next input token. Set by lexan  */
                            /*      and used by the parser          */
int     toklen;             /*  Length of the token                 */
FILE*   file;               /*  File to read from                   */

/* Declaration of the keyword table */

typedef struct {
        char    *keyword;   /*  A keyword in the language           */
        int     lextype;    /*  The lexical type for the keyword    */
} keyentry;

keyentry ktab[] = {
        { "int",     LEXTYPE },
        { "char",    LEXCHAR },
        { "if",      LEXIF   },
        { "else",    LEXELSE },
        { "while",   LEXWHILE},
        { "program", LEXPGM  },
        { "read",    LEXREAD },
        { "print",   LEXFCN  },
        { "exit",    LEXFCN  }
};

#define KTABSIZE    9

/* Declaration for the FSM */

#include "fsm-defs"

/* Definition of the character range */

#define CHAR_RANGE      257         /* Range of character values 0 to 255   */
                                    /*      plus one for "EOF"              */
#define EOF_CHAR        256

/* FSM actions */

#define FSM_ACT_SKIP    0           /* Skip this character (no action)      */
#define FSM_ACT_SAVE    1           /* Save the current character           */
#define FSM_ACT_APP     2           /* Append the current character to the  */
                                    /*      save characters                 */
#define FSM_ACT_RET     3           /* Have reached the end of a token, so  */
                                    /*      return (convert string to       */
                                    /*      integer value if LEXNUM)        */
#define FSM_ACT_SRET    4           /* Save and return. Used with single-   */
                                    /*      character token, such as comma  */
#define FSM_ACT_ARET    5           /* Append and return, Used with multi-  */
                                    /*      character token, such as +=     */
#define FSM_ACT_ERR     6           /* Invalid combination of state and     */
                                    /*      input character                 */

/* One entry in the FSM array */

typedef struct {
        int     fsm_nextst;         /* Next state                           */
        int     fsm_action;         /* Action for this transition           */
        int     fsm_lextyp;         /* For FSM_RET, the token type to return*/
} fsmentry;

fsmentry fsm[STATES][CHAR_RANGE];

/****************************************************************************/
/*                                                                          */
/* lexinit -- initialize the lexical analyzer                               */
/*                                                                          */
/****************************************************************************/

void    lexinit(void) {
        int     s, c;

        /* Initialize the FSM */

        for(s = 0; s < STATES; s++) {
            for(c = 0; c < CHAR_RANGE; c++) {
                fsm[s][c].fsm_nextst = 0;
                fsm[s][c].fsm_action = FSM_ACT_ERR;
                fsm[s][c].fsm_lextyp = LEXUNUSED;
            }
        }
#include "fsm-code"

        /* Initialize nextchar */
        getch();
        if(nextchar == EOF_CHAR) {
            fprintf(stderr, "Input file is empty\n");
            exit(1);
        }
        state = FSM_STATE_INIT;
}

/****************************************************************************/
/*                                                                          */
/* getch -- place the next input character in nextchar, mapping EOF         */
/*            to EOF_CHAR                                                   */
/*                                                                          */
/****************************************************************************/

void    getch(void) {
        if(!file) {
            fprintf(stderr, "No opened file");
            exit(1);
        }
        nextchar = getc(file);
        if(nextchar == -1) {
            nextchar = EOF_CHAR;
        } else if(nextchar == NEWLINE) {
            linenum++;
            linepos = 0;
        } else {
            linepos++;
        }
}

/****************************************************************************/
/*                                                                          */
/* errmsg -- print out the error message along with line and character      */
/*                 number                                                   */
/****************************************************************************/

void    errmsg(char* msg) {
        fprintf(stderr, "%s\n", msg);
        fprintf(stderr, "Error parsing input. LineNum: %d characterNum: %d\n", linenum, linepos);
        fprintf(stderr, "tok: %s\n", nexttok.tok_str);
        exit(1);
}

/****************************************************************************/
/*                                                                          */
/* gettoken -- perform lexical analysis to set nexttok to the next          */
/*                     token                                                */
/*                                                                          */
/****************************************************************************/

void    gettoken(void) {
        

        fsmentry    *fsmptr;            /* Ptr to the current FSM entry     */
        int         ch;                 /* Current input character          */
        char        *pch;               /* Walks through token string       */
        int         dig;                /* Next digit                       */
        unsigned int tmp;               /* Used to accumulate a number      */

        nexttok.tok_typ = -1;
        nexttok.tok_val = -1;
        memset(nexttok.tok_str, '\0', MAXTOK);
        while(1) {
                ch = nextchar;
                fsmptr = &fsm[state][ch];
                state = fsmptr->fsm_nextst;

                switch(fsmptr->fsm_action) {
                    case FSM_ACT_SKIP:
                        if(ch != EOF_CHAR) {
                            getch();
                        }
                        break;

                    case FSM_ACT_SAVE:
                        nexttok.tok_str[0] = ch;
                        toklen = 1;
                        getch();
                        break;

                    case FSM_ACT_APP:
                        if(toklen >= MAXTOK - 1) {
                            errmsg("Token is too long");
                        }
                        nexttok.tok_str[toklen++] = ch;
                        getch();
                        break;

                    case FSM_ACT_RET:
                        nexttok.tok_str[toklen] = '\0';
                        nexttok.tok_typ = fsmptr->fsm_lextyp;

                        /* Convert numeric values and look up names */

                        if(fsmptr->fsm_lextyp == LEXNUM) {
                            /* Convert to numeric value and check */
                            if(nexttok.tok_str[1] == 'x') { /* Hex */
                                pch = &nexttok.tok_str[2];
                                for(tmp = 0; *pch != '\0'; pch++) {
                                    if((*pch >= '0') && (*pch <= '9')) {
                                        dig = *pch - '0';
                                    } else if((*pch >= 'A') && (*pch <= 'F')) {
                                        dig = 10 + (*pch - 'A');
                                    } else {
                                        dig = 10 + (*pch - 'a');
                                    }
                                    tmp = (tmp << 4) | dig;
                                    if(tmp > MAXINT) {
                                        errmsg("Invalid hex constant");
                                    } 
                                }
                                nexttok.tok_val = tmp;
                            } else { /* Decimal */
                                pch = &nexttok.tok_str[0];
                                for(tmp = 0; *pch != '\0'; pch++) {
                                    dig = *pch - '0';
                                    tmp = 10 * tmp + dig;
                                    if(tmp > MAXINT) {
                                        errmsg("Invalid decimal constant");
                                    }
                                }
                                nexttok.tok_val = tmp;
                            }
                        } else if(fsmptr->fsm_lextyp == LEXNAME) {
                            lookup();
                        }
                        return;

                    case FSM_ACT_SRET:
                        nexttok.tok_str[0] = nextchar;
                        toklen = 1;
                        nexttok.tok_str[0] = '\0';
                        nexttok.tok_typ = fsmptr->fsm_lextyp;
                        getch();
                        return;

                    case FSM_ACT_ARET:
                        if(toklen >= MAXTOK - 1) {
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

/****************************************************************************/
/*                                                                          */
/* lookup -- check to see if the current string is a keyword                */
/*                                                                          */
/****************************************************************************/

void    lookup(void) {
    for(int i = 0; i < KTABSIZE; i++) {
        if(!strcmp(nexttok.tok_str, ktab[i].keyword)) {
            nexttok.tok_typ = ktab[i].lextype;
        }
    }
}

int     main(int argc, char **argv) {
    if(argc != 2) {
        printf("Usage: ./lex [file]");
        exit(1);
    }
    file = fopen(argv[1], "r");
    lexinit();
    gettoken();
    while(nexttok.tok_typ != LEXEOF) {
        printf("tok:\t\t%s\t\t\t%d\n", nexttok.tok_str, nexttok.tok_typ);
        gettoken();
    }
    printf("Got EOF\n");
    return 0;
}
