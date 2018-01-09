#include <iostream>
#include <iomanip>
#include <sstream>
#include "syntax_analysis.h"

using namespace std;
#define DEBUG

void SyntaxAnalyzer::init(const string & grammar)
{
	istringstream iss(erase_space(string(grammar)));
	string line;
	string left;
	while (getline(iss, line))
	{
		size_t m = line.find('-');
		size_t n, end;
		if (m != -1)
		{
			left = line.substr(0, m);
			if (!VN_dic[left])
			{
				VN_dic[left] = (int)VN_set.size();
				VN_set.emplace_back(left);
			}
			WF &wf = VN_set[VN_dic[left]];
			m += 2;

			end = m;
			while ((n = line.find('|', end)) != -1)
			{
				if (line[n + 1] == '|')
				{
					end = n + 2;
					continue;
				}
				wf.insert(line.substr(m, n - m));
				end = m = n + 1;
			}
			wf.insert(line.substr(m));
		}
	}

	// 预处理产生式，把非终结符换成序号，终结符换成对于的token值
	for (auto &wf : VN_set)
	{
		for (auto &it : wf.right_src)
		{
			Formula formula; // 增加一条产生式
			for (size_t i = 0; i < it.length(); i++)
			{
				if (it[i] == '<' && i != it.length() - 1 && it.length() > 2)
				{
					// 非终结符
					// 若Ａ->B…，则将First(B)并入First(A)
					size_t j = it.find('>', i + 1); // >的位置
					if (j == -1)
					{
						cout << "文法右部不合法" << it << endl;
						return;
					}
					string vn = it.substr(i, j - i + 1);
					auto vnit = VN_dic.find(vn);
					if (vnit == VN_dic.end())
					{
						cout << "文法错误，非终结符" << vn << "没有产生式" << endl;
						return;
					}

					formula.emplace_back(true, (unsigned char)vnit->second);
					i = j;
				}
				else
				{
					int step;
					int value = matchPreset(it.data() + i, &step);
					if (value)
					{
						formula.emplace_back(false, (unsigned char)value);
						i += step - 1;
					}
					else
					{
						cout << "不支持的终结符: " << it[i] << endl;
						return;
					}
				}
			}
			wf.right.push_back(std::move(formula));
		}
		print_wf(wf);
	}

	make_first();
	make_follow();
	make_table();

	temp_count = 0;
	m_op = 0;
}

void SyntaxAnalyzer::make_first()
{
	bool *visied = new bool[VN_set.size()];
	memset(visied, 0, VN_set.size());
	for (int i = 0; i < VN_set.size(); i++)
	{
		first_dfs(i, visied);
	}
	delete[] visied;
#ifdef DEBUG
	cout << "***************FIRST集***********************";
	for (auto &wf : VN_set)
	{
		wf.print_first();
	}
#endif
}

void SyntaxAnalyzer::first_dfs(int n, bool *visied)
{
	if (visied[n])
		return;
	visied[n] = true;
	auto &wf = VN_set[n];
	for (auto &formula: wf.right)
	{
		for (auto &node: formula)
		{
			if (node.non)
			{
				// 非终结符
				// 若Ａ->B…，则将First(B)并入First(A)
				WF &wfB = VN_set[node.value];
				first_dfs(node.value, visied); // 先递归求First(B)
				bool flag = true;
				for (auto itb: wfB.first)
				{
					if (itb == SYN_EMPTY)
						flag = false;
					else
						wf.first.insert(itb);
				}
				if (flag) break; // 若Ａ->XB…或Ａ->Xa…，且X->ε
			}
			else if (!isActionSign(node.value))
			{
				wf.first.insert(node.value);
				break;
			}
		}
	}
}

void SyntaxAnalyzer::make_follow()
{	while (true)
	{
		bool goon = false;
		for (auto &wf: VN_set)
		{
			for (auto &formula : wf.right)
			{
				auto end = formula.end();
				if (!formula.empty())
				{
					while (isActionSign((end - 1)->value))
					{
						--end;
					}
				}
				for (auto pNode = formula.begin(); pNode != end; ++pNode)
				{
					if (pNode->non)
					{
						// 非终结符
						WF &wf_a = VN_set[pNode->value];
						
						if (pNode == end - 1)
						{
							// 形如U→xA的规则，则 FOLLOW(U)∈FOLLOW(A)
							if (&wf_a != &wf)
							{
								size_t before_size = wf_a.follow.size();
								wf_a.follow.insert(wf.follow.begin(), wf.follow.end());
								if (before_size < wf_a.follow.size())
								{
									goon = true;
								}
							}
						}
						else
						{
							// 若文法G[S]中有形如U→xAY, x∈V*, Y∈Vt的规则，
							// 则FIRST(Y)\ε ∈ FOLLOW(A)
							for (auto pNodeAfter = pNode + 1; pNodeAfter != end; ++pNodeAfter)
							{
								if (pNodeAfter->non)
								{
									WF &wf_y = VN_set[pNodeAfter->value];
									size_t before_size = wf_a.follow.size();
									for (auto t: wf_y.first)
									{
										if (t != SYN_EMPTY)
										{
											wf_a.follow.insert(t);
										}
									}
									if (before_size < wf_a.follow.size())
									{
										goon = true;
									}
									if (wf_y.elicit_empty())
									{
										// 形如U→xAy的规则且ε∈FIRST(y)，其中x∈V*, y∈V*, 则 FOLLOW(U)∈FOLLOW(A)
										if (&wf_a != &wf)
										{
											before_size = wf_a.follow.size();
											wf_a.follow.insert(wf.follow.begin(), wf.follow.end());

											if (before_size < wf_a.follow.size())
											{
												goon = true;
											}
										}
									}
								}
								else
								{
									if (isActionSign(pNodeAfter->value))
									{
										continue;
									}
									// 产生式形如B→xAy (y为终极符)
									// 将y并入Follow(A)
									if (wf_a.follow.find(pNodeAfter->value) == wf_a.follow.end())
									{
										wf_a.follow.insert(pNodeAfter->value);
										goon = true;
									}
								}
								break;
							}
						}
					}
				}
			}
		}
		if (!goon)
			break;
	}
	VN_set[0].follow.insert(SYN_START);

#ifdef DEBUG
	cout << "****************FOLLOW集**********************";
	for (auto &wf : VN_set)
	{
		wf.print_follow();
	}
#endif
}

VT SyntaxAnalyzer::formula_first(const Formula & formula)
{
	VT vt;
	for (auto &node : formula)
	{
		if (node.non)
		{
			// 非终结符
			// 若Ａ->B…，则将First(B)并入First(A)
			WF &wfB = VN_set[node.value];
			bool flag = true;
			for (auto itb : wfB.first)
			{
				if (itb == SYN_EMPTY)
					flag = false;
				else
					vt.insert(itb);
			}
			if (flag) break; // 若Ａ->XB…或Ａ->Xa…，且X->ε
		}
		else if (!isActionSign(node.value))
		{
			vt.insert(node.value);
			break;
		}
	}
	return vt;
}

void SyntaxAnalyzer::make_table()
{
	vector<VT::key_type> letter;
	for (auto &wf : VN_set)
	{
		auto &row = wf.predict_table;
		memset(row.data(), 0, row.size());
		for (int i = 0; i < wf.right.size(); ++i)
		{
			auto &formula = wf.right[i];

			// 判断该产生式会不会推出空
			bool make_empty = !(formula[0].non || formula[0].value != SYN_EMPTY);
			if (!make_empty)
			{
				make_empty = true;
				for (auto &node: formula)
				{
					if (node.non)
					{
						if (!VN_set[node.value].elicit_empty())
						{
							make_empty = false;
							break;
						}
					}
					else if (!isActionSign(node.value))
					{
						// 遇到非空终结符，说明不会推出空
						make_empty = false;
						break;
					}
				}
			}

			// A→α, A∈VN,α∈V*, 若α不能推导出ε,则SELECT(A→α)=FIRST(α)
			// 如果α能推导出ε则：SELECT(A→α)=（FIRST(α) –{ε}）∪FOLLOW(A)
			for (auto t: formula_first(formula))
			{
				if (t != SYN_EMPTY)
				{
					row[t] = i + 1; // 把产生式序号填入预测表
					if (find(letter.begin(), letter.end(), t) == letter.end())
					{
						letter.push_back(t);
					}
				}
			}
			if(make_empty)
			{
				for (auto t : wf.follow)
				{
					row[t] = i + 1;
					if (find(letter.begin(), letter.end(), t) == letter.end())
					{
						letter.push_back(t);
					}
				}
			}
		}
	}
#ifdef DEBUG
	/*for (auto t: letter)
	{
		cout << getPresetStr(t) << " ";
	}*/

	for (auto &wf: VN_set)
	{
		for (auto t: letter)
		{
			if (wf.predict_table[t])
			{
				cout << "M[" << wf.left << "]"
					 << "[" << getPresetStr(t) << "]=";
				print_formula(wf.right[wf.predict_table[t] - 1]);
				cout << endl;
			}
		}
	}
#endif
}

bool SyntaxAnalyzer::analyse(vector<Token> &tokens)
{
	stack<FormulaNode> stk; // 符号栈
	stk.emplace(false, SYN_START);
	stk.emplace(true, 0);
	int steps = 0; // 步骤序号
	int idx = 0; // token 序号
	auto tokenIt = tokens.begin();

	while (!stk.empty())
	{
		FormulaNode node = stk.top();
		stk.pop();
		if (!node.non)
		{
			// 终结符
			if (isActionSign(node.value))
			{
				// 语义动作
				// cout << getPresetStr(node.value) << endl;
				handleActionSign(node.value, *tokenIt);
			}
			else
			{
				// IF X ∈ Vt THEN
				// IF X = b THEN 把下一个输入符号读进b；
				//	 ELSE ERROR
				if (tokenIt == tokens.end())
				{
					if (node.value == SYN_START)
					{
						cout << "匹配成功，符号串是该文法的句子" << endl;
						// 打印四元式
						print_quads();
						return true;
					}
					else
					{
						cout << getPresetStr(node.value) << endl;
					}
				}
				else if (tokenIt->syn == node.value)
				{
					++tokenIt;
				}
				else
				{
					cout << "符号串不是该文法的句子，分析栈顶终结符" << getPresetStr(node.value)
						<< "与余串开头" << getPresetStr(tokenIt->syn) << "不一致" << endl;
					return false;
				}
			}
		}
		else
		{
			// 非终结符
			auto &wf = VN_set[node.value];
			int p = wf.predict_table[tokenIt->syn];
			if (!p)
			{
				cout << "查预测表出错, M[" << wf.left << "][" << getPresetStr(tokenIt->syn) << "] 为空" << endl;
				return false;
			}
			else
			{
				// 应用产生式
				auto &formula = wf.right[p - 1];
				/*cout << "使用产生式";
				cout << wf.left << " -> ";
				print_formula(formula);
				cout << endl;*/
				auto pNode = formula.rbegin();
				if (pNode->non || pNode->value != SYN_EMPTY)
				{
					// pNode不为空，入栈
					for (; pNode != formula.rend(); ++pNode)
					{
						stk.push(*pNode);
					}
				}
			}
		}
	}
	return false;
}

void SyntaxAnalyzer::handleActionSign(int action, const Token &token)
{
	string op, ARG1, ARG2, RES;
	switch (action)
	{
	case ACT_ADD_SUB:
		if (m_op == SYN_ADD || m_op == SYN_SUB)
		{
			ARG2 = semanticStack.top();
			semanticStack.pop();
			ARG1 = semanticStack.top();
			semanticStack.pop();
			RES = newTemp();
			newQuad(getPresetStr(m_op), ARG1.c_str(), ARG2.c_str(), RES.c_str());
			semanticStack.push(RES);
			m_op = 0;
		}
		break;
	case ACT_ADD:
		m_op = SYN_ADD;
		break;
	case ACT_SUB:
		m_op = SYN_SUB;
		break;
	case ACT_DIV_MUL:
		if (m_op == SYN_MUL || m_op == SYN_DIV)
		{
			ARG2 = semanticStack.top();
			semanticStack.pop();
			ARG1 = semanticStack.top();
			semanticStack.pop();
			RES = newTemp();
			newQuad(getPresetStr(m_op), ARG1.c_str(), ARG2.c_str(), RES.c_str());
			semanticStack.push(RES);
			m_op = 0;
		}
		break;
	case ACT_DIV:
		m_op = SYN_DIV;
		break;
	case ACT_MUL:
		m_op = SYN_MUL;
		break;
	case ACT_TRAN_LF:
		// F_value = L_value;
		break;
	case ACT_ASSIGN:
		if (token.syn == SYN_ID)
		{
			semanticStack.push(m_lex.symbals[token.index]);
		}
		else
		{
			semanticStack.push(to_string(token.ival));
		}
		break;
	case ACT_SINGLE:
		if (!for_op.empty() && for_op.top())
		{
			ARG1 = semanticStack.top();
			semanticStack.pop();
			RES = ARG1;
			newQuad(getPresetStr(for_op.top()), ARG1.c_str(), "/", RES.c_str());
			for_op.pop();
		}
		break;
	case ACT_SINGLE_OP:
		for_op.push(token.syn);
		break;
	case ACT_EQ:
		ARG1 = semanticStack.top();
		semanticStack.pop();
		RES = semanticStack.top();
		semanticStack.pop();
		newQuad(getPresetStr(SYN_ASSIGN), ARG1.c_str(), "/", RES.c_str());
		break;
	case ACT_COMPARE:
		ARG2 = semanticStack.top();
		semanticStack.pop();
		op = semanticStack.top();
		semanticStack.pop();
		ARG1 = semanticStack.top();
		semanticStack.pop();
		RES = newTemp();
		newQuad(op.c_str(), ARG1.c_str(), ARG2.c_str(), RES.c_str());
		semanticStack.push(RES);
		break;
	case ACT_COMPARE_OP:
		semanticStack.push(getPresetStr(token.syn));
		break;
	case ACT_IF_FJ:
		ARG1 = semanticStack.top();
		semanticStack.pop();
		newQuad("FJ", "", ARG1.c_str(), "/");
		fj_stack.push((int)quads.size());
		break;
	case ACT_IF_BACKPATCH_FJ:
		backpatch(fj_stack.top(), (int)quads.size() + 2);
		fj_stack.pop();
		break;
	case ACT_IF_RJ:
		newQuad("RJ", "/", "/", "/");
		rj_stack.push((int)quads.size());
		break;
	case ACT_IF_BACKPATCH_RJ:
		backpatch(rj_stack.top(), (int)quads.size() + 1);
		rj_stack.pop();
		break;
	case ACT_WHILE_FJ:
		ARG1 = semanticStack.top();
		semanticStack.pop();
		newQuad("FJ", "", ARG1.c_str(), "/");
		fj_stack.push((int)quads.size());
		break;
	case ACT_WHILE_RJ:
		ARG1 = to_string(fj_stack.top() - 1);
		newQuad("RJ", ARG1.c_str(), "/", "/");
		break;
	case ACT_WHILE_BACKPATCH_FJ:
		backpatch(fj_stack.top(), (int)quads.size() + 1);
		fj_stack.pop();
		break;
	case ACT_FOR_FJ:
		ARG1 = semanticStack.top();
		semanticStack.pop();
		newQuad("FJ", "", ARG1.c_str(), "/");
		fj_stack.push((int)quads.size());
		break;
	case ACT_FOR_RJ:
		ARG1 = to_string(fj_stack.top() - 1);
		newQuad("RJ", ARG1.c_str(), "/", "/");
		break;
	case ACT_FOR_BACKPATCH_FJ:
		backpatch(fj_stack.top(), (int)quads.size() + 1);
		fj_stack.pop();
		break;
	}
}

std::string SyntaxAnalyzer::newTemp()
{
	string result = to_string(++temp_count);
	result.insert(0, "T");
	return result;
}

Quad & SyntaxAnalyzer::newQuad(const char * op, const char * ag1, const char * ag2, const char * result)
{
	quads.emplace_back();
	auto &quad = quads.back();
	strcpy(quad.op, op);
	strcpy(quad.arg1, ag1);
	strcpy(quad.arg2, ag2);
	strcpy(quad.result, result);
	return quad;
}

void SyntaxAnalyzer::backpatch(int i, int res)
{
	sprintf(quads[i - 1].arg1, "%d", res);
}

void SyntaxAnalyzer::print_wf(const WF & wf)
{
	cout << wf.left << "->";
	auto it = wf.right.begin();
	if (it != wf.right.end())
	{
		print_formula(*it++);
	}
	for (; it != wf.right.end(); it++)
	{
		cout << "|";
		print_formula(*it);
	}
	cout << endl;
}

void SyntaxAnalyzer::print_formula(const Formula & formula)
{
	const char* text;
	for (auto &node : formula)
	{
		if (node.non)
		{
			text = VN_set[node.value].left.c_str();
		}
		else
		{
			text = getPresetStr(node.value);
		}
		if (text)
		{
			cout << text;
		}
	}
}

void SyntaxAnalyzer::print_quads()
{
	cout << "***************四元式***********************" << endl;
	int i = 0;
	for (auto &quad : quads)
	{
		cout << setw(2) << ++i << " ";
		quad.print();
	}
}


bool WF::elicit_empty()
{
	return first.find(SYN_EMPTY) != first.end();
}

void WF::print_vt(VT &vt)
{
	bool flag = false;
	for (auto t : vt)
	{
		if (flag) 
			cout << ",";
		const char* text = getPresetStr(t);
		if (text)
		{
			cout << text;
		}
		flag = true;
	}
	cout << "}";
}

void WF::print_first()
{
	cout << "FIRST(" << left.c_str() << ")={";
	print_vt(first);
}

void WF::print_follow()
{
	cout << "FOLLOW(" << left.c_str() << ")={";
	print_vt(follow);
}

void Quad::print()
{
	cout << "(" << op << ", " << arg1 << ", " << arg2 << ", " << result << ")" << endl;
}
