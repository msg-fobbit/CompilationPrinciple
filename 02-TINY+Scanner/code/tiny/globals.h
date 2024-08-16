
/****************************************************/
/* 文件: globals.h                                    */
/* TINY编译器的全局类型和变量                           */
/* 必须放在其他包含文件之前                              */
/* 编译器构造：原理与实践                                */
/* Kenneth C. Louden                                  */
/****************************************************/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = the number of reserved words */
#define MAXRESERVED 23

// typedef enum 
//     /* book-keeping tokens */
//    {ENDFILE,ERROR,
//     /* reserved words */
//     IF,THEN,ELSE,END,REPEAT,UNTIL,READ,WRITE,
//     /* multicharacter tokens */
//     ID,NUM,
//     /* special symbols */
//     ASSIGN,EQ,LT,PLUS,MINUS,TIMES,OVER,LPAREN,RPAREN,SEMI
//    } TokenType;

typedef enum 
    /*  book-keeping tokens */
   {ENDFILE,ERROR,
   /*  EOF       */
     /* 关键字 */
    IF,THEN,ELSE,END,REPEAT,UNTIL,READ,WRITE,
    /* 变量符号 */
    ID,NUM,
/*标识符,数字*/
    /* 特殊字符 */
    ASSIGN,EQ,LT,PLUS,MINUS,TIMES,OVER,LPAREN,RPAREN,SEMI,
    /* :=   =  <   +    -     *     /     (      )     ;    */
    /* 新增关键字 */
    TRUE1,FALSE1,OR,AND,NOT,INT,BOOL1,STRING,FLOAT,DOUBLE,DO,WHILE,INCLUDE,BREAK,CONTINUE,
    /* true,false,or,and,not,int,bool,string,float,double,do,while,include,break,continue */
    /* 新增变量符号*/
    STR,
    /* 字符串变量 */
    /* 新增特殊字符 */
    MT,ME,LE,COMMA,UPDOX,PERCENT,POWER,
/*   > >= <=  ,      '     %      **    */
   } TokenType;
extern FILE* source; /* source code text file */
extern FILE* listing; /* listing output text file */
extern FILE* code; /* code text file for TM simulator */

extern int lineno; /* source line number for listing */

/**************************************************/
/***********   Syntax tree for parsing ************/
/**************************************************/

typedef enum {StmtK,ExpK} NodeKind;
typedef enum {IfK,RepeatK,AssignK,ReadK,WriteK} StmtKind;
typedef enum {OpK,ConstK,IdK} ExpKind;

/* ExpType is used for type checking */
typedef enum {Void,Integer,Boolean} ExpType;

#define MAXCHILDREN 3

typedef struct treeNode
   { struct treeNode * child[MAXCHILDREN];
     struct treeNode * sibling;
     int lineno;
     NodeKind nodekind;
     union { StmtKind stmt; ExpKind exp;} kind;
     union { TokenType op;
             int val;
             char * name; } attr;
     ExpType type; /* for type checking of exps */
   } TreeNode;

/**************************************************/
/***********   Flags for tracing       ************/
/**************************************************/

/* EchoSource = TRUE causes the source program to
 * be echoed to the listing file with line numbers
 * during parsing
 */
extern int EchoSource;

/* TraceScan = TRUE causes token information to be
 * printed to the listing file as each token is
 * recognized by the scanner
 */
extern int TraceScan;

/* TraceParse = TRUE causes the syntax tree to be
 * printed to the listing file in linearized form
 * (using indents for children)
 */
extern int TraceParse;

/* TraceAnalyze = TRUE causes symbol table inserts
 * and lookups to be reported to the listing file
 */
extern int TraceAnalyze;

/* TraceCode = TRUE causes comments to be written
 * to the TM code file as code is generated
 */
extern int TraceCode;

/* Error = TRUE prevents further passes if an error occurs */
extern int Error; 
#endif
