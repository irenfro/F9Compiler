#include "defs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* PARSER           */

/* PARSER DEFINES   */
#define MAXARGS 10
#define MAXSYMS 100
#define SYMINT 0
FILE *file; /*  File to read from */
/* END              */

/* PARSER EXTERNS   */
extern void lexinit(void);
extern void gettoken(void);
extern void errmsg(char *);
extern token nexttok;
/* END              */

/* PARSER STRUCTS DEFINES   */
typedef struct {
  char sym_name[MAXTOK];
  int sym_type;
  int size;
  int val;
} sym_entry;

sym_entry sym_tab[MAXSYMS];
int numsyms = 0;
/* END                      */

/* PARSER PROTOTYPES                */
int main(int, char **);
void parseinit(void);
void match(int, char *);
void program(void);
void pgm(void);
void parseDECLS(void);
void parseSTMTS(void);
void parseSTMT(void);
void parseIF(void);
void parseWHILE(void);
void parseEXPR(int);
void parseCOMP(int);
void parseADDN(int);
void parseMULN(int);
void parseTERM(int);
void parseASSIGN(void);
void parseREAD(int);
void parseFCN(char *);
void printIntFunc(void);
void symStore(char *, int);
int symLookup(char *);
int symFind(char *);
int getTmpNum(int);
/* END                              */
