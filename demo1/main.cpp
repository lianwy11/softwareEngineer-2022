#include <fstream>
#include <cstring>
#include <iostream>
#include <string>
#include <regex>
#include <stack>
#include <vector>
#include "myfunc.h"

using namespace std;

// KeyNum 为关键词表的关键词总数，SwitchIdx 为 switch 关键词在表中的索引
const int KeyNum = 32;
const int SwitchIdx = 25;
// TotalNum 为关键词总数， IfElsNum 为 if-else 总数， ElsIfNum 为 if-elseif-else总数
int TotalNum = 0, IfElsNum = 0, ElsIfNum = 0;


// 关键词表，含关键词及其出现次数
struct Key {
    string word;
    int num;
}   key[KeyNum] = {
    "auto", 0, "break", 0,
    "case", 0, "char", 0,
    "const", 0, "continue", 0,
    "default", 0, "do", 0,
    "double", 0, "else", 0,
    "enum", 0, "extern", 0,
    "float", 0, "for", 0,
    "goto", 0, "if", 0,
    "int", 0, "long", 0,
    "register", 0, "return", 0,
    "short", 0, "signed", 0,
    "sizeof", 0, "static", 0,
    "struct", 0, "switch", 0,
    "typedef", 0, "union", 0,
    "unsigned", 0, "void", 0,
    "volatile", 0, "while", 0
};

// OpenMyFile 根据传入的文件路径打开文件
ifstream OpenMyFile(const string& fp) {
    ifstream testFile(fp);
    if (!testFile.is_open())
    {
        cout << "file open failed\n";
        exit(0);
    }
    return testFile;
}

// BinSearch 对传入字符串的每个词进行关键词表的二分查找
void BinSearch(int index, int len, const string& str) {
    string wd;
    if (index == 0)
    {
        if (isalpha(str[index]))
        {
            wd += str[index];
        }
        index = 1;
    }
    for (int i = index; i < len; i++)
    {
        if (isalpha(str[i])) // 拼接单词
        {
            wd += str[i];
        }
        else if (isalpha(str[i - 1]) && !isalpha(str[i])) // 找到一个单词
        {
            // 对 wd 进行关键词表的二分查找
            int high = KeyNum - 1, low = 0, cond;
            const char* s1 = wd.data();
            while (low <= high)
            {
                int mid = (high + low) / 2;
                const char* s2 = key[mid].word.data();
                if ((cond = (strcmp(s1, s2))) < 0)  // 关键词表按字典序排列！
                {
                    high = mid - 1;
                }
                else if (cond > 0)
                {
                    low = mid + 1;
                }
                else
                {
                    if (key[mid].word != wd)
                    {
                        break;
                    }
                    key[mid].num++;
                    break;
                }
            }
            wd = "";  // wd 设置为空，重新开始下一个单词的拼接
        }
    }
}

// DeleSingle 删除传入字符串中//类型的单行注释以及"xxx"类型的单行字符串
// 并去除else if中间的空格，替换为elseif
string DeleSingle(const string& str) {
    string temp = str;
    int len = temp.length(), i = 0;
    while (i < len)
    {
        if (temp[i] == '/' && temp[i + 1] == '/') // 遇到 //类型注释
        {
            temp.erase(i, len - i);
            return temp;
        }
        i++;
    }
    len = temp.length();
    i = 0;
    int st = 0;
    bool Flag = false;
    while (i < len)
    {
        if (Flag && temp[i] == '\"')  // 说明遇到 "xx"，删除这部分内容
        {
            Flag = false;
            temp.erase(st, i - st + 1);
            i = st;
            len = temp.length() - 1;  // 注意删除后字符串总长度变化，需重新赋值
            continue;
        }
        if (temp[i] == '\"')
        {
            st = i;
            Flag = true;  // 表示遇到了第一个"
        }
        i++;
    }
    return temp;
}

// DeleMuch 删除文本中/* */类型的注释
string DeleMuch(const string& str) {
    string temp = str;
    int st = 0, ed = 0, len = temp.length() - 1;
    bool stFlag = false, edFlag = false;
    int i = 0;
    while (i < len)
    {
        if (temp[i] == '/' && temp[i + 1] == '*')
        {
            st = i;
            stFlag = true;  // 表示遇到了 /*
        }
        if (temp[i] == '*' && temp[i + 1] == '/')
        {
            ed = i + 2;
            edFlag = true;  // 表示遇到了 */
        }
        if (stFlag && edFlag)  // 说明遇到 /* */ 注释，删除这部分内容
        {
            stFlag = edFlag = false;
            temp.erase(st, ed - st);
            i = ed = st - 1;
            len = temp.length() - 1;  // 注意删除后字符串总长度变化，需重新赋值
        }
        i++;
    }
    return temp;
}

// Count_Key_Num 计算关键词总数
string Count_Key_Num(const string& str) {
    int start_index = 0, end_index;;
    string NewStr = DeleMuch(str);
    end_index = NewStr.length();
    BinSearch(start_index, end_index, NewStr);
    regex z("else if");
    NewStr = regex_replace(NewStr, z, "elseif");
    for (int i = 0; i < KeyNum; i++)
    {
        if (key[i].num != 0)
        {
            TotalNum += key[i].num;
            // cout << key[i].word << " num: " << key[i].num << endl;
        }
    }
    cout << "total num: " << TotalNum << endl;
    // cout << NewStr << endl;
    return NewStr;
}

// Count_SwiCase_Num 计算关键词总数及 switch case 数量
string Count_SwiCase_Num(const string& str) {
    string NewStr = Count_Key_Num(str);
    int len = NewStr.length(), index = 0, cnt;
    stack<int> IdxStack;
    // find 函数找不到时会返回一个 string::npos ,表示-1或4294967295
    // 我这里是-1，所以找不到的情况需要判断 index != -1
    while ((index = NewStr.find("switch", index)) < len && (index != -1)) // 找到switch的位置并入栈
    {
        IdxStack.push(index);
        index++;
        // cout << "index:" << index << endl;
    }
    printf("switch num: %d\n", key[SwitchIdx].num);
    printf("case num:");
    vector<int> CaseNum;
    while (!IdxStack.empty())
    {
        cnt = 0, index = IdxStack.top() + 5; // 从后往前出栈，根据 switch 的位置每次统计一个 switch 的 case 数量
        while ((index = NewStr.find("case", index)) < len && (index != -1))
        {
            cnt++;
            index++;
        }
        len = IdxStack.top();
        IdxStack.pop();
        CaseNum.push_back(cnt);  // 结果加入 vector 容器
    }
    for (int i = CaseNum.size() - 1; i >= 0; i--)  // 倒序输出
    {
        printf(" %d", CaseNum[i]);
    }
    putchar('\n');
    return NewStr;
}

// Count_IfEls_Num 计算关键词总数，switch case 数量，
// if else数量以及根据传入的 level 选择是否输出if-elseif-else数量
void Count_IfEls_Num(const string& str, int level) {
    string NewStr = Count_SwiCase_Num(str);
    string wd;
    stack<string> IfStack;
    int len = NewStr.length();
    if (isalpha(NewStr[0]))
    {
        wd += NewStr[0];
    }
    for (int i = 1; i < len; i++)
    {
        if (isalpha(NewStr[i]))
        {
            wd += NewStr[i];
        }
        else if (isalpha(NewStr[i - 1]) && !isalpha(NewStr[i]))
        {
            // 遇到 if elseif 入栈，遇到 else 出栈并根据情况计数
            if (wd == "if" || wd == "elseif")
            {
                IfStack.push(wd);
            }
            else if (wd == "else")
            {
                if (IfStack.top() == "elseif")
                {
                    ElsIfNum++;
                    while (IfStack.top() == "elseif")
                    {
                        IfStack.pop();
                    }
                }
                else
                {
                    IfElsNum++;
                }
                IfStack.pop();
            }
            wd = "";
        }
    }
    cout << "if-else num: " << IfElsNum << endl;
    if (level == 4)
    {
        cout << "if-elseif-else num: " << ElsIfNum << endl;
    }
    return;
}

// 根据传入的等级选择需要使用的函数
void SelectFunc(int level, const string& str) {
    if (str.empty())
    {
        cout << "Get no text!\n";
        return;
    }
    switch (level)
    {
    case 1:
        Count_Key_Num(str);
        break;
    case 2:
        Count_SwiCase_Num(str);
        break;
    case 3:
        Count_IfEls_Num(str, 3);
        break;
    case 4:
        Count_IfEls_Num(str, 4);
        break;
    default:
        cout << "wrong level num!\n";
        return;
    }
}

int main() {
   int level;
   string filePath;
   string str;

   cout << "input filepath: ";
   cin >> filePath;
   cout << "input level: ";
   cin >> level;

   string temp;
   ifstream file = OpenMyFile(filePath);
   while (getline(file, temp))  // 获取文件的每一行
   {
       str += DeleSingle(temp);  // 删除每一行里的 //注释，字符串等预处理
   }
   SelectFunc(level, str);  // 根据等级选择处理函数

   file.close();
   return 0;
}
