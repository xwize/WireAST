#include "Parser.h"

static string ErrorMsg(parseErrorCode_t code) {
	string msg = "";
	switch (code) {
		case PARSE_ERR_EXPECTING_IDENTIFIER:
			msg = "Expected an identifier";
			break;
		case PARSE_ERR_EXPECTING_STATEMENT:
			msg = "Expected a valid statement";
			break;
		case PARSE_ERR_EXPECTING_TERM:
			msg = "Expected a valid term";
			break;
		case PARSE_ERR_EXPECTING_TERMINATOR:
			msg = "Expected a terminator";
			break;
		case PARSE_ERR_EXPECTING_ASSIGN:
			msg = "Expected an assignment";
			break;
		case PARSE_ERR_EXPECTING_EXPRESSION:
			msg = "Expected an expression";
			break;
		case PARSE_ERR_EXPECTING_LEFT_PAREN:
			msg = "Expected a (";
			break;
		case PARSE_ERR_EXPECTING_RIGHT_PAREN:
			msg = "Expected a )";
			break;
		case PARSE_ERR_EXPECTING_BLOCK_END:
			msg = "Expected a }";
			break;
		case PARSE_ERR_EXPECTING_BLOCK:
			msg = "Expected a block";
			break;
		default:
			msg = "Unknown error";
			break;
	}
	return msg;
}

void Parser::Error(parseErrorCode_t code, string details) {
	if (!speculative) {
		if (error.code == PARSE_ERR_NONE) {
			error.code = code;
			error.details = details;
			error.line = lexer.Line();
		}
	}
}

void Parser::Error(parseErrorCode_t code) {
	string msg = ErrorMsg(code);
	msg += ". Got: " + lexer.Token().value + ".";
	Error(code, msg);
}

void Parser::CertainError(parseErrorCode_t code, string details) {
	if (error.code == PARSE_ERR_NONE) {
		error.code = code;
		error.details = details;
		error.line = lexer.Line();
	}
}

void Parser::CertainError(parseErrorCode_t code) {
	string msg = ErrorMsg(code);
	msg += ". Got: " + lexer.Token().value + ".";
	CertainError(code, msg);
}
