/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"



/* Procedure printToken prints a token 
 * and its lexeme to the listing file
 */
void printToken( TokenType token, const char* tokenString )
{ switch (token)
  { case IF:
    case THEN:
    case ELSE:
    case END:
    case REPEAT:
    case UNTIL:
    case READ:
    case WRITE:
	case T_TRUE:
	case T_FALSE:
	case OR:
	case AND:
	case NOT:
	case INT:
	case BOOL:
	case STRING:
  case FLOAT:
  case DOUBLE:
	case DO:
	case WHILE: 
       fprintf(listing,
         "reserved word: %s\n",tokenString);
		 break;
    case ASSIGN: fprintf(listing,":=\n"); break;

    case LT: fprintf(listing,"<\n"); break;
    case EQ: fprintf(listing,"=\n"); break;
	case GT: fprintf(listing,">\n"); break;
    case LTE: fprintf(listing,"<=\n"); break;
	case GTE: fprintf(listing,">=\n"); break;

    case LPAREN: fprintf(listing,"(\n"); break;
    case RPAREN: fprintf(listing,")\n"); break;
    case SEMI: fprintf(listing,";\n"); break;
	case COMMA: fprintf(listing,",\n"); break;
    case SQM: fprintf(listing,"\'\n"); break;
    case PLUS: fprintf(listing,"+\n"); break;
    case MINUS: fprintf(listing,"-\n"); break;
    case TIMES: fprintf(listing,"*\n"); break;
    case OVER: fprintf(listing,"/\n"); break;
    case ENDFILE: fprintf(listing,"EOF\n"); break;
    case NUM:
      fprintf(listing,
          "NUM, val= %s\n",tokenString);
      break;
    case ID:
      fprintf(listing,
          "ID, name= %s\n",tokenString);
      break;
	case STR:
	  fprintf(listing,"STR,name= %s\n",tokenString);
	  break;
	case ERROR: {
	
			fprintf(listing,
				"ERROR %s :%s\n",errorMsg[errorCode],tokenString);

		
		}break;
   
	

    default: /* should never happen */
      fprintf(listing,"Unknown token: %d\n",token);
  }
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode * newStmtNode(StmtKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
    t->lineno = lineno;
  }
  return t;
}

/* Function newExpNode creates a new expression 
 * node for syntax tree construction
 */
TreeNode * newExpNode(ExpKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->lineno = lineno;
    t->type = Void;
  }
  return t;
}

// 新增
/* Function newDeclNode creates a new declaration
 * node for syntax tree construction
 */
TreeNode* newDeclNode(){
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if(t == NULL){
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  }else{
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = DeclK;
    t->lineno = lineno; 
  }
  return t;
}

TreeNode* newProNode(){
  TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
  int i;
  if(t == NULL){
    fprintf(listing, "Out of memory error at line %d\n", lineno);
  }else{
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ProK;
    t->lineno = lineno; 
  }
  return t;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char * copyString(char * s)
{ int n;
  char * t;
  if (s==NULL) return NULL;
  n = strlen(s)+1;
  t = malloc(n);
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else strcpy(t,s);
  return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{ int i;
  for (i=0;i<indentno;i++)
    fprintf(listing," ");
}

/* procedure printTree prints a syntax tree to the 
 * listing file using indentation to indicate subtrees
 */
void printTree( TreeNode * tree )
{ int i;
  INDENT;
  while (tree != NULL) {
    if (tree->nodekind==StmtK)
    { 
      printSpaces();
      switch (tree->kind.stmt) {
        case IfK:
          fprintf(listing,"If\n");
          break;
        case RepeatK:
          fprintf(listing,"Repeat\n");
          break;
        case AssignK:
          fprintf(listing,"Assign to: %s\n",tree->attr.name);
          break;
        case ReadK:
          fprintf(listing,"Read: %s\n",tree->attr.name);
          break;
        case WriteK:
          fprintf(listing,"Write\n");
          break;
        // 新增
        case DoK:
          fprintf(listing,"Do\n");
          break;
        case WhileK:
          fprintf(listing,"While\n");
          break;
        default:
          fprintf(listing,"Unknown ExpNode kind\n");
          break;
      }
    }
    else if (tree->nodekind==ExpK)
    { 
      printSpaces();
      switch (tree->kind.exp) {
        case OpK:
          fprintf(listing,"Op: ");
          printToken(tree->attr.op,"\0");
          break;
        case ConstK:
          if(tree->type == String)
            fprintf(listing,"Const: string: %s\n",tree->attr.name);
          else
            fprintf(listing,"Const: Integer: %d\n",tree->attr.val);
          break;
        case IdK:
          fprintf(listing,"Id: %s\n",tree->attr.name);
          break;
        case SpecifierK:
          switch(tree->type){
            case Integer:
              fprintf(listing,"Type: int\n");
              break;
            case Boolean:
              fprintf(listing, "Type: bool\n");
              break;
            case String:
              fprintf(listing, "Type: string\n");
              break;
            case Double:
              fprintf(listing, "Type: double\n");
              break;
            case Float:
              fprintf(listing, "Type: float\n");
              break;
          }
          break;
        default:
          fprintf(listing,"Unknown ExpNode kind\n");
          break;
      }
    }
    // 新增
   
    // 新增
    else if(tree->nodekind == ProK){
      printSpaces();
      fprintf(listing, "Program:\n");
    }
    else if(tree->nodekind == DeclK){
     //取消回退缩进
    
    }

    else fprintf(listing,"Unknown node kind\n");
    for (i=0;i<MAXCHILDREN;i++)
         printTree(tree->child[i]);
    tree = tree->sibling;
  }
  UNINDENT;
}





int isLegalChar(char c) {
// 新增

	return (isalnum(c)||
			isspace(c)||
			c == '>'||
			c == '<' ||
			c == '=' ||
			c == ',' ||
			c ==';' ||
			c =='\'' ||
			c =='{'	||
			c =='}' ||
			c =='+' ||
			c =='-' ||
			c =='*' ||
			c =='/' ||
			c =='(' ||
			c ==')' 
			);


	}