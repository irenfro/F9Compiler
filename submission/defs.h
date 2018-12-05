#define     BLANK       ' '
#define     TAB         '\t'
#define     NEWLINE     '\n'
#define     MAXTOK      100
#define     MAXINT      2147483647  // 2^31 - 1
#define     LEXUNUSED   0
#define     LEXEOF      1
#define     LEXLP       2
#define     LEXRP       3
#define     LEXLB       4
#define     LEXRB       5
#define     LEXSEMI     6
#define     LEXCOMMA    7
#define     LEXMULOP    8
#define     LEXASNOP    9
#define     LEXNAME     10
#define     LEXNUM      11
#define     LEXADDOP    12
#define     LEXCMPOP    13
#define     LEXSTR      14
#define     LEXLOGIC    15
#define     LEXINT      16
#define     LEXCHAR     17
#define     LEXIF       18
#define     LEXELSE     19
#define     LEXWHILE    20
#define     LEXPGM      21
#define     LEXREAD     22
#define     LEXFCN      23

typedef struct {
    int     tok_typ;
    char    tok_str[MAXTOK];
    int     tok_val;
} token;
