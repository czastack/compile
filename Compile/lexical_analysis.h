/**
 * �ʷ�����
 */

#pragma once

#include <string>
#include <vector>

// �����ֱ�
extern char RESERVE_WORD[31][10];

// ����������
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

	SYN_CNUM, // char ����
	SYN_INUM, // int ����
	SYN_FNUM, // float ����
	SYN_STRING,
	SYN_IDENT,
};


struct Token
{
	int syn; // �ֱ���
	union
	{
		bool bval;
		char cval;
		short sval;
		int ival;
		float fval;
		int index;
	};

	Token(int syn) : syn(syn), ival(0) {  }              // �ؼ��ֻ���
	Token(int syn, int val) : syn(syn), ival(val) { }    // ���γ������ʶ��
	Token(int syn, float val) : syn(syn), fval(val) { }  // ���㳣��
};


class LexicalScanner
{
public:
	void scan(std::string &src);
	void reset();
	int appendSymbal(std::string &item);

	std::vector<Token> tokens;         // ɨ�����token�б�
	std::vector<std::string> symbals;  // ��ʶ���б�
};

/**
 * ���ұ�����
 */
int findReserve(const char *s);

/**
 * ���ҽ���������
 */
int findOperatorOrDelimiter(const char *s);

/**
 * �ж��Ƿ�Ϊ��ĸ
 */
bool isLetter(char ch);

/**
 * �ж��Ƿ�Ϊ����
 */
inline bool isDigit(char ch)
{
	return ch >= '0' && ch <= '9';
}

/**
 * ����ת���ַ�
 */
char translate_char(char ch);

/**
 * ����Ԥ����ȡ�����õ��ַ���ע��
 */
std::string filterSource(std::string &src);