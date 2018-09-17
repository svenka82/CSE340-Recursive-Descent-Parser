/*
 * Copyright (C) Rida Bazzi, 2016
 *
 * Do not share this file with anyone
 */
#ifndef __LEXER__H__
#define __LEXER__H__

#include <vector>
#include <string>

#include "inputbuf.h"

 // ------- token types -------------------

typedef enum {
	END_OF_FILE = 0,
	PUBLIC, PRIVATE,
	EQUAL, COLON, COMMA, SEMICOLON,
	LBRAC, RBRAC, ID, ERROR // TODO: Add labels for new token types here
} TokenType;

class Token {
public:
	void Print();

	std::string lexeme;
	TokenType token_type;
	int line_no;
};

class LexicalAnalyzer {
public:
	Token GetToken();
	TokenType UngetToken(Token);
	LexicalAnalyzer();

private:
	std::vector<Token> tokens;
	int line_no;
	Token tmp;
	InputBuffer input;

	bool SkipSpace();
	bool IsKeyword(std::string);
	void SkipComments();
	TokenType FindKeywordIndex(std::string);
	Token ScanIdOrKeyword();
};

class VariableAssignment {
public:
	std::string leftVariable;
	std::string rightVariable;
	std::string leftVscope;
	std::string rightVScope;
};

class Node {
public:
	Node();
	Node* next;
	Node* prev;
	std::string name;
	std::string scope;
	int access;
};

class SymbolTable {
public:
	SymbolTable();
	void InsertNode(std::string);
	void DeleteNodes(std::string);
	std::string GetCurrentScope();
	void AddAssignment(Token, Token);
private:
	Node* root;
};

class Parser {
public:
	void parse_program();
	void parse_global_vars();
	void parse_var_list();
	void parse_scope();
	void parse_public_vars();
	void parse_private_vars();
	void parse_stmt_list();
	void parse_stmt();

private:
	LexicalAnalyzer lexer;
	SymbolTable syTable;
};

#endif  //__LEXER__H__
