/**
 * 词法分析
 */

#pragma once

#include <string>
#include <vector>

// 保留字表
extern char RESERVE_WORD[31][10];

// 界符运算符表
extern char OPERATOR_OR_DELIMITER[36][3];

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

	SYN_ADD,
	SYN_SUB,
	SYN_MUL,
	SYN_DIV,
	SYN_LT,
	SYN_LTE,
	SYN_GT,
	SYN_GTE,
	SYN_EQ,
	SYN_ASSIGN,
	SYN_NE,
	SYN_SEMICOLON,
	SYN_LEFT_BRACKET,
	SYN_RIGHT_BRACKET,
	SYN_CAP,
	SYN_COMMA,
	SYN_DQUOTE,
	SYN_SQUOTE,
	SYN_SHARP,
	SYN_AND,
	SYN_LOGICAL_AND,
	SYN_OR,
	SYN_LOGICAL_OR,
	SYN_MOD,
	SYN_TILDE,
	SYN_LEFT_SHIFT,
	SYN_RIGHT_SHIFT,
	SYN_LEFT_SQUARE_BRACKET,
	SYN_RIGHT_SQUARE_BRACKET,
	SYN_LEFT_CURLY_BRACKET,
	SYN_RIGHT_CURLY_BRACKET,
	SYN_BACKSLASH,
	SYN_DOT,
	SYN_QUESTIONMARK,
	SYN_COLON,
	SYN_NOT,

	SYN_CNUM, // char 常量
	SYN_INUM, // int 常量
	SYN_FNUM, // float 常量
	SYN_STRING,
	SYN_IDENT,
};


struct Token
{
	int syn; // 种别码
	union
	{
		bool bval;
		char cval;
		short sval;
		int ival;
		float fval;
		int index;
	};

	Token(int syn) : syn(syn), ival(0) {  }              // 关键字或界符
	Token(int syn, int val) : syn(syn), ival(val) { }    // 整形常量或标识符
	Token(int syn, float val) : syn(syn), fval(val) { }  // 浮点常量
};


class LexicalScanner
{
public:
	void scan(std::string &src);
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
 * 查找界符运算符字
 */
int findOperatorOrDelimiter(const char *s);

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
 * 编译预处理，取出无用的字符和注释
 */
std::string filterSource(std::string &src);