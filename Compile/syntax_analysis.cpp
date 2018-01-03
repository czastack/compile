#include <stdio.h>
#include <iostream>
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
		size_t n;
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

			while ((n = line.find('|', m)) != -1)
			{
				wf.insert(line.substr(m, n - m));
				m = n + 1;
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
				if (it[i] == '<' && it.length() != 1)
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
}

WF& SyntaxAnalyzer::get_rule(const string & left, int *pIndex)
{
	int index = VN_dic[left];
	if (pIndex)
	{
		*pIndex = index;
	}
	return VN_set[index];
}

void SyntaxAnalyzer::make_first()
{
	memset(used, 0, sizeof(used));
	for (int i = 0; i < VN_set.size(); i++)
	{
		dfs(i);
	}
#ifdef DEBUG
	puts("***************FIRST集***********************");
	for (auto &wf : VN_set)
	{
		wf.print_first();
	}
#endif
}

void SyntaxAnalyzer::dfs(int x)
{
	if (used[x]) return;
	used[x] = true;
	auto &wf = VN_set[x];
	for (auto &formula: wf.right)
	{
		for (auto &node: formula)
		{
			if (node.non)
			{
				// 非终结符
				// 若Ａ->B…，则将First(B)并入First(A)
				WF &wfB = VN_set[node.value];
				dfs(node.value); // 先递归求First(B)
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
			else
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
				for (auto pNode = formula.begin(), end = formula.end(); pNode != end; ++pNode)
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
									if (wf_y.contains_empty())
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
	puts("****************FOLLOW集**********************");
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
		else
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
			if (formula[0].non || formula[0].value != SYN_EMPTY)
			{
				// 产生式形如A-> x THEN
				// FOR First(x)中的每个终极符a DO
				//	 置M[A][a] = 'A->x'
				for (auto t: formula_first(formula))
				{
					row[t] = i + 1; // 把产生式序号填入预测表
					if (find(letter.begin(), letter.end(), t) == letter.end())
					{
						letter.push_back(t);
					}
				}
			}
			else
			{
				// 产生式形如A-> ε THEN
				// FOR Follow(A)中的每个终极符a DO
				//	 置M[A][a] = ’ A->ε’
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

void SyntaxAnalyzer::print(int steps, stack<string> &stk, const string &src, const string &wf, int x)
{
	printf("%-10d", steps);
	string out = "";
	while (!stk.empty())
	{
		out = stk.top() + out;
		stk.pop();
	}
	printf("#%-9s", out.c_str());
	out = "";
	for (int i = x; i < src.length(); i++)
		out += src[i];
	printf("%-10s", (out + "#").c_str());
	printf("%-10s\n", wf.c_str());
}

void SyntaxAnalyzer::analyse(const string& src)
{
	/*stack<string> stk;
	stk.push("E");
	int steps = 0;
	int idx = 0;
	printf("%-10s%-10s%-10s%-10s\n", "步骤", "符号栈", "输入串", "所用产生式");
	while (!stk.empty())
	{
		string u = stk.top();
		string tmp = "";
		stk.pop();
		if (!isupper(u[0]))
		{
			if (idx == src.length() && u[0] == SYN_EMPTY);
			else if (src[idx] == u[0])
				idx++;
		}
		else
		{
			int x = VN_dic[u];
			tmp = predict_table[x][src[idx]];
			for (size_t i = tmp.length() - 1; i >= 0; i--)
			{
				if (tmp[i] == '\'')
				{
					string v = tmp.substr(i - 1, 2);
					stk.push(v);
					i--;
				}
				else
				{
					string v = tmp.substr(i, 1);
					stk.push(v);
				}
			}
			tmp = u + "->" + tmp;
		}
		print(steps++, stk, src, tmp, idx);
	}*/
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
			printf(text);
		}
	}
}


bool WF::contains_empty()
{
	for (auto &formula: right)
	{
		for (auto &node: formula)
		{
			if (!node.non && node.value == SYN_EMPTY)
			{
				return true;
			}
		}
	}
	return false;
}

void WF::print_vt(VT &vt)
{
	bool flag = false;
	for (auto t : vt)
	{
		if (flag) printf(",");
		const char* text = getPresetStr(t);
		if (text)
		{
			printf(text);
		}
		flag = true;
	}
	puts("}");
}

void WF::print_first()
{
	printf("FIRST(%s)={", left.c_str());
	print_vt(first);
}

void WF::print_follow()
{
	printf("FOLLOW(%s)={", left.c_str());
	print_vt(follow);
}
