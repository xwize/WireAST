#include "Lexer.h"
#include "Char.h"

//#ifdef _DEBUG
#if 0
#define DEBUG_TRACE(X) { printf(X); printf("\n"); }
#else
#define DEBUG_TRACE(X)
#endif

Lexer::Lexer(const char * inp) : input(inp) {
	assert(input != nullptr);
	inputLength = static_cast<int>(strlen(input));
	line = 0;
	value.clear();
	head = -1;
	Advance();
}

Lexer::~Lexer() {
}

void Lexer::_ClearToken() {
	tok.type = TOK_EOF;
	tok.value.clear();
}

void Lexer::_Push(char c) {
	value.push_back(c);
}

void Lexer::_Pop() {
	if (value.size() > 0)
		value.pop_back();
}

void Lexer::_Clear() {
	value.clear();
}

int Lexer::Line() const {
	return line;
}

int Lexer::Head() const {
	return head;
}

token_t Lexer::Token() const {
	return tok;
}

void Lexer::Restore(int where) {
	while (head > where) {
		_Pop();
		head--;
		look = input[head];
		if (IsVerticalWhite(look)) {
			line--;
		}
	}
	assert(head == where);
	assert(look == input[head]);
}

void Lexer::Restore(int where, token_t reset) {
	Restore(where);
	tok = reset;
}

void Lexer::Advance() {
	if (head < inputLength) {
		_Push(look);
		if (IsVerticalWhite(look)) {
			line++;
		}
		head++;
		look = input[head];
	}
}

bool Lexer::Match(char c) {
	if (look == c) {
		Advance();
		return true;
	}
	return false;
}

bool Lexer::MatchAlpha() {
	if (IsAlpha(look)) {
		Advance();
		return true;
	}
	return false;
}

bool Lexer::MatchAlphaNum() {
	if (IsAlphaNumeric(look)) {
		Advance();
		return true;
	}
	return false;
}

bool Lexer::MatchWhite() {
	if (IsWhite(look)) {
		Advance();
		return true;
	}
	return false;
}

bool Lexer::MatchVerticalWhite() {
	if (IsVerticalWhite(look)) {
		Advance();
		return true;
	}
	return false;
}

bool Lexer::MatchDecimalDigit() {
	if (IsDigit(look)) {
		Advance();
		return true;
	}
	return false;
}

bool Lexer::MatchDecimalInteger() {
	if (MatchDecimalDigit()) {
		while (MatchDecimalDigit()) {
		}
		return true;
	}
	return false;
}

bool Lexer::MatchIdentifier() {
	if (!MatchAlpha())
		return false;

	while (
		MatchAlphaNum() ||
		Match('_')) {
	}

	return true;
}

bool Lexer::Match(const char* str) {
	int i = 0;
	int len = static_cast<int>(strlen(str));
	for (int i = 0; i < len; ++i) {
		if (!Match(str[i])) {
			return false;
		}
	}
	return true;
}

bool Lexer::MatchKeyword(const char* str) {
	if (!Match(str))
		return false;

	if (IsAlphaNumeric(look)) {
		return false;
	}

	return true;
}

bool Lexer::MatchLineComment() {
	if (Match("//")) {
		while (!MatchVerticalWhite()) {
			Advance();
		}
		return true;
	}
	return false;
}

void Lexer::AdvanceToken() {

	do {
		while (MatchLineComment()) {}
	} while (MatchWhite());

	_Clear();

	int tmp = head;

	if (head >= inputLength) {
		tok.type = TOK_EOF;
		DEBUG_TRACE("Found EOF.");
		return;
	}

	Restore(tmp);
	if (Match('!')) {
		tok.type = TOK_BANG;
		tok.value = value;
		assert(tok.value == "!");
		DEBUG_TRACE("Found bang.");
		return;
	}

	Restore(tmp);
	if (Match("++")) {
		tok.type = TOK_INCREMENT;
		tok.value = value;
		assert(tok.value == "++");
		DEBUG_TRACE("Found increment.");
		return;
	}

	Restore(tmp);
	if (Match("--")) {
		tok.type = TOK_DECREMENT;
		tok.value = value;
		assert(tok.value == "--");
		DEBUG_TRACE("Found decrement.");
		return;
	}

	Restore(tmp);
	if (MatchKeyword("if")) {
		tok.type = TOK_IF;
		tok.value = value;
		assert(tok.value == "if");
		DEBUG_TRACE("Found if.");
		return;
	}

	Restore(tmp);
	if (MatchKeyword("while")) {
		tok.type = TOK_WHILE;
		tok.value = value;
		assert(tok.value == "while");
		DEBUG_TRACE("Found while.");
		return;
	}

	Restore(tmp);
	if (MatchKeyword("function")) {
		tok.type = TOK_FUNCTION;
		tok.value = value;
		assert(tok.value == "function");
		DEBUG_TRACE("Found function.");
		return;
	}

	Restore(tmp);
	if (MatchKeyword("return")) {
		tok.type = TOK_RETURN;
		tok.value = value;
		assert(tok.value == "return");
		DEBUG_TRACE("Found return.");
		return;
	}

	Restore(tmp);
	if (MatchKeyword("break")) {
		tok.type = TOK_BREAK;
		tok.value = value;
		assert(tok.value == "break");
		DEBUG_TRACE("Found break.");
		return;
	}

	Restore(tmp);
	if (Match('+')) {
		tok.type = TOK_PLUS;
		tok.value = value;
		assert(tok.value == "+");
		DEBUG_TRACE("Found +.");
		return;
	}

	Restore(tmp);
	if (Match('-')) {
		tok.type = TOK_MINUS;
		tok.value = value;
		assert(tok.value == "-");
		DEBUG_TRACE("Found -.");
		return;
	}

	Restore(tmp);
	if (Match('=')) {
		tok.type = TOK_ASSIGN;
		tok.value = value;
		assert(tok.value == "=");
		DEBUG_TRACE("Found =.");
		return;
	}

	Restore(tmp);
	if (Match(';')) {
		tok.type = TOK_TERMINATOR;
		tok.value = value;
		assert(tok.value == ";");
		DEBUG_TRACE("Found ;.");
		return;
	}

	Restore(tmp);
	if (Match(',')) {
		tok.type = TOK_COMMA;
		tok.value = value;
		assert(tok.value == ",");
		DEBUG_TRACE("Found ,.");
		return;
	}

	Restore(tmp);
	if (Match('(')) {
		tok.type = TOK_LPAREN;
		tok.value = value;
		assert(tok.value == "(");
		DEBUG_TRACE("Found (.");
		return;
	}

	Restore(tmp);
	if (Match(')')) {
		tok.type = TOK_RPAREN;
		tok.value = value;
		assert(tok.value == ")");
		DEBUG_TRACE("Found ).");
		return;
	}

	Restore(tmp);
	if (Match('{')) {
		tok.type = TOK_LBRACE;
		tok.value = value;
		assert(tok.value == "{");
		DEBUG_TRACE("Found {.");
		return;
	}

	Restore(tmp);
	if (Match('}')) {
		tok.type = TOK_RBRACE;
		tok.value = value;
		assert(tok.value == "}");
		DEBUG_TRACE("Found }.");
		return;
	}


	Restore(tmp);
	if (MatchDecimalInteger()) {
		tok.type = TOK_DECIMAL_INTEGER;
		tok.value = value;
		assert(IsDigit(tok.value[0]));
		DEBUG_TRACE("Found decimal integer.");
		return;
	}

	Restore(tmp);
	if (MatchIdentifier()) {
		tok.type = TOK_IDENTIFIER;
		tok.value = value;
		assert(IsAlpha(tok.value[0]));
		DEBUG_TRACE("Found identifier.");
		return;
	}
}
