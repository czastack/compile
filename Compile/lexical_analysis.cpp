#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "lexical_analysis.h"
#include "common.h"
using namespace std;

// 保留字表
DEFINE(RESERVE_WORD) = {
	"auto", "break", "case", "char", "const", "continue",
	"default", "do", "double", "else", "enum", "extern",
	"float", "for", "goto", "if", "int", "long",
	"return", "short", "signed", "sizeof", "static",
	"struct", "switch", "typedef", "union", "unsigned", "void",
	"volatile", "while",
	"describe", "type", "id", "digit", "string", "$", "#" // 为语法分析预留的
};

// 界符运算符表
DEFINE(OPERATOR_OR_DELIMITER) = {
	"->","+","-","*","/","%","<<",">>","<=",">=","==","!=","<",">","=",
	";","(",")","^",",","\"","'","#",
	"&&","||","&","|","~","[","]","{",
	"}","\\",".","\?",":","!"
};


/**
 * 查找保留字
 */
int findReserve(const char *s)
{
	for (size_t i = 0; i < RESERVE_WORD_LEN; i++)
	{
		if (strcmp(RESERVE_WORD[i], s) == 0)
		{
			return (int)(i + 1); //返回种别码
		}
	}
	return 0; // 查找不成功，即为标识符 0同时可以代表空($)
}

/**
 * 查找界符运算符字
 */
int findOperatorOrDelimiter(const char *s)
{
	for (size_t i = 0; i < OPERATOR_OR_DELIMITER_LEN; i++)
	{
		if (strcmp(OPERATOR_OR_DELIMITER[i], s) == 0)
		{
			return (int)(RESERVE_WORD_LEN + 1 + i); //返回种别码
		}
	}
	return 0;
}

int matchPreset(const char * s, int *pLen)
{
	// 查找保留字
	size_t len;
	for (size_t i = 0; i < RESERVE_WORD_LEN; i++)
	{
		if (strncmp(RESERVE_WORD[i], s, (len = strlen(RESERVE_WORD[i]))) == 0)
		{
			if (pLen)
			{
				*pLen = (int)len;
			}
			return (int)(i + 1); // 返回种别码
		}
	}
	// 查找界符运算符字
	for (size_t i = 0; i < OPERATOR_OR_DELIMITER_LEN; i++)
	{
		if (strncmp(OPERATOR_OR_DELIMITER[i], s, (len = strlen(OPERATOR_OR_DELIMITER[i]))) == 0)
		{
			if (pLen)
			{
				*pLen = (int)len;
			}
			return (int)(RESERVE_WORD_LEN + 1 + i); //返回种别码
		}
	}
	return 0;
}

const char * getPresetStr(int value)
{
	if (value <= RESERVE_WORD_LEN)
	{
		return RESERVE_WORD[value - 1];
	}
	else if (value <= (RESERVE_WORD_LEN + OPERATOR_OR_DELIMITER_LEN))
	{
		return OPERATOR_OR_DELIMITER[value - RESERVE_WORD_LEN - 1];
	}
	return nullptr;
}


/**
 * 判断是否为字母
 */
bool isLetter(char ch)
{
	return (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch == '_');
}

char translate_char(char ch)
{
	switch (ch)
	{
	case 't': ch = '\t'; break;
	case 'n': ch = '\n'; break;
	default:
		break;
	}
	return ch;
}

string filterSource(string &src)
{
	src.append("\n");
	ostringstream os;
	for (size_t i = 0; i <= src.size(); i++)
	{
		if (src[i] == '/' && src[i + 1] == '/')
		{
			// 若为单行注释，则去除注释后面的东西，直至遇到回车换行
			while (src[i] != '\n')
			{
				i++; //向后扫描
			}
		}
		if (src[i] == '/' && src[i + 1] == '*')
		{
			// 若为多行注释"/ *...* /"，则去除该内容
			i += 2;
			while (src[i] != '*' || src[i + 1] != '/')
			{
				i++;//继续扫描
				if (i == src.size())
				{
					printf("注释出错，没有找到 */\n");
					exit(0);
				}
			}
			i += 2; //跨过"* /"
		}
		if (src[i] != '\n' && src[i] != '\t' && src[i] != '\v' && src[i] != '\r')
		{
			//若出现无用字符，则过滤；否则加载
			os << src[i];
		}
	}
	
	return os.str();
}

void LexicalScanner::scan(std::string & src)
{
	ostringstream token_stream;
	string token;
	char ch;
	int syn;
	for (size_t i = 0; i < src.size();)
	{
		while ((ch = src[i]) == ' ' || ch == '\t')
		{
			++i;
		};

		token_stream.str("");

		if (isLetter(ch))
		{
			// 字母或下划线开头，为标识符或关键字
			while (isLetter(ch) || isDigit(ch))
			{
				token_stream << ch;
				ch = src[++i];
			}
			token = token_stream.str();
			syn = findReserve(token.c_str());
			if (syn)
			{
				tokens.emplace_back(syn);
			}
			else
			{
				// 若不是保留字则是标识符
				syn = SYN_IDENT;
				tokens.emplace_back(syn, appendSymbal(token));
			}
		}
		else if (isDigit(ch))
		{
			bool isfloat = false; // 是浮点数
			while (true)
			{
				token_stream << ch;
				ch = src[++i];
				if (ch == '.')
				{
					isfloat = true;
				}
				else if (!isDigit(ch))
				{
					break;
				}
			}
			token = token_stream.str();
			if (isfloat)
			{
				tokens.emplace_back(SYN_FNUM, (float)atof(token.c_str()));
			}
			else
			{
				tokens.emplace_back(SYN_INUM, atoi(token.c_str()));
			}
		}
		else if (ch == '\'')
		{
			ch = src[++i];
			if (ch == '\\')
			{
				ch = translate_char(src[++i]);
			}
			if (src[++i] != '\'')
			{
				cout << "expected \"'\", found " << src[i] << endl;
				exit(1);
			}
			++i;
			tokens.emplace_back(SYN_CNUM, ch);
		}
		else if (ch == '"')
		{
			while (true)
			{
				ch = src[++i];
				if (ch == '\\')
				{
					ch = translate_char(src[++i]);
				}
				else if (ch == '"')
				{
					++i;
					break;
				}
				else if (ch == '\n')
				{
					cout << "字符串没有闭合" << endl;
					exit(1);
				}
				token_stream << ch;
			}
			token = token_stream.str();
			tokens.emplace_back(SYN_STRING, appendSymbal(token));
		}
		else
		{
			// 查找界符或操作符
			token_stream << ch;
			token = token_stream.str();
			syn = findOperatorOrDelimiter(token.c_str());
			if (syn)
			{
				token_stream << src[i + 1];
				token = token_stream.str();
				int syn_tmp = findOperatorOrDelimiter(token.c_str());
				if (syn_tmp)
				{
					syn = syn_tmp;
					++i;
				}
				tokens.emplace_back(syn);
				++i;
			}
			else
			{
				cout << "无法识别的符号: " << ch << endl;
				return;
			}
		}
	}
}

void LexicalScanner::reset()
{
	tokens.clear();
	symbals.clear();
}

int LexicalScanner::appendSymbal(std::string &item)
{
	int index;
	auto it = find(symbals.begin(), symbals.end(), item);
	if (it == symbals.end())
	{
		index = (int)symbals.size();
		symbals.emplace_back(item);
	}
	else
	{
		index = (int)(it - symbals.begin());
	}
	return index;
}
