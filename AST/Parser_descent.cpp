#include "Parser.h"

bool Parser::_OptPrimary() {

	if (Match(TOK_IDENTIFIER)) {
		builder.MakePrimaryFromIdentifier(matched.value);
		return true;
	}

	if (Match(TOK_DECIMAL_INTEGER)) {
		builder.MakePrimaryFromDecIntLiteral(matched.value);
		return true;
	}

	auto BracketedExpr = [&]() -> bool {

		if (!Match(TOK_LPAREN)) {
			return false;
		}

		if (!_OptExpression()) {
			Error(PARSE_ERR_EXPECTING_EXPRESSION);
			return false;
		}

		if (!Match(TOK_RPAREN)) {
			CertainError(PARSE_ERR_EXPECTING_RIGHT_PAREN);
			return false;
		}

		return true;
	};
	
	if (!BracketedExpr()) {
		return false;
	}

	builder.MakePrimaryFromExpression();
	return true;
}

bool Parser::_OptLhsExpression() {

	auto Tentative = [&]() -> bool {
		if (!_OptCallExpression())
			return false;
		builder.MakeLhsFromCall();
		return true;
	};

	Save();
	Speculate(true);
	if (Tentative()) {
		Speculate(false);
		Backtrack();
		bool result = Tentative();
		assert(result);
		return true;
	}

	Speculate(false);
	Backtrack();
	if (_OptPrimary()) {
		builder.MakeLhsFromPrimary();
		return true;
	}

	return false;
}

void Parser::_OptArgList() {

	builder.PushAssignLhsBoundary();
	if (!_OptAssignExpression_r()) {
		builder.PopAssignLhsBoundary();
		return;
	}

	builder.MakeAssign();
	builder.PushCallArgumentFromAssign();

	while (Match(TOK_COMMA)) {
		builder.PushAssignLhsBoundary();
		if (!_OptAssignExpression_r()) {
			builder.PopAssignLhsBoundary();
			CertainError(PARSE_ERR_EXPECTING_EXPRESSION);
			return;
		}

		builder.MakeAssign();
		builder.PushCallArgumentFromAssign();
	}
}

bool Parser::_OptCallExpression() {
	if (!Match(TOK_IDENTIFIER))
		return false;

	builder.PushCallIdentifier(matched.value);

	if (!Match(TOK_LPAREN))
		return false;

	builder.PushCallArgumentBoundary();
	_OptArgList();

	if (!Match(TOK_RPAREN)) {
		builder.PopCallArgumentBoundary();
		CertainError(PARSE_ERR_EXPECTING_RIGHT_PAREN);
		return false;
	}

	builder.MakeCall();
	return true;
}

bool Parser::_OptAssignExpression_r() {

	auto Tentative = [&]() -> bool {
		if (!_OptLhsExpression())
			return false;

		builder.PushAssignLhsFromLhs();

		if (!Match(TOK_ASSIGN)) {
			Error(PARSE_ERR_EXPECTING_ASSIGN);
			return false;
		}

		if (!_OptAssignExpression_r()) {
			Error(PARSE_ERR_EXPECTING_EXPRESSION);
			return false;
		}

		return true;
	};

	Save();
	Speculate(true);
	if (Tentative()) {
		Speculate(false);
		Backtrack();
		bool result = Tentative();
		assert(result);
		return true;
	}

	Speculate(false);
	Backtrack();
	if (_OptAdd()) {
		builder.PushAssignLhsFromAdd();
		return true;
	}

	return false;
}

bool Parser::_OptExpression() {
	builder.PushAssignLhsBoundary();
	if (_OptAssignExpression_r()) {
		builder.MakeAssign();
		builder.MakeExpression();
		return true;
	}
	builder.PopAssignLhsBoundary();
	return false;
}

void Parser::_ExpProgram() {
	while (!Match(TOK_EOF)) {
		if (HasError())
			break;

		if (_ExpStatement()) {
			builder.ProgramStatementFromStatement();
		}
	}

	if (!HasError()) {
		builder.MakeProgram();
	}
}

bool Parser::_OptExpressionStatement() {
	if (!_OptExpression())
		return false;

	if (!Match(TOK_TERMINATOR)) {
		Error(PARSE_ERR_EXPECTING_TERMINATOR);
		return false;
	}

	return true;
}

bool Parser::_OptFunctionStatement() {
	if (!Match(TOK_FUNCTION))
		return false;

	if (!Match(TOK_IDENTIFIER)) {
		Error(PARSE_ERR_EXPECTING_IDENTIFIER);
		return false;
	}

	builder.FunctionIdentifier(matched.value);

	if (!Match(TOK_LPAREN)) {
		CertainError(PARSE_ERR_EXPECTING_LEFT_PAREN);
		return false;
	}

	_OptParamList();

	if (!Match(TOK_RPAREN)) {
		CertainError(PARSE_ERR_EXPECTING_RIGHT_PAREN);
		return false;
	}

	if (!_OptBlock()) {
		CertainError(PARSE_ERR_EXPECTING_BLOCK);
		return false;
	}

	builder.FunctionBlockFromBlock();
	builder.MakeFunction();
	return true;
}

bool Parser::_OptBreakStatement() {
	if (!Match(TOK_BREAK))
		return false;

	if (!Match(TOK_TERMINATOR)) {
		CertainError(PARSE_ERR_EXPECTING_TERMINATOR);
		return false;
	}

	builder.MakeBreak();
	return true;
}

bool Parser::_OptReturnStatement() {
	if (!Match(TOK_RETURN))
		return false;

	if (!_OptExpression()) {
		CertainError(PARSE_ERR_EXPECTING_EXPRESSION);
		return false;
	}

	builder.ReturnExpressionFromExpression();

	if (!Match(TOK_TERMINATOR)) {
		CertainError(PARSE_ERR_EXPECTING_TERMINATOR);
		return false;
	}

	builder.MakeReturn();
	return true;
}

bool Parser::_OptIfStatement() {
	if (!Match(TOK_IF))
		return false;

	if (!Match(TOK_LPAREN)) {
		CertainError(PARSE_ERR_EXPECTING_LEFT_PAREN);
		return false;
	}

	if (!_OptExpression()) {
		CertainError(PARSE_ERR_EXPECTING_EXPRESSION);
		return false;
	}

	builder.PushIfExpressionFromExpression();

	if (!Match(TOK_RPAREN)) {
		CertainError(PARSE_ERR_EXPECTING_RIGHT_PAREN);
		return false;
	}

	if (!_OptStatement()) {
		CertainError(PARSE_ERR_EXPECTING_STATEMENT);
		return false;
	}

	builder.PushIfStatementFromStatement();
	builder.MakeIf();
	return true;
}

bool Parser::_OptWhileStatement() {
	if (!Match(TOK_WHILE))
		return false;

	if (!Match(TOK_LPAREN)) {
		CertainError(PARSE_ERR_EXPECTING_LEFT_PAREN);
		return false;
	}

	if (!_OptExpression()) {
		CertainError(PARSE_ERR_EXPECTING_EXPRESSION);
		return false;
	}

	builder.PushWhileExpressionFromExpression();

	if (!Match(TOK_RPAREN)) {
		CertainError(PARSE_ERR_EXPECTING_RIGHT_PAREN);
		return false;
	}

	if (!_OptStatement()) {
		CertainError(PARSE_ERR_EXPECTING_STATEMENT);
		return false;
	}

	builder.PushWhileStatementFromStatement();
	builder.MakeWhile();
	return true;
}


void Parser::_OptParamList() {
	if (!Match(TOK_IDENTIFIER))
		return;

	builder.PushFunctionParameter(matched.value);

	while (Match(TOK_COMMA)) {
		if (!Match(TOK_IDENTIFIER)) {
			CertainError(PARSE_ERR_EXPECTING_IDENTIFIER);
			return;
		}

		builder.PushFunctionParameter(matched.value);
	}
}

bool Parser::_OptStatement() {
	if (_OptBlock()) {
		builder.MakeStatementFromBlock();
		return true;
	} 

	if (_OptExpressionStatement()) {
		builder.MakeStatementFromExpression();
		return true;
	}

	if (_OptFunctionStatement()) {
		builder.MakeStatementFromFunction();
		return true;
	}

	if (_OptIfStatement()) {
		builder.MakeStatementFromIf();
		return true;
	}

	if (_OptWhileStatement()) {
		builder.MakeStatementFromWhile();
		return true;
	}

	if (_OptBreakStatement()) {
		builder.MakeStatementFromBreak();
		return true;
	}

	if (_OptReturnStatement()) {
		builder.MakeStatementFromReturn();
		return true;
	}

	return false;
}

bool Parser::_ExpStatement() {
	bool result = _OptStatement();
	if (!result) 
		Error(PARSE_ERR_EXPECTING_STATEMENT);
	return result;
}

bool Parser::_OptAdd() {
	if (!_OptUnaryExpression())
		return false;
	
	builder.PushAddTermBoundary();
	builder.PushAddTermFromUnary();

	while (Match(TOK_PLUS) || Match(TOK_MINUS)) {
		builder.PushAddTokenType(matched.type);
		if (!_OptUnaryExpression()) {
			builder.PopAddTermBoundary();
			CertainError(PARSE_ERR_EXPECTING_EXPRESSION);
			return false;
		}
		builder.PushAddTermFromUnary();
	}

	builder.MakeAdd();
	return true;
}

bool Parser::_OptBlock() {
	if (!Match(TOK_LBRACE)) {
		return false;
	}

	builder.PushBlockStatementBoundary();

	// Zero or more
	while (_OptStatement()) {
		builder.PushBlockStatementFromStatement();
	}

	// Expected
	if (!Match(TOK_RBRACE)) {
		builder.PopBlockStatementBoundary();
		CertainError(PARSE_ERR_EXPECTING_BLOCK_END);
		return false;
	}

	builder.MakeBlock();
	return true;
}

bool Parser::_OptUnaryExpression() {
	if (_OptPostfixExpression()) {
		builder.MakeUnaryFromPostfix();
		return true;
	}

	if (Match(TOK_BANG)) {
		if (_OptPostfixExpression()) {
			builder.MakeUnaryNotFromPostfix();
			return true;
		}
		CertainError(PARSE_ERR_EXPECTING_EXPRESSION);
		return false;
	}

	return false;
}

bool Parser::_OptPostfixExpression() {
	if (_OptLhsExpression()) {
		if (Match(TOK_INCREMENT)) {
			builder.MakePostfixIncrementFromLhs();
			return true;
		}

		if (Match(TOK_DECREMENT)) {
			builder.MakePostfixDecrementFromLhs();
			return true;
		}

		builder.MakePostfixFromLhs();
		return true;
	}

	return false;
}