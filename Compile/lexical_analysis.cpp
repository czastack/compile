#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include "lexical_analysis.h"
#include "common.h"
using namespace std;

// �����ֱ�
DEFINE(RESERVE_WORD) = {
	"auto", "break", "case", "char", "const", "continue",
	"default", "do", "double", "else", "enum", "extern",
	"float", "for", "goto", "if", "int", "long",
	"return", "short", "signed", "sizeof", "static",
	"struct", "switch", "typedef", "union", "unsigned", "void",
	"volatile", "while"
};

// ����������
DEFINE(OPERATOR_OR_DELIMITER) = {
	"+","-","*","/","<","<=",">",">=","=","==",
	"!=",";","(",")","^",",","\"","'","#","&",
	"&&","|","||","%","~","<<",">>","[","]","{",
	"}","\\",".","\?",":","!"
};


/**
 * ���ұ�����
 */
int findReserve(const char *s)
{
	for (size_t i = 0; i < lengthof(RESERVE_WORD); i++)
	{
		if (strcmp(RESERVE_WORD[i], s) == 0)
		{
			return i + 1; //�����ֱ���
		}
	}
	return -1; // ���Ҳ��ɹ�����Ϊ��ʶ��
}

/**
 * ���ҽ���������
 */
int findOperatorOrDelimiter(const char *s)
{
	for (size_t i = 0; i < lengthof(OPERATOR_OR_DELIMITER); i++)
	{
		if (strcmp(OPERATOR_OR_DELIMITER[i], s) == 0)
		{
			return lengthof(RESERVE_WORD) + 1 + i; //�����ֱ���
		}
	}
	return -1;
}


/**
 * �ж��Ƿ�Ϊ��ĸ
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
			// ��Ϊ����ע�ͣ���ȥ��ע�ͺ���Ķ�����ֱ�������س�����
			while (src[i] != '\n')
			{
				i++; //���ɨ��
			}
		}
		if (src[i] == '/' && src[i + 1] == '*')
		{
			// ��Ϊ����ע��"/ *...* /"����ȥ��������
			i += 2;
			while (src[i] != '*' || src[i + 1] != '/')
			{
				i++;//����ɨ��
				if (i == src.size())
				{
					printf("ע�ͳ�����û���ҵ� */\n");
					exit(0);
				}
			}
			i += 2; //���"* /"
		}
		if (src[i] != '\n' && src[i] != '\t' && src[i] != '\v' && src[i] != '\r')
		{
			//�����������ַ�������ˣ��������
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
			// ��ĸ���»��߿�ͷ��Ϊ��ʶ����ؼ���
			while (isLetter(ch) || isDigit(ch))
			{
				token_stream << ch;
				ch = src[++i];
			}
			token = token_stream.str();
			syn = findReserve(token.c_str());
			if (syn != -1)
			{
				tokens.emplace_back(syn);
			}
			else
			{
				// �����Ǳ��������Ǳ�ʶ��
				syn = SYN_IDENT;
				tokens.emplace_back(syn, appendSymbal(token));
			}
		}
		else if (isDigit(ch))
		{
			bool isfloat = false; // �Ǹ�����
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
					cout << "�ַ���û�бպ�" << endl;
					exit(1);
				}
				token_stream << ch;
			}
			token = token_stream.str();
			tokens.emplace_back(SYN_STRING, appendSymbal(token));
		}
		else
		{
			// ���ҽ���������
			token_stream << ch;
			token = token_stream.str();
			syn = findOperatorOrDelimiter(token.c_str());
			if (syn != -1)
			{
				token_stream << src[i + 1];
				token = token_stream.str();
				int syn_tmp = findOperatorOrDelimiter(token.c_str());
				if (syn_tmp != -1)
				{
					syn = syn_tmp;
					++i;
				}
				tokens.emplace_back(syn);
				++i;
			}
			else
			{
				cout << "�޷�ʶ��ķ���: " << ch << endl;
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
		index = it - symbals.begin();
	}
	return index;
}