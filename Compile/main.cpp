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

	// 读入目标源码
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
					// 语法、语义分析成功，开始目标代码生成
					code_gen(syntax.quads);
				}
			}
			else
			{
				cout << "文法文件grammar.txt不存在" << endl;
			}
		}
	}
	else
	{
		cout << input_file << "不存在" << endl;
	}

    return 0;
}
