#include <iostream>
#include <fstream>
#include <sstream>
#include "lexical_analysis.h"
#include "syntax_analysis.h"
#include "code_gen.h"
using namespace std;


int main(int argc, char *argv[])
{
	char *input_file;
	if (argc == 2)
	{
		input_file = argv[1];
	}
	else
	{
		input_file = "src.cpp";
	}

	// ����Ŀ��Դ��
	ifstream file(input_file, ios::in);
	if (file)
	{
		ostringstream os;
		os << file.rdbuf();
		string buff = os.str();
		buff = filterSource(buff);
		// cout << buff;
		LexicalScanner lexical;
		if (lexical.scan(buff))
		{

			SyntaxAnalyzer syntax(lexical);
			ifstream grammar_file("grammar.txt", ios::in);
			if (grammar_file)
			{
				ostringstream os;
				os << grammar_file.rdbuf();
				string grammar = os.str();
				syntax.init(grammar);

				if (syntax.analyse(lexical.tokens))
				{
					// �﷨����������ɹ�����ʼĿ���������
					code_gen(syntax.quads);
				}
			}
			else
			{
				cout << "�ķ��ļ�grammar.txt������" << endl;
			}
		}
	}
	else
	{
		cout << input_file << "������" << endl;
	}

    return 0;
}
