#include <stdio.h>
#include <ctype.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <list>
#include <sstream>
#include<stack>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <algorithm> 
using namespace std;
using std::cout;
using std::endl;
#define MSGLEN  64


string internalvariablesnames[10];
int internalvariables[10];
int internalsize = 0;

int inputvariables[100];
string inputvariablesnames[100];
int inputsize = 0;

list<string> tokenss[100];
int tokenssize[100];
int tokensssize = 0;
int pipec = 0;

bool IsOperand(char C)
{
	if (C >= '0' && C <= '9') return true;
	if (C >= 'a' && C <= 'z') return true;
	if (C >= 'A' && C <= 'Z') return true;
	return false;
}

bool IsOperator(string C)
{
	if (C == "+" || C == "-" || C == "*" || C == "/")
		return true;

	return false;
}

int GetOperatorWeight(string op)
{
	int weight = -1;
	if (op == "+" || op == "-")
		weight = 1;
	if (op == "*" || op == "/")
		weight = 2;

	return weight;
}

int HasHigherPrecedence(string op1, string op2)
{
	int op1Weight = GetOperatorWeight(op1);
	int op2Weight = GetOperatorWeight(op2);

	if (op1Weight >= op2Weight)
	{
		return true;
	}
	return false;
}
bool isinternal(string s)
{
	for (int x = 0; x < internalsize;x++)
	{
		if (internalvariablesnames[x] == s)
			return true;
	}

	return false;
}

bool isinput(string s)
{
	for (int x = 0; x < inputsize; x++)
	{
		if (inputvariablesnames[x] == s)
			return true;
	}

	return false;
}

list<string> InfixToPostfix(list<string> expression)
{
	
	stack<string> S;
	list<string> postfix;

	int size = expression.size();
	for (int i = 0; i< size; i++)
	{
		if (expression.front() == " " || expression.front()== ",")
		{
			expression.pop_front();
		}
		else
		{
			if (IsOperator(expression.front()))
			{
				while (!S.empty() && S.top() != "(" && HasHigherPrecedence(S.top(), expression.front()))
				{
					postfix.push_back(S.top());
					S.pop();
				}
				S.push(expression.front());
				expression.pop_front();
			}
			else
			{
				
				if (isinput(expression.front())||isinternal(expression.front()))
				{
					postfix.push_back(expression.front());
					expression.pop_front();
				}

				else if (expression.front() == "(")
				{
					S.push(expression.front());
					expression.pop_front();
				}

				else if (expression.front() == ")")
				{
					expression.pop_front();
					while (!S.empty() && S.top() != "(") {
						postfix.push_back(S.top());
						S.pop();
					}
					if(!S.empty())
					S.pop();
				}
			}
		}
		
	}

	while (!S.empty()) {
		postfix.push_back(S.top());
		S.pop();
	}

	return postfix;
}

int EvaluatePostfix(list<string> expression, list<int> numbers)
{
	stack<int> S;
	
	int size = expression.size();
	for (int i = 0; i< size; i++)
	{
		 if (IsOperator(expression.front())) {
			// Pop two operands. 
			int operand2 = S.top(); S.pop();
			int operand1 = S.top(); S.pop();
			// Perform operation
			int result =0;
			if (expression.front() =="+")
			{
				result = operand1 + operand2;
			}
			if (expression.front() == "-")
			{
				result = operand1 - operand2;
			}
			if (expression.front() == "*")
			{
				result = operand1 * operand2;
			}
			if (expression.front() == "/")
			{
				result = operand1 / operand2;
			}
			expression.pop_front();
			//Push back result of operation on stack. 
			S.push(result);
		}
		else
		{
			// Push operand on stack. 
			S.push(numbers.front());
			numbers.pop_front();
			expression.pop_front();
		}
	}
	// If expression is in correct format, Stack will finally have one element. This will be the output. 
	return S.top();
	return 0;
}

int operatorcount(list<string> tokens)
{
	int s = tokens.size();
	int count = 0;

	for (int x = 0; x < s; x++)
	{
		string op = tokens.front();
		tokens.pop_front();
		if (op == "+" || op == "-" || op == "*" || op == "/")
			count++;

	}

	return count;
}

const int MAX_CHARS_PER_LINE = 512;
const int MAX_TOKENS_PER_LINE = 20;
const char* const DELIMITER = " {";

int main() {



	/*string input1;
	cout << "file name for input variables: ";
	cin >> input1;
	string input2;
	cout << "file name for code: ";
	cin >> input2;*/

	ifstream file2("hw1input2.txt");

	string str;
	getline(file2, str);
	stringstream ss(str);
	int i;
	while (ss >> i)
	{
		inputvariables[inputsize] = i;
		inputsize++;
		if (ss.peek() == ',' || ss.peek() == ' ')
			ss.ignore();
	}

	file2.close();

	//for (int x = 0; x < inputsize; x++)
	//{
	//	cout << inputvariables[x]<< endl;
	//}

	ifstream  file1("hw1input1.txt");

	string str2;

	int cobegin = 0;

	while (getline(file1, str2))
	{
		if (str2.substr(0,1) == "}")
		{
			break;
		}
		if (str2.size() >= 1)
		{
			str2 = str2.substr(0, str2.length() - 1);
			if (str2.substr(str2.length() - 1) != ";")
			{
				str2 += ";";
			}
		}
		stringstream ss2(str2);
		string token;
		list<string> tokens;

		while (ss2.peek() != ';')
		{

			if (ss2.peek() == ',' || ss2.peek() == ' ' || ss2.peek() == ',')
			{
				ss2.ignore();
				if (token != "{" && token!="")
				{
					tokens.push_back(token);
				}

				token = "";
			}
			else
			{
				if (ss2.peek() == '(' || ss2.peek() == ')' || ss2.peek() == '=' || ss2.peek() == '+' || ss2.peek() == '-' || ss2.peek() == '*' || ss2.peek() == '/')
				{
					if (token == "")
					{
						token = ss2.peek();
						tokens.push_back(token);
						ss2.ignore();
						token = "";
					}
					else
					{
						tokens.push_back(token);
						token = ss2.peek();
						
						ss2.ignore();
						tokens.push_back(token);
						token = "";
						
					}
				}
				else
				{
					token += ss2.peek();
					ss2.ignore();
				}

			}
		}

		if (token != "")
		{
			tokens.push_back(token);
		}


		int n = tokens.size();

		if (tokens.front() == "cobegin" || tokens.front() == "coend" || isinternal(tokens.front()))
		{
			copy(tokens.begin(), tokens.end(), back_inserter(tokenss[tokensssize]));
			tokenssize[tokensssize] = n;
			tokensssize++;
			if(isinternal(tokens.front()))
			{
				tokens.pop_front();
				tokens.pop_front();
				n = tokens.size();
				for (int x = 0; x < n;x++)
				{
					if (isinternal(tokens.front())||isinput(tokens.front()))
					{
						pipec++;
					}
					tokens.pop_front();
				}
			}
		}

		if (tokens.front() == "input_var")
		{
			
			tokens.pop_front();
			n--;

			for (int x = 0; x < n; x++)
			{
				inputvariablesnames[x] =  tokens.front();
				tokens.pop_front();

				//cout << inputvariablesnames[x] << endl;
			}
		}

		if (tokens.front() == "internal_var")
		{
			tokens.pop_front();
			n--;
			for (int x = 0; x < n; x++)
			{
				internalsize++;
				internalvariablesnames[x] = tokens.front();
				tokens.pop_front();

				//cout << internalvariablesnames[x] << endl;
			}
		}

		//for (int x = 0; x < n; x++)
		//{
		//	cout << tokens.front() << " ";
		//	tokens.pop_front();
		//}
	//	cout << endl; 
	}
	//cout <<"end" << endl;
	file1.close();

	int pid;
	string processname; // process name
	int num; // value of process
	int inputpipes[inputsize][2]; //pipes to read and write to process of input and internal
	int internalpipes[internalsize][2];
	int pipes[pipec][2];
	string pipeorder[pipec]; //write
	string pipeorder2[pipec]; //read

	int index = 0;

	for (int x = 0; x < tokensssize; x++)
	{
		if (isinternal(tokenss[x].front()))
		{
			string readp = tokenss[x].front();
			list<string> temp;
			copy(tokenss[x].begin(), tokenss[x].end(), back_inserter(temp));
			temp.pop_front();
			temp.pop_front();
			while (!temp.empty())
			{
				if (isinternal(temp.front())||isinput(temp.front()))
				{
					pipeorder[index] = temp.front();
					pipeorder2[index] = readp;
					index++;
				}
				temp.pop_front();
			}

		}
	}
	//for (int x = 0; x < pipec; x++)
	//{
	//	cout<< "write "<< pipeorder[x] << endl;
	//	cout << "read " << pipeorder2[x] << endl;
	//}

	bool input = false;

	for (int x = 0; x < inputsize; x++)
	{
		if (pipe(inputpipes[x]) == -1)
		{
			perror("error creating input pipe");
			exit(0);
		}
	}

	for (int x = 0; x < internalsize; x++)
	{
		if (pipe(internalpipes[x]) == -1)
		{
			perror("error creating internal pipe");
			exit(0);
		}
	}

	for (int x = 0; x < pipec; x++)
	{
		if (pipe(pipes[x]) == -1)
		{
			perror("error creating connecting pipe");
			exit(0);
		}
	}
	
	for (int x = 0; x<inputsize; x++)
	{
		pid = fork();

		if (pid == 0)
		{
			input = true;
			processname = inputvariablesnames[x];

		//	cout << "I am input " << processname << " child" << endl;

			//	close(inputpipes[x][1]);
				read(inputpipes[x][0], &num, sizeof(num));
				cout<<processname<<" received " << num <<", pid is: " << getpid()<<endl;
				//printf("Child(%d) %s received value: %d\n", getpid(), num);
				close(inputpipes[x][0]);


				//write to process
				for (int y = 0; y < pipec; y++)
				{
					if (processname == pipeorder[y])
					{
						close(pipes[y][0]);
						write(pipes[y][1], &num, sizeof(num));
						cout << processname <<" INPUT VAR WRITE PIPE: " << num << ", pid is: " << getpid() << endl;
						close(pipes[y][1]);
					}
				}


					write(inputpipes[x][1], &num, sizeof(num));
					cout << processname << " INPUT VAR WRITE TO PARENT: " << num << ", pid is: " << getpid() << endl;
					close(inputpipes[x][1]);

				break;
		}

		cout << "My input child "<< processname << x << ", pid is: " << pid << endl;
	}

	if (!input)
	{
		//create internal processes

		for (int x = 0; x<internalsize; x++)
		{
			pid = fork();

			if (pid == 0)
			{
				processname = internalvariablesnames[x];

			//	cout << "I am internal " << processname << " child" << endl;
				list<int> numbers; //store inputs


				for (int y = 0; y < pipec; y++)
				{
					if (processname == pipeorder2[y])
					{
						int g;
						close(pipes[y][1]);
						read(pipes[y][0], &g, sizeof(g));
						cout << processname <<" INTERNAL VAR READ PIPE: " << g << ", pid is: " << getpid() << endl;
						numbers.push_back(g);
						close(pipes[y][0]);
						
					}
				}

				list<string> operations; //operations to evaluate
				for (int y = 0; y < tokensssize; y++)
				{
					if (tokenss[y].front()==processname)
					{
						copy(tokenss[y].begin(), tokenss[y].end(), back_inserter(operations));
					}
				}
				if (!operations.empty())
					operations.pop_front();
				if (!operations.empty())
					operations.pop_front();

				num = EvaluatePostfix(InfixToPostfix(operations), numbers);
				
				//write to process
				for (int y = 0; y < pipec; y++)
				{
					if (processname == pipeorder[y])
					{
						close(pipes[y][0]);
						write(pipes[y][1], &num, sizeof(num));
						cout << processname << " INTERNAL VAR WRITE TO PIPE: " << num << ", pid is: " << getpid() << endl;
						close(pipes[y][1]);
					}
				}


				close(internalpipes[x][0]);
				write(internalpipes[x][1], &num, sizeof(num));
				cout << processname << " INTERNAL VAR WRITE TO PARSENT: " << num << ", pid is: " << getpid() << endl;
				close(internalpipes[x][1]);



				break;


			}
			cout << "My internal variable child " << x << ", pid is: " << pid << endl;
		}
	}

	if (pid == 0)
	{
		//cout << "i am child" << getpid() << endl;
	}
	else
	{
		for (int x = 0; x<inputsize; x++)
		{
				num = inputvariables[x];

				//close(inputpipes[x][0]);
				write(inputpipes[x][1], &num, sizeof(num));
				printf("PARENT(%d) SEND VALUE TO INPUT VAR: %d\n", getpid(), num);
				close(inputpipes[x][1]);
		}

		//write
		for (int x = 0; x < inputsize + internalsize;x++)
		{
			wait(NULL);
		}
		for (int x = 0; x < inputsize; x++)
		{

			read(inputpipes[x][0], &num, sizeof(num));
			close(inputpipes[x][0]);

			cout << inputvariablesnames[x] << " = " << num <<endl;
		}
		for (int x = 0; x < internalsize; x++)
		{
			close(internalpipes[x][1]);
			read(internalpipes[x][0], &num, sizeof(num));
			close(internalpipes[x][0]);

			cout << internalvariablesnames[x] << " = " << num<<endl;
		}

	}

	//exit(0);
}