#ifndef HEADERS
#define HEADERS 0
#include "parserASMGEN.h"

#endif

#define BRANCHLABEL "LAB"
#define LABEL 0
int label = 1;
#define VARLABEL "tmp"
#define VAR 1
int var = 1;
#define STRINGLABEL "msg"
#define MSG 2
int msg = 1;
#define LENLABEL "len"
#define LEN 3
int len = 1;

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

// TODO do asm
void pgm(void) {
  match(LEXPGM, "Expected \"program\"");
  match(LEXLB, "Expected \"{\"");
  printf("section .bss\n"
         "\tprintStack resb 100\n"
         "\tprintStackPointer resb 8\n");
  parseDECLS();
  printf("\nsection .text\n"
         "\tglobal _start\n");
  printIntFunc();
  printf("_start:\n");
  parseSTMTS();
  match(LEXRB, "Expected \"}\"");
  printf("mov eax, SYS_EXIT\n"
         "mov ebx, EXIT_CODE\n"
         "int 0x80\n");
  return;
}

void parseDECLS() {
  printf("section .data\n"
         "\tSYS_EXIT  equ 1\n"
         "\tSYS_READ  equ 3\n"
         "\tSYS_WRITE equ 4\n"
         "\tSTD_IN    equ 0\n"
         "\tSTD_OUT   equ 1\n"
         "\tEXIT_CODE equ 0\n"
         "\tNEW_LINE  db 0xa\n"
         "\ttmp000    dd 0\n");
  char name[MAXTOK];
  while (nexttok.tok_typ == LEXINT) {
    gettoken();
    if (nexttok.tok_typ != LEXNAME) {
      errmsg("Expecting identifier");
    }
    printf("\t%s dd 0\n", nexttok.tok_str);
    strcpy(name, nexttok.tok_str);
    symStore(name, 0);
    memset(name, '\0', MAXTOK);
    gettoken();
    while (nexttok.tok_typ == LEXCOMMA) {
      gettoken();
      if (nexttok.tok_typ != LEXNAME) {
        errmsg("Expecting identifier");
      }
      printf("\t%s dd 0\n", nexttok.tok_str);
      strcpy(name, nexttok.tok_str);
      symStore(name, 0);
      memset(name, '\0', MAXTOK);
      gettoken();
    }
    match(LEXSEMI, "Expected \";\"");
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
  int ifLabel = getTmpNum(LABEL);
  int boolCond = getTmpNum(VAR);
  parseEXPR(boolCond);
  match(LEXRP, "Expecting \")\"");
  match(LEXLB, "Expecting \"{\"");
  printf("cmp dword [%s%03d], 0\n"
         "je %s%03d\n",
         VARLABEL, boolCond, BRANCHLABEL, ifLabel);
  parseSTMTS(); // Parse statments until the next right brace
  match(LEXRB, "Expecting \"}\"");
  if (nexttok.tok_typ == LEXELSE) {
    gettoken(); // Remove the else keyword
    match(LEXLB, "Expecting \"{\"");
    int elseLabel = getTmpNum(LABEL);
    printf("jmp %s%03d\n", BRANCHLABEL, elseLabel);
    printf("%s%03d: \n", BRANCHLABEL, ifLabel);
    parseSTMTS();
    match(LEXRB, "Expecting \"}\"");
    printf("%s%03d: \n", BRANCHLABEL, elseLabel);
  } else {
    printf("%s%03d: \n", BRANCHLABEL, ifLabel);
  }
}

void parseWHILE() {
  match(LEXLP, "Expecting \"(\"");
  int startWhile = getTmpNum(LABEL);
  int endWhile = getTmpNum(LABEL);
  int boolCond = getTmpNum(VAR);
  printf("%s%03d: \n", BRANCHLABEL, startWhile);
  parseEXPR(boolCond);
  printf("cmp dword [%s%03d], 0\n"
         "je %s%03d\n",
         VARLABEL, boolCond, BRANCHLABEL, endWhile);
  match(LEXRP, "Expecting \")\"");
  match(LEXLB, "Expecting \"{\"");
  parseSTMTS();
  match(LEXRB, "Expecting \"}\"");
  printf("jmp %s%03d\n", BRANCHLABEL, startWhile);
  printf("%s%03d: \n", BRANCHLABEL, endWhile);
}

void parseEXPR(int save) {
  int storage = getTmpNum(VAR);
  parseCOMP(storage);
  while (nexttok.tok_typ == LEXLOGIC) {
    char op[2];
    strcpy(op, nexttok.tok_str);
    int storage2 = getTmpNum(VAR);
    gettoken();
    parseCOMP(storage2);
    printf("mov eax, [%s%03d]\n"
           "mov ebx, [%s%03d]\n",
           VARLABEL, storage, VARLABEL, storage2);
    if (!strcmp(op, "&&")) {
      printf("and eax, ebx\n");
    } else if (!strcmp(op, "||")) {
      printf("or eax, ebx\n");
    } else {
      errmsg("Invalid logical operator");
    }
    printf("mov dword [%s%03d], eax\n", VARLABEL, storage);
  }
  printf("mov eax, [%s%03d]\n"
         "mov dword [%s%03d], eax\n",
         VARLABEL, storage, VARLABEL, save);
}

void parseCOMP(int save) {
  int storage = getTmpNum(VAR);
  parseADDN(storage);
  while (nexttok.tok_typ == LEXCMPOP) {
    char op[2];
    strcpy(op, nexttok.tok_str);
    int storage2 = getTmpNum(VAR);
    gettoken();
    parseADDN(storage2);
    printf("mov eax, [%s%03d]\n"
           "mov ebx, [%s%03d]\n"
           "cmp eax, ebx\n",
           VARLABEL, storage, VARLABEL, storage2);
    int trueLabel = getTmpNum(LABEL);
    int endLabel = getTmpNum(LABEL);
    if (!strcmp(op, "==")) {
      printf("je ");
    } else if (!strcmp(op, "<=")) {
      printf("jle ");
    } else if (!strcmp(op, ">=")) {
      printf("jge ");
    } else if (!strcmp(op, "!=")) {
      printf("jne ");
    } else if (!strcmp(op, "<")) {
      printf("jl ");
    } else if (!strcmp(op, ">")) {
      printf("jg ");
    } else {
      errmsg("Invalid comparision operator");
    }
    printf("%s%03d\n"
           "mov eax, 0\n"
           "jmp %s%03d\n"
           "%s%03d:\n"
           "mov eax, 1\n"
           "%s%03d:\n",
           BRANCHLABEL, trueLabel, BRANCHLABEL, endLabel, BRANCHLABEL,
           trueLabel, BRANCHLABEL, endLabel);
    printf("mov dword [%s%03d], eax\n", VARLABEL, storage);
  }
  printf("mov eax, [%s%03d]\n"
         "mov dword [%s%03d], eax\n",
         VARLABEL, storage, VARLABEL, save);
}

void parseADDN(int save) {
  int storage = getTmpNum(VAR);
  parseMULN(storage);
  while (nexttok.tok_typ == LEXADDOP) {
    char op = nexttok.tok_str[0];
    int storage2 = getTmpNum(VAR);
    gettoken();
    parseMULN(storage2);
    printf("mov eax, [%s%03d]\n"
           "mov ebx, [%s%03d]\n",
           VARLABEL, storage, VARLABEL, storage2);
    switch (op) {
    case '+':
      printf("add eax, ebx\n");
      break;
    case '-':
      printf("sub eax, ebx\n");
      break;
    default:
      errmsg("Invalid addition operator");
    }
    printf("mov dword [%s%03d], eax\n", VARLABEL, storage);
  }
  printf("mov eax, [%s%03d]\n"
         "mov dword [%s%03d], eax\n",
         VARLABEL, storage, VARLABEL, save);
}

void parseMULN(int save) {
  int storage = getTmpNum(VAR);
  parseTERM(storage);
  while (nexttok.tok_typ == LEXMULOP) {
    char op = nexttok.tok_str[0];
    int storage2 = getTmpNum(VAR);
    gettoken();
    parseTERM(storage2);
    printf("mov eax, [%s%03d]\n"
           "mov ebx, [%s%03d]\n",
           VARLABEL, storage, VARLABEL, storage2);
    switch (op) {
    case '*':
      printf("imul eax, ebx\n");
      break;
    case '/':
      printf("idiv ebx\n");
      break;
    default:
      errmsg("Invalid multiply operator");
    }
    printf("mov dword [%s%03d], eax\n", VARLABEL, storage);
  }
  printf("mov eax, [%s%03d]\n"
         "mov dword [%s%03d], eax\n",
         VARLABEL, storage, VARLABEL, save);
}

void parseTERM(int save) {
  if (nexttok.tok_typ == LEXNAME) {
    symLookup(nexttok.tok_str);
    printf("mov eax, [%s]\n"
           "mov dword [%s%03d], eax\n",
           nexttok.tok_str, VARLABEL, save);
    gettoken();
  } else if (nexttok.tok_typ == LEXNUM) {
    printf("mov dword [%s%03d], %d\n", VARLABEL, save, nexttok.tok_val);
    gettoken();
  } else if (nexttok.tok_typ == LEXLP) {
    gettoken();
    int storage = getTmpNum(VAR);
    parseEXPR(storage);
    printf("mov eax, [%s%03d]\n"
           "mov dword [%s%03d], eax\n",
           VARLABEL, storage, VARLABEL, save);
    match(LEXRP, "Expecting \")");
  } else {
    errmsg("Invalid term in expression");
  }
}

void parseASSIGN() {
  // current token is the identifier
  int symindex;
  char operator[MAXTOK];
  int storage = getTmpNum(VAR);
  symindex = symLookup(nexttok.tok_str);
  symLookup(nexttok.tok_str);
  gettoken();
  if (nexttok.tok_typ != LEXASNOP) {
    errmsg("Expecting assignment operator");
  }
  strcpy(operator, nexttok.tok_str);
  gettoken();

  if (nexttok.tok_typ == LEXREAD) {
    parseREAD(storage);
  } else {
    parseEXPR(storage);
    match(LEXSEMI, "Expecting semicolon");
  }
  int len = strlen(operator);
  if (len == 2 && operator[1] == '=') {
    printf("mov eax, [%s]\n", sym_tab[symindex].sym_name);
    printf("mov ebx, [%s%03d]\n", VARLABEL, storage);
    switch (operator[0]) {
    case '+':
      printf("add eax, ebx\n");
      break;
    case '-':
      printf("sub eax, ebx\n");
      break;
    case '*':
      printf("imul eax, ebx\n");
      break;
    case '/':
      printf("idiv ebx\n");
      break;
    default:
      errmsg("Invalid Operator");
    }
  } else if (len == 1 && operator[0] == '=') {
    printf("mov eax, [%s%03d]\n", VARLABEL, storage);
  } else {
    errmsg("Invalid Operator");
  }
  printf("mov dword [%s], eax\n", sym_tab[symindex].sym_name);
}

// TODO do asm
void parseREAD(int save) {
  gettoken();
  match(LEXLP, "Expecting left paren");
  int more;
  int argc;

  more = 1;
  argc = 0;
  while (more > 0) {
    argc++;
    if (argc >= MAXARGS - 1) {
      errmsg("too many arguments to 'read'");
    }
    if (nexttok.tok_typ != LEXNAME) {
      errmsg("read expects a list of variable names");
    }
    symLookup(nexttok.tok_str);
    printf("mov eax, SYS_WRITE\n"
           "mov ebx, STD_IN\n"
           "mov ecx, %s\n"
           "mov edx, 4\n"
           "int 0x80\n",
           nexttok.tok_str);
    gettoken();
    if (nexttok.tok_typ == LEXCOMMA) {
      gettoken();
    } else {
      more = 0;
    }
  }
  match(LEXRP, "Expecting a ')'");
  printf("mov dword [%s%03d], %d\n", VARLABEL, save, argc);
  return;
}

// TODO do asm
void parseFCN(char *fname) {
  // match left paren
  match(LEXLP, "Expecting \"(\"");
  if (!strcmp(fname, "exit")) {
    int storage = getTmpNum(VAR);
    parseEXPR(storage);
    printf("mov eax, SYS_EXIT\n"
           "mov ebx, [%s%03d]\n"
           "int 0x80\n",
           VARLABEL, storage);
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
        int msgStorage = getTmpNum(MSG);
        int msgLenStorage = getTmpNum(LEN);
        printf("section .data\n"
               "\t%s%03d db '%s', 0xa\n"
               "\t%s%03d equ $ - %s%03d\n"
               "section .text\n",
               STRINGLABEL, msgStorage, nexttok.tok_str, LENLABEL,
               msgLenStorage, STRINGLABEL, msgStorage);
        printf("mov eax, SYS_WRITE\n"
               "mov ebx, STD_OUT\n"
               "mov ecx, %s%03d\n"
               "mov edx, %s%03d\n"
               "int 0x80\n",
               STRINGLABEL, msgStorage, LENLABEL, msgLenStorage);
        gettoken();
      } else {
        int storage = getTmpNum(VAR);
        parseEXPR(storage);
        printf("mov edi, [%s%03d]\n"
               "call _printINT\n",
               VARLABEL, storage);
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

int getTmpNum(int n) {
  if (!n) {
    label++;
    return label - 1;
  } else if (n == 1) {
    printf("section .data\n"
           "\t%s%03d dd 0\n"
           "section .text\n",
           VARLABEL, var);
    return var++;
  } else if (n == 2) {
    return msg++;
  } else if (n == 3) {
    return len++;
  }
  return -1;
}

void printIntFunc() {
  printf("_printINT:\n"
         "\tmov rax, rdi\n"
         "\tmov rcx, printStack\n"
         "\tmov rbx, 10\n"
         "\tmov [rcx], rbx\n"
         "\tinc rcx\n"
         "\tmov [printStackPointer], rcx\n\n"
         "_printINTLoop:\n"
         "\tmov rdx, 0\n"
         "\tmov rbx, 10\n"
         "\tdiv rbx\n"
         "\tpush rax\n"
         "\tadd rdx, 48\n\n"
         "\tmov rcx, [printStackPointer]\n"
         "\tmov [rcx], dl\n"
         "\tinc rcx\n"
         "\tmov [printStackPointer], rcx\n\n"
         "\tpop rax\n"
         "\tcmp rax, 0\n"
         "\tjne _printINTLoop\n\n"
         "_printINTLoop2:\n"
         "\tmov rcx, [printStackPointer]\n\n"
         "\tmov rax, 1\n"
         "\tmov rdi, 1\n"
         "\tmov rsi, rcx\n"
         "\tmov rdx, 1\n"
         "\tsyscall\n\n"
         "\tmov rcx, [printStackPointer]\n"
         "\tdec rcx\n"
         "\tmov [printStackPointer], rcx\n\n"
         "\tcmp rcx, printStack\n"
         "\tjge _printINTLoop2\n"
         "\tret\n");
}
