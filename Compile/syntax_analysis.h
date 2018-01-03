#pragma once

#include <string>
#include <vector>
#include <array>
#include <map>
#include <set>
#include <stack>
#include "common.h"
#include "lexical_analysis.h"

#define MAX 120


struct FormulaNode
{
	bool non; // �Ƿ��ս��
	unsigned char value;

	FormulaNode(bool non=false, unsigned char value=0): non(non), value(value)
	{

	}

	bool operator< (const FormulaNode& r) const
	{
		return *(short*)this < *(short*)(&r);
	}
};

typedef std::vector<FormulaNode> Formula; // ����ʽ�Ҳ�
typedef std::set<unsigned char> VT;

class WF
{
public:
	std::string left;
	std::set<std::string> right_src;
	std::vector<Formula> right;
	VT first, follow;
	std::array<VT::key_type, SYN_MAX> predict_table;

	WF(const std::string &s)
	{
		left = s;
	}

	void insert(const std::string &s)
	{
		right_src.insert(s);
	}

	// ���Ƴ���
	bool contains_empty();

	void print_vt(VT &vt);
	void print_first();
	void print_follow();
};


class SyntaxAnalyzer
{
public:
	std::map<std::string, int> VN_dic;
	std::vector<WF> VN_set;
	bool used[MAX];
	void init(const std::string& grammar);
	// �����ķ��󲿻�ȡ�ķ�����
	WF& get_rule(const std::string& left, int *pIndex=nullptr);
	void make_first();
	void make_follow();
	void dfs(int x);
	// ��һ������ʽ��First��
	VT formula_first(const Formula& formula);
	void make_table();
	void analyse(std::vector<Token> &tokens);
	void print_wf(const WF &wf);
	void print_formula(const Formula &formula);
};
