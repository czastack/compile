/**
 * 词法分析
 */

#pragma once

#include <string>
#include <vector>
#include "common.h"

// 保留字表
extern char RESERVE_WORD[41][10];

// 界符运算符表
extern char OPERATOR_OR_DELIMITER[39][3];

// 界符运算符表
extern char ACTIONSIGN[23][20];

enum 
{
	RESERVE_WORD_LEN = lengthof(RESERVE_WORD),
	OPERATOR_OR_DELIMITER_LEN = lengthof(OPERATOR_OR_DELIMITER),
	OPERATOR_OR_DELIMITER_START = RESERVE_WORD_LEN,
	ACTIONSIGN_LEN = lengthof(ACTIONSIGN),
	ACTIONSIGN_START = RESERVE_WORD_LEN + OPERATOR_OR_DELIMITER_LEN,
};

enum SYN
{
	SYN_END,
	SYN_AUTO,
	SYN_BREAK,
	SYN_CASE,
	SYN_CHAR,
	SYN_CONST,
	SYN_CONTINUE,
	SYN_DEFAULT,
	SYN_DO,
	SYN_DOUBLE,
	SYN_ELSE,
	SYN_ENUM,
	SYN_EXTERN,
	SYN_FLOAT,
	SYN_FOR,
	SYN_GOTO,
	SYN_IF,
	SYN_INT,
	SYN_LONG,
	SYN_RETURN,
	SYN_SHORT,
	SYN_SIGNED,
	SYN_SIZEOF,
	SYN_STATIC,
	SYN_STRUCT,
	SYN_SWITCH,
	SYN_TYPEDEF,
	SYN_UNION,
	SYN_UNSIGNED,
	SYN_VOID,
	SYN_VOLATILE,
	SYN_WHILE,

	SYN_MAIN,
	SYN_PRINTF,
	SYN_SCANF,
	SYN_ID,
	SYN_STRING,
	SYN_CNUM, // char 常量
	SYN_INUM, // int 常量
	SYN_FNUM, // float 常量
	SYN_EMPTY,
	SYN_START,

	SYN_PTR,
	SYN_IADD,
	SYN_ISUB,
	SYN_ADD,
	SYN_SUB,
	SYN_MUL,
	SYN_DIV,
	SYN_MOD,
	SYN_LEFT_SHIFT,
	SYN_RIGHT_SHIFT,
	SYN_LTE,
	SYN_GTE,
	SYN_EQ,
	SYN_NE,
	SYN_LT,
	SYN_GT,
	SYN_ASSIGN,
	SYN_SEMICOLON,
	SYN_LEFT_BRACKET,
	SYN_RIGHT_BRACKET,
	SYN_CAP,
	SYN_COMMA,
	SYN_DQUOTE,
	SYN_SQUOTE,
	SYN_SHARP,
	SYN_LOGICAL_AND,
	SYN_LOGICAL_OR,
	SYN_AND,
	SYN_OR,
	SYN_TILDE,
	SYN_LEFT_SQUARE_BRACKET,
	SYN_RIGHT_SQUARE_BRACKET,
	SYN_LEFT_CURLY_BRACKET,
	SYN_RIGHT_CURLY_BRACKET,
	SYN_BACKSLASH,
	SYN_DOT,
	SYN_QUESTIONMARK,
	SYN_COLON,
	SYN_NOT,

	ACT_ASSIGN,
	ACT_EQ,
	ACT_ADD_SUB,
	ACT_ADD,
	ACT_SUB,
	ACT_DIV_MUL,
	ACT_DIV,
	ACT_MUL,
	ACT_TRAN_LF,
	ACT_SINGLE_OP,
	ACT_COMPARE_OP,
	ACT_COMPARE,
	ACT_IF_FJ,
	ACT_IF_BACKPATCH_FJ,
	ACT_IF_RJ,
	ACT_IF_BACKPATCH_RJ,
	ACT_WHILE_FJ,
	ACT_WHILE_RJ,
	ACT_WHILE_BACKPATCH_FJ,
	ACT_FOR_FJ,
	ACT_SINGLE,
	ACT_FOR_RJ,
	ACT_FOR_BACKPATCH_FJ,

	SYN_MAX
};


struct Token
{
	SYN syn; // 种别码
	union
	{
		bool bval;
		char cval;
		short sval;
		int ival;
		float fval;
		int index;
	};

	Token(SYN syn) : syn(syn), ival(0) {  }              // 关键字或界符
	Token(SYN syn, int val) : syn(syn), ival(val) { }    // 整形常量或标识符
	Token(SYN syn, float val) : syn(syn), fval(val) { }  // 浮点常量
};


class LexicalScanner
{
public:
	bool scan(std::string &src);
	void reset();
	int appendSymbal(std::string &item);

	std::vector<Token> tokens;         // 扫描出的token列表
	std::vector<std::string> symbals;  // 标识符列表
};

/**
 * 查找保留字
 */
int findReserve(const char *s);

/**
 * 查找界符运算符
 */
int findOperatorOrDelimiter(const char *s);

/**
 * 查找语义动作
 */
int findActionSign(const char *s);

/*
 * 匹配保留字或界符运算符
 */
int matchPreset(const char *s, int *pLen=nullptr);

/*
 * 根据序号获取对应的保留字或界符运算符的文本
 */
const char* getPresetStr(int value);

/**
* 判断是否为语义动作
*/
bool isActionSign(int value);

/**
 * 判断是否为字母
 */
bool isLetter(char ch);

/**
 * 判断是否为数字
 */
inline bool isDigit(char ch)
{
	return ch >= '0' && ch <= '9';
}

/**
 * 处理转义字符
 */
char translate_char(char ch);

/**
 * 编译预处理，去除无用的字符和注释
 */
std::string filterSource(std::string &src);