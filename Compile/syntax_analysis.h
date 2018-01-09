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
	bool non; // 是非终结符
	unsigned char value;

	FormulaNode(bool non=false, unsigned char value=0): non(non), value(value)
	{

	}

	bool operator< (const FormulaNode& r) const
	{
		return *(short*)this < *(short*)(&r);
	}
};

typedef std::vector<FormulaNode> Formula; // 产生式右部
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

	// 该文法存在产生式会推出空
	bool elicit_empty();

	void print_vt(VT &vt);
	void print_first();
	void print_follow();
};

/*
 * 四元式
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
	std::vector<WF> VN_set; // 文法列表
	// 语义分析变量
	std::vector<Quad> quads; // 四元式列表
	std::stack<std::string> semanticStack;  // 语义栈
	std::stack<SYN> for_op;
	std::stack<int> fj_stack, rj_stack;
	int temp_count; // 临时变量计数
	int m_op;       // 当前操作符
	LexicalScanner &m_lex;

	SyntaxAnalyzer(LexicalScanner &lex): m_lex(lex) {}

	// 初始化语法分析器
	void init(const std::string& grammar); 
	// 求所有文法first集
	void make_first();
	// 求所有文法follow集
	void make_follow();
	// 递归求first集
	void first_dfs(int n, bool *visied);
	// 求一个产生式的First集
	VT formula_first(const Formula& formula);
	// 生成预测分析表
	void make_table();
	// 语法分析
	bool analyse(std::vector<Token> &tokens);
	// 打印一行文法
	void print_wf(const WF &wf);
	// 打印一条产生式
	void print_formula(const Formula &formula);
	// 打印四元式
	void print_quads();
	// 处理语义动作(语义子程序)
	void handleActionSign(int action, const Token &token);
	// 新临时变量
	std::string newTemp();
	Quad &newQuad(const char* op, const char* ag1, const char* ag2, const char* result);
	// 回填
	void backpatch(int i, int res);
};
