#include <iostream>
#include <iostream>
#include "code_gen.h"

using namespace std;

void code_gen(std::vector<Quad>& quads)
{
	cout << endl << "***************Ŀ�����***********************" << endl;
	cout << "assume cs:code, ds:data" << endl;
	vector<string> vars;
	vector<int> labels;
	string var_name;

	int line = 0;
	for (auto &quad : quads)
	{
		if (isalpha(quad.result[0]) && !(istemp(quad.result)))
		{
			// ɨ�����
			var_name = quad.result;
			if (find(vars.begin(), vars.end(), var_name) == vars.end())
			{
				vars.push_back(var_name);
			}
		}
		else if (strcmp(quad.op, "FJ") == 0 || strcmp(quad.op, "RJ") == 0)
		{
			int dest = atoi(quad.arg1);
			// ɨ����
			if (find(labels.begin(), labels.end(), dest) == labels.end())
			{
				// �����תĿ�ĵص��к�
				labels.push_back(dest);
			}
		}
	}

	sort(labels.begin(), labels.end());

	// ��������
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
			// ���
			cout << "L" << (label_it - label_begin + 1) << ": ";
		}
		if (strcmp(quad.op, "=") == 0)
		{
			// ��ֵ
			if (istemp(quad.arg1))
			{
				// �������ʱ����������Ĵ���
				cout << "    mov " << quad.result << ", " << quad.arg1 << endl;
			}
			else
			{
				// ��ax�ݴ�
				cout << "    mov ax, " << quad.arg1 << endl;
				cout << "    mov " << quad.result << ", ax" << endl;
			}
		}
		else if (strcmp(quad.op, "+") == 0)
		{
			// �ӷ�����
			if (istemp(quad.result))
			{
				// �������ʱ����������Ĵ���
				cout << "    mov " << quad.result << ", " << quad.arg1 << endl;
				cout << "    add " << quad.result << ", " << quad.arg2 << ", " << endl;
			}
			else
			{
				// ��ax�ݴ�
				cout << "    mov ax, " << quad.arg1 << endl;
				cout << "    add ax, " << quad.arg2 << ", " << endl;
				cout << "    mov " << quad.result << ", ax" << endl;
			}
		}
		else if (strcmp(quad.op, "-") == 0)
		{
			// ��������
			if (istemp(quad.result))
			{
				// �������ʱ����������Ĵ���
				cout << "    mov " << quad.result << ", " << quad.arg1 << endl;
				cout << "    add " << quad.result << ", " << quad.arg2 << ", " << endl;
			}
			else
			{
				// ��ax�ݴ�
				cout << "    mov ax, " << quad.arg1 << endl;
				cout << "    sub ax, " << quad.arg2 << ", " << endl;
				cout << "    mov " << quad.result << ", ax" << endl;
			}
		}
		else if (strcmp(quad.op, "*") == 0)
		{
			// �˷�����
			cout << "    mov ax, " << quad.arg1 << endl;
			cout << "    mul " << quad.arg2 << ", " << endl;
			cout << "    mov " << quad.result << ", ax" << endl;
		}
		else if (strcmp(quad.op, "/") == 0)
		{
			// �˷�����
			cout << "    mov ax, " << quad.arg1 << endl;
			cout << "    div " << quad.arg2 << ", " << endl;
			cout << "    mov " << quad.result << ", ax" << endl;
		}
		else if (strcmp(quad.op, "++") == 0)
		{
			// ����
			cout << "    mov ax, " << quad.arg1 << endl;
			cout << "    inc ax" << endl;
			cout << "    mov " << quad.result << ", ax" << endl;
		}
		else if (strcmp(quad.op, "--") == 0)
		{
			// �Լ�
			cout << "    mov ax, " << quad.arg1 << endl;
			cout << "    dec ax" << endl;
			cout << "    mov " << quad.result << ", ax" << endl;
		}
		// ����������ǹ�ϵ������һ��������FJ
		else if ((strcmp(quad.op, "<") == 0 || strcmp(quad.op, "<=") == 0 || strcmp(quad.op, ">") == 0 || strcmp(quad.op, ">=") == 0)
			&& strcmp(quads[line].op, "FJ") == 0)
		{
			// ��ϵ�Ƚ�����
			cout << "    cmp " << quad.arg1 << ", " << quad.arg2 << endl;
			const char* j;
			if (strcmp(quad.op, "<") == 0)
			{
				// ���ڵ���ʱ��ת (FJ������Ϊ��ʱ��ת������ȡ�෴����������һ��)
				j = "jnb";
			}
			else if (strcmp(quad.op, "<=") == 0)
			{
				// ����ʱ��ת
				j = "ja";
			}
			else if (strcmp(quad.op, ">") == 0)
			{
				// С�ڵ���ʱ��ת
				j = "jna";
			}
			else //if (strcmp(quad.op, ">=") == 0)
			{
				// С��ʱ��ת
				j = "jb";
			}
			// ��ѯ����б�arg1����Ԫʽ��ţ������б�õ������ţ�
			if ((label_it = find(label_begin, labels.end(), atoi(quads[line].arg1))) != labels.end())
			{
				// ���
				cout << "    " << j << " L" << (label_it - label_begin + 1) << endl;
			}
		}
		else if (strcmp(quad.op, "RJ") == 0)
		{
			// ��������ת
			if ((label_it = find(label_begin, labels.end(), atoi(quad.arg1))) != labels.end())
			{
				// ��ѯ����б�arg1����Ԫʽ��ţ������б�õ������ţ�
				cout << "    jmp short L" << (label_it - label_begin + 1) << endl;
			}
		}
	}
	if (!labels.empty())
	{
		// ���һ�����
		cout << "L" << labels.size() << ":" << endl;
	}
	// ���������
	cout << "code ends" << endl;
	cout << "end start" << endl;

}

bool istemp(const char * var_name)
{
	return var_name[0] == 'T' && isdigit(var_name[1]);
}
