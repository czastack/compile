#include <iostream>
#include <fstream>
#include <sstream>
#include "lexical_analysis.h"
#include "syntax_analysis.h"
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

	SyntaxAnalyzer syntax;
	ifstream grammar_file("grammar.txt", ios::in);
	if (grammar_file)
	{
		ostringstream os;
		os << grammar_file.rdbuf();
		string grammar = os.str();
		syntax.init(grammar);

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
			lexical.scan(buff);
			syntax.analyse(lexical.tokens);
		}
		else
		{
			cout << input_file << "������" << endl;
		}
	}
	else
	{
		cout << "�ķ��ļ�grammar.txt������" << endl;
	}

    return 0;
}
