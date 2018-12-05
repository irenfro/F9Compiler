#ifndef HEADERS
#define HEADERS 0
#include "parser.h"

#endif

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: ./parse [file]");
    exit(1);
  }
  file = fopen(argv[1], "r");
  parseinit();
  return 0;
}

void parseinit() {
  lexinit();
  gettoken();
  program();
}

void match(int tokTyp, char *msg) {
  nexttok.tok_typ == tokTyp ? gettoken() : errmsg(msg);
}

void program(void) {
  pgm();
  match(LEXEOF, "Expected EOF");
}

void pgm(void) {
  match(LEXPGM, "Expected \"program\"");
  /* Code Gen */
  printf("#include <stdio.h>\n");
  printf("#include <stdlib.h>\n");
  /* End Code Gen */
  match(LEXLB, "Expected \"{\"");
  printf("int    main(void)  {\n");
  parseDECLS();
  parseSTMTS();
  match(LEXRB, "Expected \"}\"");
  printf("    exit(0);\n}");
  return;
}

void parseDECLS() {
  char name[MAXTOK];
  while (nexttok.tok_typ == LEXINT) {
    printf("         %s    ", nexttok.tok_str);
    gettoken();
    if (nexttok.tok_typ != LEXNAME) {
      errmsg("Expecting identifier");
    }
    printf("%s", nexttok.tok_str);
    strcpy(name, nexttok.tok_str);
    symStore(name, 0);
    memset(name, '\0', MAXTOK);
    gettoken();
    while (nexttok.tok_typ == LEXCOMMA) {
      gettoken();
      if (nexttok.tok_typ != LEXNAME) {
        errmsg("Expecting identifier");
      }
      printf(", %s", nexttok.tok_str);
      strcpy(name, nexttok.tok_str);
      symStore(name, 0);
      memset(name, '\0', MAXTOK);
    }
    match(LEXSEMI, "Expected \";\"");
    printf(";\n");
  }
  return;
}

void parseSTMTS(void) {
  while (nexttok.tok_typ != LEXRB) {
    parseSTMT();
  }
}

void parseSTMT(void) {
  char fname[MAXTOK];
  switch (nexttok.tok_typ) {
  case LEXIF:
    gettoken(); // Move past the if keyword
    parseIF();
    break;
  case LEXWHILE:
    gettoken(); // Move past the while keyword
    parseWHILE();
    break;
  case LEXNAME:
    // Do not call gettoken here because we need the name
    parseASSIGN();
    break;
  case LEXFCN:
    strcpy(fname, nexttok.tok_str);
    gettoken();
    parseFCN(fname);
    break;
  case LEXSEMI:
    errmsg("Did not expect \";\"");
    break;
  default:
    errmsg("Expecting a statment");
  }
}

void parseIF() {
  match(LEXLP, "Expecting \"(\"");
  printf("if (");
  parseEXPR();
  match(LEXRP, "Expecting \")\"");
  printf(")");
  match(LEXLB, "Expecting \"{\"");
  printf(" { \n");
  parseSTMTS(); // Parse statments until the next right brace
  match(LEXRB, "Expecting \"}\"");
  printf("}\n");
  if (nexttok.tok_typ == LEXELSE) {
    gettoken(); // Remove the else keyword
    match(LEXLB, "Expecting \"{\"");
    printf(" else {\n");
    parseSTMTS();
    match(LEXRB, "Expecting \"}\"");
    printf(" }\n");
  }
}

void parseWHILE() {
  match(LEXLP, "Expecting \"(\"");
  printf("while ( ");
  parseEXPR();
  match(LEXRP, "Expecting \")\"");
  printf(" )");
  match(LEXLB, "Expecting \"{\"");
  printf(" {\n");
  parseSTMTS();
  match(LEXRB, "Expecting \"}\"");
  printf(" }\n");
}

void parseEXPR() {
  parseCOMP();
  while (nexttok.tok_typ == LEXLOGIC) {
    printf(" %s ", nexttok.tok_str);
    gettoken();
    parseCOMP();
  }
}

void parseCOMP() {
  parseADDN();
  while (nexttok.tok_typ == LEXCMPOP) {
    printf(" %s ", nexttok.tok_str);
    gettoken();
    parseADDN();
  }
}

void parseADDN() {
  parseMULN();
  while (nexttok.tok_typ == LEXADDOP) {
    printf(" %s ", nexttok.tok_str);
    gettoken();
    parseMULN();
  }
}

void parseMULN() {
  parseTERM();
  while (nexttok.tok_typ == LEXMULOP) {
    printf(" %s ", nexttok.tok_str);
    gettoken();
    parseTERM();
  }
}

void parseTERM() {
  if (nexttok.tok_typ == LEXNAME) {
    symLookup(nexttok.tok_str);
    printf("%s", nexttok.tok_str);
    gettoken();
  } else if (nexttok.tok_typ == LEXNUM) {
    printf("%d", nexttok.tok_val);
    gettoken();
  } else if (nexttok.tok_typ == LEXLP) {
    printf("( ");
    gettoken();
    parseEXPR();
    match(LEXRP, "Expecting \")");
    printf(" )");
  } else {
    errmsg("Invalid term in expression");
  }
}

void parseASSIGN() {
  // current token is the identifier
  // int symindex;
  char temptok[MAXTOK];

  // symindex = symLookup(nexttok.tok_str);
  symLookup(nexttok.tok_str);
  printf("%s  ", nexttok.tok_str);
  gettoken();
  if (nexttok.tok_typ != LEXASNOP) {
    errmsg("Expecting assignment operator");
  }
  printf("%s  ", nexttok.tok_str);
  gettoken();

  strcpy(temptok, nexttok.tok_str);
  if (nexttok.tok_typ == LEXREAD) {
    parseREAD();
  } else {
    parseEXPR();
    match(LEXSEMI, "Expecting semicolon");
    printf(";\n");
  }
}

void parseREAD() {
  gettoken();
  match(LEXLP, "Expecting left paren");
  int more;
  int varindex;
  int indexes[MAXARGS];
  int argc;

  more = 1;
  argc = 0;
  while (more > 0) {

    if (argc >= MAXARGS - 1) {
      errmsg("too many arguments to 'read'");
    }
    if (nexttok.tok_typ != LEXNAME) {
      errmsg("read expects a list of variable names");
    }
    varindex = symLookup(nexttok.tok_str);
    indexes[argc++] = varindex;
    gettoken();
    if (nexttok.tok_typ == LEXCOMMA) {
      gettoken();
    } else {
      more = 0;
    }
  }
  match(LEXRP, "Expecting a ')'");
  printf("scanf(\"");
  for (int i = 0; i < argc; i++) {
    printf("%%d");
  }
  printf("\"");
  for (int i = 0; i < argc; i++) {
    printf(", &%s", sym_tab[indexes[i]].sym_name);
  }
  printf(");\n");
  return;
}

void parseFCN(char *fname) {
  // match left paren
  match(LEXLP, "Expecting \"(\"");
  if (!strcmp(fname, "exit")) {
    printf("exit( ");
    parseEXPR();
    printf(" );\n");
  } else {
    // handle print
    // see if list is empty
    if (nexttok.tok_typ == LEXRP) {
      errmsg("empty list");
    }
    int moreargs = 1;
    int argscount = 0;
    while (moreargs > 0) {
      argscount++;
      if (argscount >= MAXARGS) {
        errmsg("too many args, quitting");
      }
      if (nexttok.tok_typ == LEXSTR) {
        symFind(nexttok.tok_str);
        printf("printf(\"%%s\", %s);\n", nexttok.tok_str);
        gettoken();
      } else {
        printf("printf(\"\%%d\", ");
        parseEXPR();
        printf(" );\n");
      }
      if (nexttok.tok_typ == LEXCOMMA) {
        gettoken();
      } else {
        moreargs = 0;
      }
    }
  }
  match(LEXRP, "Expected right paren");
  return;
}

int symFind(char *name) {
  for (int i = 0; i < numsyms; i++) {
    if (!strcmp(sym_tab[i].sym_name, name)) {
      return i;
    }
  }
  return -1;
}

void symStore(char *name, int val) {
  if (symFind(name) != -1) {
    errmsg("Cannont have two declarations with the same name");
  }
  strcpy(sym_tab[numsyms].sym_name, name);
  sym_tab[numsyms].sym_type = SYMINT;
  sym_tab[numsyms].size = sizeof(int);
  sym_tab[numsyms++].val = val;
}

int symLookup(char *name) {
  int indx;
  if ((indx = symFind(name)) == -1) {
    errmsg("Cannot find defined variable");
  }
  return indx;
}
