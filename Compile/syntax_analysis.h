#pragma once

#include <string>
#include <vector>
#include <array>
#include <map>
#include <set>
#include <stack>
#include "common.h"
#include "lexical_analysis.h"


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

	// ���ķ����ڲ���ʽ���Ƴ���
	bool elicit_empty();

	void print_vt(VT &vt);
	void print_first();
	void print_follow();
};

/*
 * ��Ԫʽ
 */
struct Quad
{
	char arg1[8];
	char arg2[8];
	char op[8];
	char result[8];

	Quad() {}

	void print();
};

class SyntaxAnalyzer
{
public:
	std::map<std::string, int> VN_dic;
	std::vector<WF> VN_set; // �ķ��б�
	// �����������
	std::vector<Quad> quads; // ��Ԫʽ�б�
	std::stack<std::string> semanticStack;  // ����ջ
	std::stack<SYN> for_op;
	std::stack<int> fj_stack, rj_stack;
	int temp_count; // ��ʱ��������
	int m_op;       // ��ǰ������
	LexicalScanner &m_lex;

	SyntaxAnalyzer(LexicalScanner &lex): m_lex(lex) {}

	// ��ʼ���﷨������
	void init(const std::string& grammar); 
	// �������ķ�first��
	void make_first();
	// �������ķ�follow��
	void make_follow();
	// �ݹ���first��
	void first_dfs(int n, bool *visied);
	// ��һ������ʽ��First��
	VT formula_first(const Formula& formula);
	// ����Ԥ�������
	void make_table();
	// �﷨����
	bool analyse(std::vector<Token> &tokens);
	// ��ӡһ���ķ�
	void print_wf(const WF &wf);
	// ��ӡһ������ʽ
	void print_formula(const Formula &formula);
	// ��ӡ��Ԫʽ
	void print_quads();
	// �������嶯��(�����ӳ���)
	void handleActionSign(int action, const Token &token);
	// ����ʱ����
	std::string newTemp();
	Quad &newQuad(const char* op, const char* ag1, const char* ag2, const char* result);
	// ����
	void backpatch(int i, int res);
};
