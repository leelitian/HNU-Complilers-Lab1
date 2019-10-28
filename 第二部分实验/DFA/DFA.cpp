#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <fstream>
using namespace std;

/*MODE为1表示使用键盘输入，MODE为2表示文件读写*/
#define MODE 1

string charSet;				//字符集
int stateNum;				//状态集大小
int startState;				//开始状态
vector<int> acceptStateSet;	//接受状态
vector<vector<int>> trans;	//状态转换表 

unordered_map<char, size_t> getCharNum;		//字符->字符编号

/*从键盘输入DFA*/
void inputDFA()
{
	charSet.clear();
	acceptStateSet.clear();
	trans.clear();
	getCharNum.clear();

	int n, state;
	cout << "请输入字符集(o代表除了前面字符的任意字符)：";
	getline(cin, charSet);
	for (size_t i = 0; i < charSet.size(); ++i)
		getCharNum[charSet[i]] = i;

	cout << "请输入状态集的大小（状态默认从0开始）：";
	cin >> stateNum;

	cout << "请输入开始状态：";
	cin >> startState;

	cout << "请输入接受状态的个数：";
	cin >> n;

	cout << "请输入接受状态集：";
	for (int i = 0; i < n; ++i)
	{
		cin >> state;
		acceptStateSet.push_back(state);
	}

	cout << "请输入状态转换表：" << endl;
	trans.assign(stateNum, vector<int>());
	for (int i = 0; i < stateNum; ++i)
		for (size_t j = 0; j < charSet.size(); ++j)
		{
			cin >> state;
			trans[i].push_back(state);
		}
}

/*从文件读入DFA*/
void readDFA(const string& path)
{
	charSet.clear();
	acceptStateSet.clear();
	trans.clear();
	getCharNum.clear();

	ifstream in(path);
	int n, state;

	getline(in, charSet);
	for (size_t i = 0; i < charSet.size(); ++i)
		getCharNum[charSet[i]] = i;

	in >> stateNum>> startState >> n;

	for (int i = 0; i < n; ++i)
	{
		in >> state;
		acceptStateSet.push_back(state);
	}

	trans.assign(stateNum, vector<int>());
	for (int i = 0; i < stateNum; ++i)
		for (size_t j = 0; j < charSet.size(); ++j)
		{
			in >> state;
			trans[i].push_back(state);
		}
	
	in.close();
}

/*将DFA输出到文件中*/
void writeDFA(const string& path)
{
	ofstream out(path);
	out << charSet << endl << stateNum << endl << acceptStateSet.size() << endl;

	for (size_t i = 0; i < acceptStateSet.size(); ++i)
		out << acceptStateSet[i] << " ";

	out << endl;
	for (int i = 0; i < stateNum; ++i)
	{
		for (size_t j = 0; j < charSet.size(); ++j)
			out << trans[i][j] << " ";
		out << endl;
	}

	out.close();
}

/*判断是否为接受状态*/
bool isAcceptState(int state)
{
	for (size_t i = 0; i < acceptStateSet.size(); ++i)
		if (state == acceptStateSet[i]) return true;

	return false;
}

/*判断DFA是否合法*/
bool isLegalDFA()
{
	//字符集重复元素检查
	for (size_t i = 0; i < charSet.size(); ++i)
		for (size_t j = i + 1; j < charSet.size(); ++j)
			if (charSet[i] == charSet[j]) return false;

	//检查初始状态是否属于状态集
	if (startState < 0 || startState >= stateNum) return false;

	//检查接受态是否为空集
	if (acceptStateSet.empty()) return false;

	//检查接受状态集是否为状态集的子集
	for (size_t i = 0; i < acceptStateSet.size(); ++i)
		if (acceptStateSet[i] < 0 || acceptStateSet[i] >= stateNum) return false;

	//对状态转移表的检查
	for (size_t i = 0; i < trans.size(); ++i)
	{
		for (size_t j = 0; j < charSet.size(); ++j)
		{
			if (trans[i][j] < 0) continue;
			//不能到达一个不存在的状态
			if (trans[i][j] < 0 || trans[i][j] >= stateNum) return false;
		}
	}

	return true;
}

/*判断字符串是否能被DFA接受*/
bool isLegalString(string str)
{
	//如果出现了字符集合以外的字符，替换为'o'
	for (size_t i = 0; i < str.length(); ++i)
		if (charSet.find(str[i]) == string::npos)
			str[i] = 'o';

	//状态转换
	int curState = startState;
	cout << "状态转移情况：" << curState;
	for (size_t i = 0; i < str.length(); ++i)
	{
		//如果读入字符不在字符集中，将到达出错状态-1
		if (getCharNum.find(str[i]) == getCharNum.end()) curState = -1;
		else curState = trans[curState][getCharNum[str[i]]];
		cout << "->" << curState;
		if (curState == -1) return false;
	}

	return isAcceptState(curState);
}

/*深搜：找到长度为maxLength的可接受字符串*/
void dfs(int curState, int curLength, int maxLength, string& str)
{
	if (curLength == maxLength)
	{
		if (isAcceptState(curState))
			cout << (str.empty() ? ("ε") : str) << endl;
		return;
	}

	for (int j = 0; j < charSet.size(); ++j)
	{
		if (trans[curState][j] != -1)
		{
			str += charSet[j];
			dfs(trans[curState][j], curLength + 1, maxLength, str);
			str.pop_back();
		}
	}
}

/*打印出所有长度不超过N的可接受字符串*/
void getString(int N)
{
	string str = "";
	for (int maxLength = 0; maxLength < N + 1; ++maxLength)
		dfs(startState, 0, maxLength, str);
}

int main()
{
	if (MODE == 1)
	{
		inputDFA();
		writeDFA("out.dfa");
	}
	else readDFA("3.dfa");

	if (!isLegalDFA())
	{
		cout << "该DFA是非法的" << endl;
		return 0;
	}
	else cout << "该DFA是合法的" << endl;

	int N;
	cout << "请输入字符串的最大长度N：";
	cin >> N;
	getString(N);

	cout << "请输入待检查的字符串：" << endl;
	string str;
	while (cin >> str)
	{
		if (isLegalString(str))
			cout << "，可接受" << endl;
		else cout << "，不可接受" << endl;
	}

	system("pause");
	return 0;
}
