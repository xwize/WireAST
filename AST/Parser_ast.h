#ifndef __PARSER_AST_H__
#define __PARSER_AST_H__

#include "Common.h"
#include "AST.h"
#include "Lexer.h"

class Parser_AST {
private:
	friend class Parser;
	bool						speculative;
private:
	// Primary
	ASTNodeRef					primary;
	// Call
	vector<ASTNodeRef>			callArgumentStack;
	vector<size_t>				callArgumentBoundaries;
	vector<ASTIdentifierRef>	callIdentifierStack;
	ASTCallRef					call;
	// Function definition
	vector<ASTParameterRef>		functionParameters;
	ASTIdentifierRef			functionIdentifier;
	ASTBlockRef					functionBlock;
	ASTFuncDefRef				function;
	// LHS
	ASTNodeRef					lhs;
	// Postfix
	ASTNodeRef					postfix;
	// Unary					
	ASTNodeRef					unary;
	// Additive
	vector<ASTNodeRef>			addTermStack;
	vector<tokenType_t>			addTokenStack;
	vector<size_t>				addTermBoundaries;
	ASTNodeRef					add;
	// Assign
	vector<ASTNodeRef>			assignLhsStack;
	vector<size_t>				assignLhsBoundaries;
	ASTNodeRef					assign;
	// Expression
	ASTNodeRef					expression;
	// Statement
	ASTNodeRef					statement;
	// Block
	vector<ASTNodeRef>			blockStatements;
	vector<size_t>				blockBoundaries;
	ASTBlockRef					block;
	// If
	vector<ASTNodeRef>			ctrlIfExpressionStack;
	vector<ASTNodeRef>			ctrlIfStatementStack;
	ASTNodeRef					ctrlIf;
	// While
	vector<ASTNodeRef>			ctrlWhileExpressionStack;
	vector<ASTNodeRef>			ctrlWhileStatementStack;
	ASTNodeRef					ctrlWhile;
	// Break
	ASTBreakRef					ctrlBreak;
	// Return
	ASTNodeRef					ctrlReturnExpression;
	ASTReturnRef				ctrlReturn;
	// Program
	vector<ASTNodeRef>			programStatements;
	ASTProgramRef				program;
private:
	Parser_AST();
	~Parser_AST();

	// Retrieve the AST
	Ref<ASTProgram> AST();

	// Turn on to disable operation
	void Speculate(bool val);

	// Primary
	void MakePrimaryFromDecIntLiteral(const string& value);
	void MakePrimaryFromIdentifier(const string& value);
	void MakePrimaryFromExpression();

	// Call
	void PushCallIdentifier(const string& value);
	void PushCallArgumentFromAssign();
	void PushCallArgumentBoundary();
	void PopCallArgumentBoundary();
	void MakeCall();

	// Function
	void FunctionIdentifier(const string& value);
	void PushFunctionParameter(const string& value);
	void FunctionBlockFromBlock();
	void MakeFunction();

	// LHS
	void MakeLhsFromPrimary();
	void MakeLhsFromCall();

	// Postfix
	void MakePostfixFromLhs();
	void MakePostfixDecrementFromLhs();
	void MakePostfixIncrementFromLhs();

	// Unary
	void MakeUnaryFromPostfix();
	void MakeUnaryNotFromPostfix();

	// Add
	void PushAddTokenType(const tokenType_t& token);
	void PushAddTermFromUnary();
	void PushAddTermBoundary();
	void PopAddTermBoundary();
	void MakeAdd();

	// Assign
	void PushAssignLhsFromLhs();
	void PushAssignLhsFromAdd();
	void PushAssignLhsBoundary();
	void PopAssignLhsBoundary();
	void MakeAssign();

	// Expression
	void MakeExpressionFromAssign();
	void MakeExpression();

	// If
	void PushIfExpressionFromExpression();
	void PushIfStatementFromStatement();
	void MakeIf();

	// While
	void PushWhileExpressionFromExpression();
	void PushWhileStatementFromStatement();
	void MakeWhile();

	// Break
	void MakeBreak();

	// Return
	void ReturnExpressionFromExpression();
	void MakeReturn();

	// Statement
	void MakeStatementFromExpression();
	void MakeStatementFromFunction();
	void MakeStatementFromBlock();
	void MakeStatementFromIf();
	void MakeStatementFromWhile();
	void MakeStatementFromBreak();
	void MakeStatementFromReturn();

	// Block
	void PushBlockStatementFromStatement();
	void PushBlockStatementBoundary();
	void PopBlockStatementBoundary();
	void MakeBlock();

	// Program
	void ProgramStatementFromStatement();
	void MakeProgram();
};

#endif // __PARSER_AST_H__
