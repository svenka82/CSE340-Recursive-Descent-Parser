#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include <cstring>
#include <cstdlib>

#include "lexer.h"
#include "inputbuf.h"

using namespace std;

string reserved[] = { "END_OF_FILE", "PUBLIC", "PRIVATE",
	"EQUAL", "COLON", "COMMA", "SEMICOLON",
	"LBRAC", "RBRAC", "ID", "ERROR"
};

#define KEYWORDS_COUNT 2
string keyword[] = { "public", "private" };

string scope = "::";
int accessP = 0;
vector<VariableAssignment> assignemnts;

Node::Node() {
	this->next = NULL;
	this->prev = NULL;
	this->name = "";
	this->scope = "";
	this->access = 0;
}

SymbolTable::SymbolTable() {
	root = NULL;
}

void SymbolTable::AddAssignment(Token leftToken, Token rightToken) {
	Node* curr = NULL;
	Node* leftTokenNode = NULL;
	Node* rightTokenNode = NULL;
	VariableAssignment assignment;

	if (root != NULL) {
		curr = root;
		while (curr->next != NULL) {
			curr = curr->next;
		}
	}

	leftTokenNode = curr;
	rightTokenNode = curr;

	assignment.leftVariable = leftToken.lexeme;
	while (leftTokenNode != NULL) {
		if (leftTokenNode->name == leftToken.lexeme)
		{
			if ((leftTokenNode->access != 2) ||
				(leftTokenNode->access == 2 && leftTokenNode->scope == scope)) {
				assignment.leftVscope = leftTokenNode->scope;
				break;
			}
		}
		leftTokenNode = leftTokenNode->prev;
	}

	assignment.rightVariable = rightToken.lexeme;
	while (rightTokenNode != NULL) {
		if (rightTokenNode->name == rightToken.lexeme)
		{
			if ((rightTokenNode->access != 2) ||
				(rightTokenNode->access == 2 && rightTokenNode->scope == scope)) {
				assignment.rightVScope = rightTokenNode->scope;
				break;
			}
		}
		rightTokenNode = rightTokenNode->prev;
	}

	assignemnts.push_back(assignment);
}

void SymbolTable::InsertNode(std::string name) {
	Node* temp;
	Node* curr;

	temp = new Node();
	temp->name = name;
	temp->access = accessP;
	temp->scope = scope;

	if (root == NULL) {
		root = temp;
		return;
	}


	curr = root;
	while (curr->next != NULL) {
		curr = curr->next;
	}
	curr->next = temp;
	temp->prev = curr;

}

void SymbolTable::DeleteNodes(std::string scope) {
	Node* curr = NULL;
	Node* temp = NULL;

	if (root != NULL) {
		curr = root;
		while (curr->next != NULL) {
			curr = curr->next;
		}
	}

	while (curr != NULL) {
		temp = curr;
		if (curr->scope == scope) {
			curr = curr->prev;
			if (curr != NULL) {
				curr->next = NULL;
				temp->prev = NULL;
			}
		}
		else {
			curr = curr->prev;
		}
	}
}

string SymbolTable::GetCurrentScope() {
	Node* curr;

	curr = root;
	while (curr->next != NULL) {
		curr = curr->next;
	}

	return curr->scope;
}

void Parser::parse_program() {
	Token glToken = lexer.GetToken();
	if (glToken.token_type == ID) {
		Token nextToken = lexer.GetToken();
		if (nextToken.token_type == SEMICOLON || nextToken.token_type == COMMA) {
			lexer.UngetToken(nextToken);
			lexer.UngetToken(glToken);
			parse_global_vars();
			parse_scope();
		}
		else if (nextToken.token_type == LBRAC) {
			lexer.UngetToken(nextToken);
			lexer.UngetToken(glToken);
			parse_scope();
		}
		else {
			cout << "Syntax Error\n";
			std::exit(1);
		}
	}
	else {
		cout << "Syntax Error\n";
		std::exit(1);
	}
}

void Parser::parse_global_vars() {
	Token idToken = lexer.GetToken();
	if (idToken.token_type == ID) {
		lexer.UngetToken(idToken);
		parse_var_list();
		Token nextToken = lexer.GetToken();
		if (nextToken.token_type == SEMICOLON) {
			//cout << "global_vars -> var_list SEMICOLON\n";
		}
		else {
			cout << "Syntax Error\n";
			std::exit(1);
		}
	}
	else {
		cout << "Syntax Error\n";
		std::exit(1);
	}
}

void Parser::parse_var_list() {
	Token idToken = lexer.GetToken();
	if (idToken.token_type == ID) {

		syTable.InsertNode(idToken.lexeme);

		Token nextToken = lexer.GetToken();
		if (nextToken.token_type == COMMA) {
			Token nextToNextToken = lexer.GetToken();
			if (nextToNextToken.token_type == ID) {
				lexer.UngetToken(nextToNextToken);
				parse_var_list();
			}
			else {
				cout << "Syntax Error\n";
				std::exit(1);
			}
		}
		else if (nextToken.token_type == SEMICOLON) {
			lexer.UngetToken(nextToken);
			//cout << "var_list -> ID\n";
		}
		else {
			cout << "Syntax Error\n";
			std::exit(1);
		}
	}
	else {
		cout << "Syntax Error\n";
		std::exit(1);
	}
}

void Parser::parse_scope() {
	Token idToken = lexer.GetToken();
	if (idToken.token_type == ID) {
		Token nextToken = lexer.GetToken();
		if (nextToken.token_type == LBRAC) {

			scope = idToken.lexeme;

			parse_public_vars();
			parse_private_vars();
			parse_stmt_list();

			Token nextToNextToken = lexer.GetToken();
			if (nextToNextToken.token_type == RBRAC) {

				scope = idToken.lexeme;
				syTable.DeleteNodes(scope);

				//cout << "scope -> ID LBRACE public_vars private_vars stmt_list RBRACE\n";
			}
			else {
				cout << "Syntax Error\n";
				std::exit(1);
			}
		}
		else {
			cout << "Syntax Error\n";
			std::exit(1);
		}
	}
	else {
		cout << "Syntax Error\n";
		std::exit(1);
	}
}

void Parser::parse_public_vars() {
	Token pubToken = lexer.GetToken();
	if (pubToken.token_type == PUBLIC) {
		accessP = 1;
		Token colToken = lexer.GetToken();
		if (colToken.token_type == COLON) {
			parse_var_list();
			Token semiColonToken = lexer.GetToken();
			if (semiColonToken.token_type == SEMICOLON) {
				//cout << "public_vars -> PUBLIC COLON var_list SEMICOLON\n";
			}
			else {
				cout << "Syntax Error\n";
				std::exit(1);
			}
		}
		else {
			cout << "Syntax Error\n";
			std::exit(1);
		}
	}
	else if (pubToken.token_type == PRIVATE || pubToken.token_type == ID) {
		lexer.UngetToken(pubToken);
		//cout << "public_vars -> epsilon\n";
	}
	else {
		cout << "Syntax Error\n";
		std::exit(1);
	}
}

void Parser::parse_private_vars() {
	Token prvToken = lexer.GetToken();
	if (prvToken.token_type == PRIVATE) {
		accessP = 2;
		Token colToken = lexer.GetToken();
		if (colToken.token_type == COLON) {
			parse_var_list();
			Token semiColonToken = lexer.GetToken();
			if (semiColonToken.token_type == SEMICOLON) {
				//cout << "private_vars -> PRIVATE COLON var_list SEMICOLON\n";
			}
			else {
				cout << "Syntax Error\n";
				std::exit(1);
			}
		}
		else {
			cout << "Syntax Error\n";
			std::exit(1);
		}
	}
	else if (prvToken.token_type == ID) {
		lexer.UngetToken(prvToken);
		//cout << "private_vars -> epsilon\n";
	}
	else {
		cout << "Syntax Error\n";
		std::exit(1);
	}
}

void Parser::parse_stmt() {
	Token idToken = lexer.GetToken();
	if (idToken.token_type == ID) {
		Token nextToken = lexer.GetToken();
		if (nextToken.token_type == EQUAL) {
			Token nextIdToken = lexer.GetToken();
			if (nextIdToken.token_type == ID) {
				Token semiColonToken = lexer.GetToken();
				if (semiColonToken.token_type == SEMICOLON) {
					//cout << "stmt -> ID EQUAL ID SEMICOLON\n";
					syTable.AddAssignment(idToken, nextIdToken);
				}
				else {
					cout << "Syntax Error\n";
					std::exit(1);
				}
			}
			else {
				cout << "Syntax Error\n";
				std::exit(1);
			}
		}
		else if (nextToken.token_type == LBRAC) {
			lexer.UngetToken(nextToken);
			lexer.UngetToken(idToken);
			parse_scope();
		}
		else {
			cout << "Syntax Error\n";
			std::exit(1);
		}
	}
	else {
		cout << "Syntax Error\n";
		std::exit(1);
	}
}

void Parser::parse_stmt_list() {
	Token idToken = lexer.GetToken();
	if (idToken.token_type == ID) {
		lexer.UngetToken(idToken);
		parse_stmt();
		Token nextToken = lexer.GetToken();
		if (nextToken.token_type == ID) {
			lexer.UngetToken(nextToken);
			parse_stmt_list();
		}
		else if (nextToken.token_type == RBRAC) {
			lexer.UngetToken(nextToken);
			//cout << "stmt_list -> stmt\n";
		}
		else {
			cout << "Syntax Error\n";
			std::exit(1);
		}
	}
	else {
		cout << "Syntax Error\n";
		std::exit(1);
	}
}

void Token::Print()
{
	cout << "{" << this->lexeme << " , "
		<< reserved[(int) this->token_type] << " , "
		<< this->line_no << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer()
{
	this->line_no = 1;
	tmp.lexeme = "";
	tmp.line_no = 1;
	tmp.token_type = ERROR;
}

bool LexicalAnalyzer::SkipSpace()
{
	char c;
	bool space_encountered = false;

	input.GetChar(c);
	line_no += (c == '\n');

	while (!input.EndOfInput() && isspace(c)) {
		space_encountered = true;
		input.GetChar(c);
		line_no += (c == '\n');
	}

	if (!input.EndOfInput()) {
		input.UngetChar(c);
	}
	return space_encountered;
}

bool LexicalAnalyzer::IsKeyword(string s)
{
	for (int i = 0; i < KEYWORDS_COUNT; i++) {
		if (s == keyword[i]) {
			return true;
		}
	}
	return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
	for (int i = 0; i < KEYWORDS_COUNT; i++) {
		if (s == keyword[i]) {
			return (TokenType)(i + 1);
		}
	}
	return ERROR;
}

Token LexicalAnalyzer::ScanIdOrKeyword()
{
	char c;
	input.GetChar(c);

	if (isalpha(c)) {
		tmp.lexeme = "";
		while (!input.EndOfInput() && isalnum(c)) {
			tmp.lexeme += c;
			input.GetChar(c);
		}
		if (!input.EndOfInput()) {
			input.UngetChar(c);
		}
		tmp.line_no = line_no;
		if (IsKeyword(tmp.lexeme))
			tmp.token_type = FindKeywordIndex(tmp.lexeme);
		else
			tmp.token_type = ID;
	}
	else {
		if (!input.EndOfInput()) {
			input.UngetChar(c);
		}
		tmp.lexeme = "";
		tmp.token_type = ERROR;
	}
	return tmp;
}

// you should unget tokens in the reverse order in which they
// are obtained. If you execute
//
//    t1 = lexer.GetToken();
//    t2 = lexer.GetToken();
//    t3 = lexer.GetToken();
//
// in this order, you should execute
//
//    lexer.UngetToken(t3);
//    lexer.UngetToken(t2);
//    lexer.UngetToken(t1);
//
// if you want to unget all three tokens. Note that it does not
// make sense to unget t1 without first ungetting t2 and t3
//
TokenType LexicalAnalyzer::UngetToken(Token tok)
{
	tokens.push_back(tok);;
	return tok.token_type;
}

Token LexicalAnalyzer::GetToken()
{
	char c;

	// if there are tokens that were previously
	// stored due to UngetToken(), pop a token and
	// return it without reading from input
	if (!tokens.empty()) {
		tmp = tokens.back();
		tokens.pop_back();
		return tmp;
	}

	SkipSpace();
	SkipComments();
	tmp.lexeme = "";
	tmp.line_no = line_no;
	input.GetChar(c);
	switch (c) {
	case '=':
		tmp.token_type = EQUAL;
		return tmp;
	case ':':
		tmp.token_type = COLON;
		return tmp;
	case ',':
		tmp.token_type = COMMA;
		return tmp;
	case ';':
		tmp.token_type = SEMICOLON;
		return tmp;
	case '{':
		tmp.token_type = LBRAC;
		return tmp;
	case '}':
		tmp.token_type = RBRAC;
		return tmp;
	default:
		if (isalpha(c)) {
			input.UngetChar(c);
			return ScanIdOrKeyword();
		}
		else if (input.EndOfInput())
			tmp.token_type = END_OF_FILE;
		else
			tmp.token_type = ERROR;

		return tmp;
	}
}

void LexicalAnalyzer::SkipComments() {
	char c, c1, c2;

	input.GetChar(c1);
	if (c1 == '/') {
		input.GetChar(c2);
		if (c2 == '/') {
			input.GetChar(c);
			while (c != '\n') {
				input.GetChar(c);
			}
			line_no += 1;
			SkipSpace();
			SkipComments();
		}
		else {
			input.UngetChar(c2);
		}
	}
	else {
		input.UngetChar(c1);
	}
}

std::string GetScopePrefix(string scope) {
	if (scope == "") {
		return "?.";
	}
	else if (scope == "::") {
		return "::";
	}
	else {
		return scope + ".";
	}
}

void PrintVariableResolutions() {
	int length = assignemnts.size();
	for (int i = 0; i < length; i++) {
		VariableAssignment assignment = assignemnts.front();
		string leftScope = GetScopePrefix(assignment.leftVscope);
		string rightScope = GetScopePrefix(assignment.rightVScope);

		cout << leftScope + assignment.leftVariable + " = " + rightScope + assignment.rightVariable;
		cout << "\n";

		assignemnts.erase(assignemnts.begin(), assignemnts.begin() + 1);
	}
}

int main()
{
	/*LexicalAnalyzer lexer;
	Token token;

	token = lexer.GetToken();
	token.Print();
	while (token.token_type != END_OF_FILE)
	{
		token = lexer.GetToken();
		token.Print();
	}*/

	Parser parser;
	parser.parse_program();
	PrintVariableResolutions();
}
