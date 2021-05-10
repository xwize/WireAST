#ifndef __PARSER_H__
#define __PARSER_H__

#include "Lexer.h"
#include "AST.h"
#include "Symbol.h"

#include "Parser_ast.h"

enum parseErrorCode_t {
	PARSE_ERR_NONE,
	PARSE_ERR_EXPECTING_TERMINATOR,
	PARSE_ERR_EXPECTING_TERM,
	PARSE_ERR_EXPECTING_STATEMENT,
	PARSE_ERR_EXPECTING_IDENTIFIER,
	PARSE_ERR_EXPECTING_ASSIGN,
	PARSE_ERR_EXPECTING_LEFT_PAREN,
	PARSE_ERR_EXPECTING_RIGHT_PAREN,
	PARSE_ERR_EXPECTING_EXPRESSION,
	PARSE_ERR_EXPECTING_BLOCK,
	PARSE_ERR_EXPECTING_BLOCK_END,
};

struct parseError_t {
	parseErrorCode_t		code;
	string					details;
	int						line;
};

struct lexerState_t {
	int						head;
	token_t					tok;
};

struct parseResult_t {
	Ref<Scope>				global;
	Ref<ASTProgram>			ast;
};

class Parser {
private:
	Lexer					lexer;
	parseError_t			error;
	int						speculative;
	token_t					matched;
	vector<lexerState_t>	saved;
	parseResult_t			result;
private:
	Parser_AST				builder;
private:
	bool					Match(tokenType_t token);
	void					Save();
	void					Backtrack();
	void					Speculate(bool val);
	void					Error(parseErrorCode_t code);
	void					Error(parseErrorCode_t code, string details);
	void					CertainError(parseErrorCode_t code);
	void					CertainError(parseErrorCode_t code, string details);
private:
	void					_ExpProgram();
	bool					_ExpStatement();
	bool					_OptStatement();
	bool					_OptExpression();
	bool					_OptExpressionStatement();
	bool					_OptFunctionStatement();
	bool					_OptAssignExpression_r();
	bool					_OptIfStatement();
	bool					_OptWhileStatement();
	bool					_OptBreakStatement();
	bool					_OptReturnStatement();
	bool					_OptCallExpression();
	bool					_OptLhsExpression();
	void					_OptArgList();
	void					_OptParamList();
	bool					_OptPrimary();
	bool					_OptAdd();
	bool					_OptBlock();
	bool					_OptPostfixExpression();
	bool					_OptUnaryExpression();
public:
							Parser(const char* input);
							~Parser();

	bool					HasError() const;
	parseError_t			Error() const;
	parseResult_t			Parse();
};

#endif // __PARSER_H__
