#include "Parser.h"

Parser::Parser(const char * input) : lexer(input) {
	error.code = PARSE_ERR_NONE;
	error.details = "";
	speculative = 0;
}

Parser::~Parser() {
}

bool Parser::HasError() const {
	return (error.code != PARSE_ERR_NONE);
}

parseError_t Parser::Error() const {
	return error;
}

void Parser::Save() {
	lexerState_t state;
	state.head = lexer.Head();
	state.tok = lexer.Token();
	saved.push_back(state);
}

void Parser::Backtrack() {
	lexerState_t& state = saved.back();
	lexer.Restore(state.head, state.tok);
	saved.pop_back();
}

void Parser::Speculate(bool val) {
	if (val) speculative++;
	else speculative--;
	builder.Speculate(speculative != 0 ? true : false);
}

bool Parser::Match(tokenType_t type) {
	if (lexer.Token().type == type) {
		matched = lexer.Token();
		lexer.AdvanceToken();
		return true;
	}
	return false;
}

parseResult_t Parser::Parse() {
	lexer.Restore(0);
	lexer.AdvanceToken();

	_ExpProgram();

	if (HasError()) {
		result.ast = nullptr;
		result.global = nullptr;
	} else {
		result.ast = builder.AST();
	}

	return result;
}
