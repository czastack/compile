#include <iostream>
#include <fstream>
#include <sstream>
#include "lexical_analysis.h"
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

	ifstream file(input_file, ios::in);
	if (file)
	{
		ostringstream os;
		os << file.rdbuf();
		string buff = os.str();
		buff = filterSource(buff);
		// cout << buff;
		LexicalScanner scaner;
		scaner.scan(buff);
		for (auto &s: scaner.symbals)
		{
			cout << s << endl;
		}
	}
	else
	{
		cout << input_file << "²»´æÔÚ" << endl;
	}
    return 0;
}
