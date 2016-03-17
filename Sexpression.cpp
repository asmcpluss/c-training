#include <iostream>
#include <string>
#include <stdio.h>
#include <list>
#include <sstream>
//#include <Windows.h>
#include <vector>
using namespace std;
typedef list<string> STRLIST;
string compute(STRLIST);
bool IsNumber(string);
bool IsBool(string);
bool IsVariable(string);
int findRight(int,string *);
string findInVariableList(string);
string getValue(int,int,string *);


int Length;
struct Variable{
	string name;
	string value;
};
vector<Variable> varStack;

int main(void){
	int lines;//行数
	bool found = true;
	cin>>lines;
	STRLIST result;
	for(int i=0;i<lines;i++){
		STRLIST element;
		string str;
		while(found){
			cin>>str;
			element.push_back(str);
			if(getchar()!='\n'){
				continue;
			}
			else {
				found = false;
			}
		}
		found = true;
		result.push_back(compute(element));
	}
	for(int j=0;j<lines;j++){
		cout<<result.front()<<endl;
		result.pop_front();
	}
	/*system("pause");*/
	return 0;
}

string compute(STRLIST strlist){
	Length = strlist.size();
	string element[200];//注意指针的初始化
	for(int i = 0;i<Length;i++){  //因为数组更方便进行匹配访问，所以将链表的值赋给数组
		element[i]=strlist.front();
		strlist.pop_front();
	}
	return getValue(0,Length-1,element);
}

//判断字符串是否是整数字符
bool IsNumber(string spara){
	for(unsigned int i=0;i<spara.length();i++){
		if((spara[i]-'0')>=0 && (spara[i]-'0')<=9){
			continue;
		}
		else {
			return false;
		}
	}
	return true;
}

bool IsBool(string str){
	if(str.compare("true")==0||str.compare("false")==0){
		return true;
	}
	return false;
}

bool IsVariable(string str){
	for(unsigned int i=0;i<str.length();i++){
		if(str[i]-'a'<0||str[i]-'a'>26){
			return false;
		}
	}
	return true;
}
//find right ) matched with ( at xleft
int findRight(int xleft,string str[]){
	int count = 0;
	for(int i=xleft;i<Length;i++){
		if(str[i]=="("){
			count++;
		}
		if(str[i]==")"){
			count--;
			if(count==0){
				return i;
			}
		}
	}
}

//判断结果是否是应出现的三种出错情况中的一种 "Division By Zero", "Type Mismatch" and "Unbound Identifier"
bool belongToResult(string s){
	if(s.compare("Division By Zero")==0||s.compare("Type Mismatch")==0||s.compare("Unbound Identifier")==0){
		return true;
	}
	else return false;
}

string findInVariableList(string variableName){
	for(int i=varStack.size()-1;i>=0;i--){
		if((varStack.at(i).name).compare(variableName)==0){
			return varStack.at(i).value;
		}
	}
    // 找不到对应的变量
    return "Unbound Identifier";
}


//递归思想
string getValue(int left,int right,string str[]){
	if(str[left].compare("(")==0){
		if(str[left+1].compare("if")==0){
			//if expression( if a x y )
			int a_left = left + 2;
			int a_right,x_left,x_right,y_left,y_right;
			string a;
			if(str[a_left].compare("(")==0){
				a_right = findRight(a_left,str);
			}
			else {
				a_right = a_left;
			}
			a = getValue(a_left,a_right,str);
			//if a is not bool value,produce "Type Mismatch"
			if(!IsBool(a)){
				return "Type Mismatch";
			}
			//compute region of x
			x_left = a_right + 1;
			if (str[x_left].compare("(")==0) {
				x_right = findRight(x_left,str);
			}
			else{
				 x_right = x_left;
			}
			//compute region of y
			y_left = x_right + 1;
			if (str[y_left].compare("(")==0) {
				y_right = findRight(y_left,str);
			}
			else{
				 y_right = y_left;
			}
			//result
			if(a.compare("true")==0){
				return getValue(x_left, x_right,str);
			}
			else {
				return getValue(y_left, y_right,str);
			}
		}
		//let expression
		else if(str[left+1].compare("let")==0){
			int define_left = left + 2;
			int define_right = findRight(define_left,str);
			string varName = str[define_left+1];
			string varValue = getValue(define_left+2,define_right-1,str);
			if(belongToResult(varValue)){
				return varValue;
			}
			Variable var;
			var.name=varName;
			var.value=varValue;
			varStack.push_back(var);
			string tpAns = getValue(define_right + 1, right - 1,str);
			varStack.pop_back();
			return tpAns;
		}
		
		//compute operand x  (f x y)
		char op= str[left+1][0];
		int x_left = left+2;
		int x_right;
		if(str[x_left].compare("(")==0){
			x_right = findRight(x_left,str);
		}
		else {
			x_right = x_left;
		}
		string x = getValue(x_left,x_right,str);
		//compute operand y (f x y)
		int y_left = x_right + 1;
		int y_right;
		string y;
		int y_op;
		if(str[y_left]=="("){
			y_right = findRight(y_left,str); 
		}
		else {
			y_right = y_left;
		}
		y = getValue(y_left,y_right,str);
		if(!IsNumber(x)||!IsNumber(y)){
			return "Type Mismatch";
		}
		if(belongToResult(x)){
			return x;
		}
		if(belongToResult(y)){
			return y;
		}
		//compute result
		if(op=='/' && y=="0"){
			return "Division By Zero";
		}
		int x_op = atoi(getValue(x_left,x_right,str).c_str());
		y_op = atoi(getValue(y_left,y_right,str).c_str());
		stringstream ss;
		string s;
		switch(op){
			case '/':ss<<(x_op/y_op);ss>>s;return s;break;
			case '+':ss<<(x_op+y_op);ss>>s;return s;break;
			case '-':ss<<(x_op-y_op);ss>>s;return s;break;
			case '*':ss<<(x_op*y_op);ss>>s;return s;break;
			case '<':if(x_op < y_op){ss<<"true";} else ss<<"false";ss>>s;return s;break;
			case '>':if(x_op > y_op){ss<<"true";} else ss<<"false";ss>>s;return s;break;
			case '=':if(x_op == y_op){ss<<"true";} else ss<<"false";ss>>s;return s;break;
		}
	}
	else if(IsNumber(str[left])){
		return str[left];
	}
	else if(IsBool(str[left])){
		return str[left];
	}
	else if(IsVariable(str[left])){
		return findInVariableList(str[left]);
	}
}