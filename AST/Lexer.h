#ifndef __LEXER_H__
#define __LEXER_H__

#include "Common.h"

enum tokenType_t {
	TOK_IDENTIFIER,
	TOK_INCREMENT,
	TOK_DECREMENT,
	TOK_PLUS,
	TOK_MINUS,
	TOK_ASSIGN,
	TOK_DECIMAL_INTEGER,
	TOK_TERMINATOR,
	TOK_LPAREN,
	TOK_RPAREN,
	TOK_LBRACE,
	TOK_RBRACE,
	TOK_COMMA,
	TOK_FUNCTION,
	TOK_LINE_COMMENT,
	TOK_IF,
	TOK_WHILE,
	TOK_BREAK,
	TOK_RETURN,
	TOK_EOF,
	TOK_BANG,
	NUM_TOK
};

struct token_t {
	tokenType_t	type;
	string		value;
};

class Lexer {
private:
	int			head;
	char		look;
	string		value;
	const char* input;
	int			inputLength;
	int			line;
	token_t		tok;
private:
	void		_ClearToken();
	void		_Push(char c);
	void		_Pop();
	void		_Clear();
public:
				Lexer(const char* input);
				~Lexer();

	int			Line() const;
	int			Head() const;
	token_t		Token() const;
	void		AdvanceToken();
	void		Advance();
	void		Restore(int where);
	void		Restore(int where, token_t reset);

	bool		MatchAlpha();
	bool		MatchAlphaNum();
	bool		MatchWhite();
	bool		MatchVerticalWhite();
	bool		MatchDecimalDigit();
	bool		MatchDecimalInteger();
	bool		MatchIdentifier();
	bool		Match(const char* str);
	bool		MatchKeyword(const char* str);
	bool		Match(char c);
	bool		MatchLineComment();
};

#endif // __LEXER_H__
