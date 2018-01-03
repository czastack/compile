#include "common.h"
using namespace std;;

string & trim(string &s)
{
	if (!s.empty())
	{
		s.erase(s.find_last_not_of(" ") + 1);
		s.erase(0, s.find_first_not_of(" "));
	}
	return s;
}

string & erase_space(string & s)
{
	size_t index = 0;
	if (!s.empty())
	{
		string::iterator p, q, end = s.end();
		for (p = s.begin(); *p != ' ' && p != end; ++p);
		if (p != end)
		{
			q = p;
			while (q != end)
			{
				if (*q == ' ')
				{
					++q;
				}
				else
				{
					*p++ = *q++;
				}
			}
			s.erase(p, q);
		}
	}
	return s;
}
