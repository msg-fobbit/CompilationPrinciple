/****************************************************/
/* File: parse.c                                    */
/* The parser implementation for the TINY compiler  */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static TokenType token; /* holds current token */

/* function prototypes for recursive calls */
static TreeNode * stmt_sequence(void);
static TreeNode * statement(void);
static TreeNode * if_stmt(void);
static TreeNode * repeat_stmt(void);
static TreeNode * assign_stmt(void);
static TreeNode * read_stmt(void);
static TreeNode * write_stmt(void);
static TreeNode * exp(void);
static TreeNode * simple_exp(void);
static TreeNode * term(void);
static TreeNode * factor(void);
// 新增
static TreeNode * while_stmt(void);
static TreeNode * type_specifier(void);
static TreeNode * varlist(void);
static TreeNode * decl(void);
static TreeNode * declarations(void);


static void syntaxError(char * message)
{ fprintf(listing,"\n>>> ");
  fprintf(listing,"Syntax error at line %d: %s",lineno,message);
  Error = TRUE;
}
//判断当前token是否与输入的expected一致。
//一致则继续获取下一个token，否则报错。
static void match(TokenType expected)
{ if (token == expected) token = getToken();
  else {
    syntaxError("unexpected token -> ");
    printToken(token,tokenString);
    fprintf(listing,"      ");
  }
}
TreeNode * stmt_sequence(void)
{ TreeNode * t = statement();
  TreeNode * p = t;
  while ((token!=ENDFILE) && (token!=END) &&
         (token!=ELSE) && (token!=UNTIL)&&(token!=WHILE))
  { TreeNode * q;
    match(SEMI);
    q = statement();
    if (q!=NULL) {
      if (t==NULL) t = p = q;
      else /* now p cannot be NULL either */
      { p->sibling = q;
        p = q;
      }
    }
  }
  return t;
}

// 判断关键字
TreeNode * statement(void)
{ TreeNode * t = NULL;
  switch (token) {
    case IF : t = if_stmt(); break;
    case REPEAT : t = repeat_stmt(); break;
    case ID : t = assign_stmt(); break;
    case READ : t = read_stmt(); break;
    case WRITE : t = write_stmt(); break;
    // 新增
    case DO : t = while_stmt(); break;
    default : syntaxError("unexpected token -> ");
              printToken(token,tokenString);
              token = getToken();
              break;
  } /* end case */
  return t;
}

TreeNode * if_stmt(void)
{ TreeNode * t = newStmtNode(IfK);
  match(IF);
  if (t!=NULL) t->child[0] = exp();
  match(THEN);
  if (t!=NULL) t->child[1] = stmt_sequence();
  if (token==ELSE) {
    match(ELSE);
    if (t!=NULL) t->child[2] = stmt_sequence();
  }
  match(END);
  return t;
}

TreeNode * repeat_stmt(void)
{ TreeNode * t = newStmtNode(RepeatK);
  match(REPEAT);
  if (t!=NULL) t->child[0] = stmt_sequence();
  match(UNTIL);
  if (t!=NULL) t->child[1] = exp();
  return t;
}

TreeNode * assign_stmt(void)
{ TreeNode * t = newStmtNode(AssignK);
  if ((t!=NULL) && (token==ID))
    t->attr.name = copyString(tokenString);
  match(ID);
  match(ASSIGN);
  if (t!=NULL) t->child[0] = exp();
  return t;
}

TreeNode * read_stmt(void)
{ TreeNode * t = newStmtNode(ReadK);
  match(READ);
  if ((t!=NULL) && (token==ID))
    t->attr.name = copyString(tokenString);
  match(ID);
  return t;
}

TreeNode * write_stmt(void)
{ TreeNode * t = newStmtNode(WriteK);
  match(WRITE);
  if (t!=NULL) t->child[0] = exp();
  return t;
}

TreeNode * exp(void)
{ TreeNode * t = simple_exp();
  if ((token==LT)||(token==EQ)||(token==GT)||(token==LTE)||(token==GTE)) {
    TreeNode * p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
    }
    match(token);
    if (t!=NULL)
      t->child[1] = simple_exp();
  }
  return t;
}

TreeNode * simple_exp(void)
{ TreeNode * t = term();
  while ((token==PLUS)||(token==MINUS))
  { TreeNode * p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      t->child[1] = term();
    }
  }
  return t;
}

TreeNode * term(void)
{ TreeNode * t = factor();
  while ((token==TIMES)||(token==OVER))
  { TreeNode * p = newExpNode(OpK);
    if (p!=NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      p->child[1] = factor();
    }
  }
  return t;
}
// 修改
TreeNode * factor(void)
{ TreeNode * t = NULL;
  switch (token) {
    case NUM :
      t = newExpNode(ConstK);
      if ((t!=NULL) && (token==NUM))
          t->attr.val = atoi(tokenString);
      match(NUM);
      break;
    case ID :
      t = newExpNode(IdK);
      if ((t!=NULL) && (token==ID))
        t->attr.name = copyString(tokenString);
      match(ID);
      break;
    case LPAREN :
      match(LPAREN);
      t = exp();
      match(RPAREN);
      break;
    // 新增
    case STR:
      t = newExpNode(ConstK);
      if(t!=NULL && token==STR) {
        t->attr.name = copyString(tokenString);
        t->type = String;
      }
      match(STR);
      break;
    default:
      syntaxError("unexpected token -> ");
      printToken(token,tokenString);
      token = getToken();
      break;
    }
  return t;
}



// 新增，while
TreeNode* while_stmt(void){
  TreeNode *t = newStmtNode(WhileK);
  match(DO);
  if(t!=NULL) t->child[0] = stmt_sequence();
  match(WHILE);
  if(t!=NULL) t->child[1] = exp();
  return t;
}
//新增
TreeNode* type_specifier(void){
  TreeNode *t = newExpNode(SpecifierK);
  switch (token)
  {
  case INT:
    t->type = Integer;match(INT);break;
  case BOOL:
    t->type = Boolean;match(BOOL);break;
  case STRING:
    t->type = String;match(STRING);break;
  case FLOAT:
    t->type = Float;match(FLOAT);break;
  case DOUBLE:
    t->type = Double;match(DOUBLE);break;
  default:
    syntaxError("unexpected token -> ");
    printToken(token,tokenString);
    token = getToken();
    break;
  }
  return t;
} 
// 新增
TreeNode* varlist(void){
  TreeNode *t = newExpNode(IdK);
  if(t!=NULL && token==ID) t->attr.name = copyString(tokenString);
  match(ID);
  TreeNode *p = t;
  while((token!=SEMI)){
    TreeNode *q;
    match(COMMA);
    q = newExpNode(IdK);
    if(q!=NULL && token==ID) q->attr.name = copyString(tokenString);
    match(ID);
    if(q != NULL){
      if(t==NULL) t = p = q;
      else{
        p->child[0] = q;
        p = q;
      }
    }
  }
  return t;
}
// 新增
TreeNode* decl(void){
  TreeNode *t = type_specifier();
  if (t!=NULL) t->child[0] = varlist();
  return t;
}
TreeNode* declarations(void){
  TreeNode *t = newDeclNode();
  TreeNode *p = t;
  int notNull = FALSE;

  while((token!=ENDFILE)&&(token == INT || token == BOOL || token == STRING || token == FLOAT || token == DOUBLE)){
    notNull = TRUE;
    TreeNode *q = decl();
    match(SEMI);
    if(q!=NULL){
      if(t==NULL) t = p = q;
      else{
        p->sibling = q;
        p = q;
      }
    }
  }
  return notNull ? t : NULL;
}

/****************************************/
/* the primary function of the parser   */
/****************************************/
/* Function parse returns the newly 
 * constructed syntax tree
 */
// 新增
TreeNode * parse(void)
{ 
  TreeNode *t = newProNode();
  token = getToken();
  t->child[0] = declarations();
  t->child[1] = stmt_sequence();
  // 若有声明，t为pro的孩子，找到t的最右兄弟，与p连接

  if (token!=ENDFILE)
    syntaxError("Code ends before file\n");
  return t;
}
