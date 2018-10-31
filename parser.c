#ifndef HEADERS
#define HEADERS 0
#include "parser.h"

#endif

int     main(int argc, char **argv) {
    if(argc != 2) {
        printf("Usage: ./parse [file]");
        exit(1);
    }
    file = fopen(argv[1], "r");
    parseinit();
    printf("No Errors\n");
    return 0;
}

void    parseinit() {
    lexinit();
    gettoken();
    program();
}

void    match(int tokTyp, char* msg) {
    nexttok.tok_typ == tokTyp ? gettoken() : errmsg(msg);
}

void    program(void) {
    pgm();
    match(LEXEOF, "Expected EOF");
}

void    pgm(void) {
    match(LEXPGM, "Expected \"program\"");
    match(LEXLB, "Expected \"{\"");
    parseDECLS();
    parseSTMTS();
    match(LEXRB, "Expected \"}\"");
    return;
}

void    parseDECLS() {
    char name[MAXTOK];
    while(nexttok.tok_typ == LEXINT) {
        gettoken();
        if(nexttok.tok_typ != LEXNAME) {
            errmsg("Expecting identifier");
        }
        strcpy(name, nexttok.tok_str);
        symStore(name, 0);
        memset(name, '\0', MAXTOK);
        gettoken();
        match(LEXSEMI, "Expected \";\"");
    }
    return;
}

void    parseSTMTS(void) {
    while(nexttok.tok_typ != LEXRB) {
        parseSTMT();
    }
}

void    parseSTMT(void) {
    char fname[MAXTOK];
    switch(nexttok.tok_typ) {
        case LEXIF:
                gettoken();     // Move past the if keyword
                parseIF();
                break;
        case LEXWHILE:
                gettoken();     // Move past the while keyword
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

void    parseIF() {
    match(LEXLP, "Expecting \"(\"");
    parseEXPR();
    match(LEXRP, "Expecting \")\"");
    match(LEXLB, "Expecting \"{\"");
    parseSTMTS();   // Parse statments until the next right brace
    match(LEXRB, "Expecting \"}\"");
    if(nexttok.tok_typ == LEXELSE) {
        gettoken();     // Remove the else keyword
        match(LEXLB, "Expecting \"{\"");
        parseSTMTS();
        match(LEXRB, "Expecting \"}\"");
    }
}

void    parseWHILE() {
    match(LEXLP, "Expecting \"(\"");
    parseEXPR();
    match(LEXRP, "Expecting \")\"");
    match(LEXLB, "Expecting \"{\"");
    parseSTMTS();
    match(LEXRB, "Expecting \"}\"");
}

void    parseEXPR() {
    parseCOMP();
    while(nexttok.tok_typ == LEXLOGIC) {
        // TODO need to save the logical operator
        gettoken();
        parseCOMP();
    }
}

void    parseCOMP() {
    parseADDN();
    while(nexttok.tok_typ == LEXCMPOP) {
        // TODO save the operator
        gettoken();
        parseADDN();
    }
}

void    parseADDN() {
    parseMULN();
    while(nexttok.tok_typ == LEXADDOP)  {
        // TODO save the operator
        gettoken();
        parseMULN();
    }
}

void    parseMULN() {
    parseTERM();
    while(nexttok.tok_typ == LEXMULOP) {
        // TODO save the operator
        gettoken();
        parseTERM();
    }
}

void    parseTERM() {
    if(nexttok.tok_typ == LEXNAME) {
        // TODO save the name
        int temp = symLookup(nexttok.tok_str);
        temp++; // TODO remove here to compile through -Wall
        gettoken();
    } else if(nexttok.tok_typ == LEXNUM) {
        // TODO save the num
        gettoken();
    } else if(nexttok.tok_typ == LEXLP) {
        gettoken();     // Remove the (
        parseEXPR();
        match(LEXRP, "Expecting \")");
    } else {
        errmsg("Invalid term in expression");
    }
}


void    parseASSIGN() {
    // current token is the identifier
    int symindex;
    char temptok[MAXTOK];

    symindex = symLookup(nexttok.tok_str);
    gettoken();
    match(LEXASNOP, "Expecting assignment operator");

    strcpy(temptok, nexttok.tok_str);
    if(nexttok.tok_typ == LEXREAD) {
        parseREAD();   
    } else {
        parseEXPR();
        match(LEXSEMI, "Expecting semicolon");
    }
}

void    parseREAD() {
    int varindex;
    int morevars;
    gettoken();
    match(LEXLP, "Expecting left paren");
    morevars = 2;
    int argcount = 0;
    while(morevars > 0) {
        argcount++;
        if(nexttok.tok_typ != LEXNAME) {
            errmsg("Expecting var name");
        }
        varindex = symLookup(nexttok.tok_str);
        gettoken();
        if(nexttok.tok_typ == LEXCOMMA) {
            gettoken();
        } else {
            morevars = 0;
        }
    }
    match(LEXRP, "Expecting right paren");
    return;
}

void    parseFCN(char* fname) {
    // match left paren
    match(LEXLP, "Expecting \"(\"");
    if(!strcmp(fname, "exit")) {
        // handle exit
        parseEXPR();
    } else {
        // handle print
        // see if list is empty
        if(nexttok.tok_typ == LEXRP) {
            errmsg("empty list");
        }
        int moreargs = 1;
        int argscount = 0;
        while(moreargs > 0) {
            argscount++;
            if(argscount >= MAXARGS) {
                errmsg("too many args, quitting");
            }
            if(nexttok.tok_typ == LEXSTR) {
                gettoken();
            } else {
                parseEXPR();
            }
            if(nexttok.tok_typ == LEXCOMMA) {
                gettoken();
            } else {
                moreargs = 0;
            }
        }
    }
    match(LEXRP, "Expected right paren");
    return;
}

int     symFind(char* name) {
    for(int i = 0; i < numsyms; i++) {
        if(!strcmp(sym_tab[i].sym_name, name)) {
            return i;
        }
    }
    return -1;
}

void    symStore(char* name, int val) {
    if(symFind(name) != -1) {
        errmsg("Cannont have two declarations with the same name");
    }
    strcpy(sym_tab[numsyms].sym_name, name);
    sym_tab[numsyms].sym_type = SYMINT;
    sym_tab[numsyms].size = sizeof(int);
    sym_tab[numsyms++].val =  val;
}

int     symLookup(char* name) {
    int indx;
    if((indx = symFind(name)) == -1) {
        errmsg("Cannot find defined variable");
    }
    return sym_tab[indx].val;
}
