/****************************************************/
/* File: scan.c                                     */
/* TINY编译器的词法分析器实现                        */
/* 编译器构造：原理与实践                             */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"

/* scanner DFA 中的状态 */
//  开始     赋值     注释     数字 标识符 结束 小于等于 大于等于  字符     幂
typedef enum
{
    START, INASSIGN, INCOMMENT, INNUM, INID, DONE, INLE, INME, INUPDOX,INPOWER
}
StateType;

/* 标识符或保留字的词素 */
char tokenString[MAXTOKENLEN + 1];

/* BUFLEN = 源代码行的输入缓冲区长度 */
#define BUFLEN 256

static char lineBuf[BUFLEN]; /* 存储当前行 */
static int linepos = 0; /* 当前行位置 */
static int bufsize = 0; /* 缓冲区字符串的当前大小 */
static int EOF_flag = FALSE; /* 修正 EOF 时 ungetNextChar 的行为 */

/* getNextChar 从 lineBuf 中获取下一个非空白字符，如果 lineBuf 耗尽，则读取新行 */
static int getNextChar(void)
{
    if (!(linepos < bufsize))
    {
        lineno++; // 增加行号
        if (fgets(lineBuf, BUFLEN - 1, source)) // 读取新行
        {
            if (EchoSource)
                fprintf(listing, "%d: %s", lineno, lineBuf); // 将源代码写入到列表文件
            bufsize = strlen(lineBuf); // 更新缓冲区大小
            linepos = 0; // 重置行位置
            return lineBuf[linepos++]; // 返回下一个字符
        }
        else
        {
            EOF_flag = TRUE; // 标记已到达文件尾
            return EOF;
        }
    }
    else return lineBuf[linepos++]; // 返回下一个字符
}

/* ungetNextChar 回退一个字符到 lineBuf 中 */
static void ungetNextChar(void)
{
    if (!EOF_flag) linepos--; // 如果未到达文件尾，则回退一个字符
}


/* 保留字查找表 */
static struct
{
    char* str;
    TokenType tok;
} reservedWords[MAXRESERVED] // 保留字数组
= { {"if",IF},{"then",THEN},{"else",ELSE},{"end",END}, // 初始化保留字
   {"repeat",REPEAT},{"until",UNTIL},{"read",READ},
   {"write",WRITE},{"true",TRUE1},{"false",FALSE1},
   {"or",OR},{"and",AND},{"not",NOT},{"int",INT},
   {"bool",BOOL1},{"string",STRING},{"float",FLOAT},
   {"double",DOUBLE},{"do",DO},{"while",WHILE},
   {"include",INCLUDE},{"break",BREAK},{"continue",CONTINUE}};

/* 查找标识符以确定是否为保留字，使用线性查找 */
static TokenType reservedLookup(char* s)
{
    int i;
    for (i = 0; i < MAXRESERVED; i++) // 遍历保留字数组
        if (!strcmp(s, reservedWords[i].str)) // 比较字符串是否相等
            return reservedWords[i].tok; // 如果相等，返回相应的标记类型
    return ID; // 否则返回标识符类型
}

/****************************************/
/* the primary function of the scanner  */
/****************************************/
/* function getToken returns the
 * next token in source file
 */
TokenType getToken(void)
{
    // 存储到 tokenString 的索引
    int tokenStringIndex = 0;
    // 当前要返回的标记
    TokenType currentToken;
    // 当前状态始终从 START 开始
    StateType state = START;
    // 标志是否保存到 tokenString
    int save;
    // 循环直到达到 DONE 状态
    while (state != DONE)
    {
        int c = getNextChar(); // 获取下一个字符
        save = TRUE;// 默认保存到 tokenString
        switch (state)
        {
        case START:
            if (isdigit(c))  // 如果是数字，切换到数字状态
                state = INNUM;
            else if (isalpha(c)) // 如果是字母，切换到标识符状态
                state = INID;
            else if (c == ':')  // 如果是冒号，切换到赋值状态
                state = INASSIGN;
            else if (c == '>')         // 如果是大于号，切换到大于等于状态
                state = INME;
            else if (c == '<')         // 如果是小于号，切换到小于等于状态
                state = INLE;
            else if ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r'))
                save = FALSE;  //  不保存到 tokenString
            else if (c == '{')
            {
                save = FALSE;
                state = INCOMMENT;  // 切换到注释状态
            }
            else if (c == '}')
	    {
		state = DONE;
		currentToken = ERROR;
		strcpy(tokenString, "Unexpected '}' outside of comment!");
		tokenStringIndex += 34;
		break;
	    }
            else if (c == '\'')
            {
                save = FALSE;
                state = INUPDOX;  // 切换到字符状态
            }
            else if (c == '*') // 如果是星号
            {
                state = INPOWER; // 切换到 INPOWER 状态
            }
            else
            {
                state = DONE;  // // 切换到结束状态
                switch (c)
                {
                case EOF:
                    save = FALSE;
                    currentToken = ENDFILE;
                    break;
                case '=':
                    currentToken = EQ;
                    break;
                case '+':
                    currentToken = PLUS;
                    break;
                case '-':
                    currentToken = MINUS;
                    break;
                case '/':
                    currentToken = OVER;
                    break;
                case '(':
                    currentToken = LPAREN;
                    break;
                case ')':
                    currentToken = RPAREN;
                    break;
                case ';':
                    currentToken = SEMI;
                    break;
                case ',':
                    currentToken = COMMA;
                    break;
                case '%':
                    currentToken = PERCENT;
                    break;
                default:
                    currentToken = ERROR;
                    break;
                }
            }
            break;
        case INCOMMENT:  // 注释状态
            save = FALSE;  // 不保存到 tokenString
            if (c == EOF)  // 如果是文件尾
            {
                state = DONE;      // 切换到结束状态
                currentToken = ERROR; // 返回错误标记
                strcpy(tokenString, "Missing \" } \" !"); // 错误信息
                tokenStringIndex += 15; // 更新 tokenString 索引
            }
            else if (c == '}')   //   如果是注释结束符
                state = START;     // 切换到开始状态
            else if (c == '{')   // 如果是注释开始符
	    {
		state = DONE;     // 切换到结束状态
		currentToken = ERROR; // 返回错误标记
		strcpy(tokenString, "Nested comments are not allowed!"); // 错误信息
		tokenStringIndex += 31; // 更新 tokenString 索引
	    }
            break;
        case INUPDOX:  // 字符状态
            if (c == '\'')
            {
                save = FALSE;
                state = DONE;
                currentToken = STR;
            }
            else if (!(linepos < bufsize)) // 如果行缓冲区耗尽
            {
                save = FALSE;
                state = DONE;
                currentToken = ERROR;
                strcpy(tokenString, "Missing \" \' \" !");
                tokenStringIndex += 15;
            }
            break;
        case INASSIGN:   // 赋值状态
            state = DONE;
            if (c == '=')
                currentToken = ASSIGN;
            else
            { /* backup in the input */
                ungetNextChar();     // 回退一个字符
                save = FALSE;
                currentToken = ERROR;// 返回错误标记
            }
            break;
        case INNUM:
            if (!isdigit(c))
            { /* backup in the input */
                ungetNextChar();
                save = FALSE;
                state = DONE;
                currentToken = NUM;
            }
            break;
        case INLE:
            if (c == '=')          // 如果是等于号,表示小于等于
            {
                state = DONE;      
                currentToken = LE;
            }
            else                  // 否则表示小于
            { /* backup in the input */
                ungetNextChar();
                save = FALSE;
                state = DONE;
                currentToken = LT;
            }
            break;
        case INME:                
            if (c == '=')            // 如果是等于号,表示大于等于
            {
                state = DONE;
                currentToken = ME;
            }
            else                    // 否则表示大于
            { /* backup in the input */
                ungetNextChar();
                save = FALSE;
                state = DONE;
                currentToken = MT;
            }
            break;
        case INPOWER:
            if (c == '*')            // 如果是星号
            {
                state = DONE; // 切换到结束状态
                currentToken = POWER; // 返回 POWER
            }
            else{
                ungetNextChar();
                state = DONE;
                save = FALSE;
                currentToken = TIMES; // 返回 TIMES
            }
            break;
        case INID:
            if (!(isalpha(c) || isdigit(c)))  // 如果不是字母或数字
            { /* backup in the input */
                ungetNextChar();
                save = FALSE;
                state = DONE;
                currentToken = ID;             // 返回标识符
            }
            break;
        
        case DONE:
        default: /* should never happen */
            fprintf(listing, "Scanner Bug: state= %d\n", state); // 打印错误信息
            state = DONE;
            currentToken = ERROR;
            break;
        }
        if ((save) && (tokenStringIndex <= MAXTOKENLEN))  
            tokenString[tokenStringIndex++] = (char)c;   
        if (state == DONE)    // 如果状态为结束状态
        {
            tokenString[tokenStringIndex] = '\0'; 
            if (currentToken == ID)
                currentToken = reservedLookup(tokenString);
        }
    }
    if (TraceScan) {
        fprintf(listing, "\t%d: ", lineno);
        printToken(currentToken, tokenString);
    }
    return currentToken;
} /* end getToken */
