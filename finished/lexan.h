#include "defs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* LEXER     */

/* Lexical Analyzer Prototypes */
void    lexinit(void);
void    getch(void);
void    gettoken(void);
void    lookup(void);
void    errmsg(char*);
/* End                          */

/* Lexer EXTERNS    */
extern FILE*    file;
/* END              */

/* LEXER DEFINES */
int     linenum = 1;        /*  Current input line number           */
int     linepos = 0;        /*  Current input position on the line  */
int     state;              /*  Current state in the lexical FSM    */

int     nextchar = NEWLINE; /*  The next character to process       */

token   nexttok;            /*  The next input token. Set by lexan  */
                            /*      and used by the parser          */
int     toklen;             /*  Length of the token                 */


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
/* END */

/* LEXER STRUCTS DEFINES */
/* Declaration of the keyword table */

typedef struct {
        char    *keyword;   /*  A keyword in the language           */
        int     lextype;    /*  The lexical type for the keyword    */
} keyentry;

keyentry ktab[] = {
        { "int",     LEXINT },
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

/* One entry in the FSM array */

typedef struct {
        int     fsm_nextst;         /* Next state                           */
        int     fsm_action;         /* Action for this transition           */
        int     fsm_lextyp;         /* For FSM_RET, the token type to return*/
} fsmentry;

fsmentry fsm[STATES][CHAR_RANGE];
/* END */
