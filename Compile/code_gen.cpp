#include <iostream>
#include <iostream>
#include "code_gen.h"

using namespace std;

void code_gen(std::vector<Quad>& quads)
{
	cout << endl << "***************目标代码***********************" << endl;
	cout << "assume cs:code, ds:data" << endl;
	vector<string> vars;
	vector<int> labels;
	string var_name;

	int line = 0;
	for (auto &quad : quads)
	{
		if (isalpha(quad.result[0]) && !(istemp(quad.result)))
		{
			// 扫描变量
			var_name = quad.result;
			if (find(vars.begin(), vars.end(), var_name) == vars.end())
			{
				vars.push_back(var_name);
			}
		}
		else if (strcmp(quad.op, "FJ") == 0 || strcmp(quad.op, "RJ") == 0)
		{
			int dest = atoi(quad.arg1);
			// 扫描标号
			if (find(labels.begin(), labels.end(), dest) == labels.end())
			{
				// 添加跳转目的地的行号
				labels.push_back(dest);
			}
		}
	}

	sort(labels.begin(), labels.end());

	// 变量声明
	for (auto &item : vars)
	{
		cout << "    " << item << " dd 0" << endl;
	}

	cout << "data ends\n"
		"code segment start:\n"
		"start:\n"
		"    mov ax, data\n"
		"    mov ds, ax" << endl;

	vector<int>::iterator label_it, label_begin = labels.begin();
	for (auto &quad : quads)
	{
		++line;
		if ((label_it = find(label_begin, labels.end(), line)) != labels.end())
		{
			// 标号
			cout << "L" << (label_it - label_begin + 1) << ": ";
		}
		if (strcmp(quad.op, "=") == 0)
		{
			// 赋值
			if (istemp(quad.arg1))
			{
				// 如果是临时变量，代替寄存器
				cout << "    mov " << quad.result << ", " << quad.arg1 << endl;
			}
			else
			{
				// 拿ax暂存
				cout << "    mov ax, " << quad.arg1 << endl;
				cout << "    mov " << quad.result << ", ax" << endl;
			}
		}
		else if (strcmp(quad.op, "+") == 0)
		{
			// 加法操作
			if (istemp(quad.result))
			{
				// 如果是临时变量，代替寄存器
				cout << "    mov " << quad.result << ", " << quad.arg1 << endl;
				cout << "    add " << quad.result << ", " << quad.arg2 << ", " << endl;
			}
			else
			{
				// 拿ax暂存
				cout << "    mov ax, " << quad.arg1 << endl;
				cout << "    add ax, " << quad.arg2 << ", " << endl;
				cout << "    mov " << quad.result << ", ax" << endl;
			}
		}
		else if (strcmp(quad.op, "-") == 0)
		{
			// 减法操作
			if (istemp(quad.result))
			{
				// 如果是临时变量，代替寄存器
				cout << "    mov " << quad.result << ", " << quad.arg1 << endl;
				cout << "    add " << quad.result << ", " << quad.arg2 << ", " << endl;
			}
			else
			{
				// 拿ax暂存
				cout << "    mov ax, " << quad.arg1 << endl;
				cout << "    sub ax, " << quad.arg2 << ", " << endl;
				cout << "    mov " << quad.result << ", ax" << endl;
			}
		}
		else if (strcmp(quad.op, "*") == 0)
		{
			// 乘法操作
			cout << "    mov ax, " << quad.arg1 << endl;
			cout << "    mul " << quad.arg2 << ", " << endl;
			cout << "    mov " << quad.result << ", ax" << endl;
		}
		else if (strcmp(quad.op, "/") == 0)
		{
			// 乘法操作
			cout << "    mov ax, " << quad.arg1 << endl;
			cout << "    div " << quad.arg2 << ", " << endl;
			cout << "    mov " << quad.result << ", ax" << endl;
		}
		else if (strcmp(quad.op, "++") == 0)
		{
			// 自增
			cout << "    mov ax, " << quad.arg1 << endl;
			cout << "    inc ax" << endl;
			cout << "    mov " << quad.result << ", ax" << endl;
		}
		else if (strcmp(quad.op, "--") == 0)
		{
			// 自减
			cout << "    mov ax, " << quad.arg1 << endl;
			cout << "    dec ax" << endl;
			cout << "    mov " << quad.result << ", ax" << endl;
		}
		// 如果操作符是关系，且下一个操作是FJ
		else if ((strcmp(quad.op, "<") == 0 || strcmp(quad.op, "<=") == 0 || strcmp(quad.op, ">") == 0 || strcmp(quad.op, ">=") == 0)
			&& strcmp(quads[line].op, "FJ") == 0)
		{
			// 关系比较运算
			cout << "    cmp " << quad.arg1 << ", " << quad.arg2 << endl;
			const char* j;
			if (strcmp(quad.op, "<") == 0)
			{
				// 大于等于时跳转 (FJ是条件为假时跳转，所以取相反条件，下面一样)
				j = "jnb";
			}
			else if (strcmp(quad.op, "<=") == 0)
			{
				// 大于时跳转
				j = "ja";
			}
			else if (strcmp(quad.op, ">") == 0)
			{
				// 小于等于时跳转
				j = "jna";
			}
			else //if (strcmp(quad.op, ">=") == 0)
			{
				// 小于时跳转
				j = "jb";
			}
			// 查询标号列表（arg1是四元式序号，查找列表得到标号序号）
			if ((label_it = find(label_begin, labels.end(), atoi(quads[line].arg1))) != labels.end())
			{
				// 标号
				cout << "    " << j << " L" << (label_it - label_begin + 1) << endl;
			}
		}
		else if (strcmp(quad.op, "RJ") == 0)
		{
			// 无条件跳转
			if ((label_it = find(label_begin, labels.end(), atoi(quad.arg1))) != labels.end())
			{
				// 查询标号列表（arg1是四元式序号，查找列表得到标号序号）
				cout << "    jmp short L" << (label_it - label_begin + 1) << endl;
			}
		}
	}
	if (!labels.empty())
	{
		// 最后一个标号
		cout << "L" << labels.size() << ":" << endl;
	}
	// 汇编程序结束
	cout << "code ends" << endl;
	cout << "end start" << endl;

}

bool istemp(const char * var_name)
{
	return var_name[0] == 'T' && isdigit(var_name[1]);
}
