#include "Char.h"

bool IsWhite(char c) {
	return (
		c == 0x20 || // space
		c == 0x09 || // tab
		c == 0x0a || // lf
		c == 0x0b || // vt
		c == 0x0c || // ff
		c == 0x0d	 // cr
		);
}

bool IsUpper(char c) {
	return (c >= 'A' && c <= 'Z');
}

bool IsLower(char c) {
	return (c >= 'a' && c <= 'z');
}

bool IsAlpha(char c) {
	return (IsUpper(c) || IsLower(c));
}

bool IsDigit(char c) {
	return (
		c >= '0' &&
		c <= '9'
		);
}

bool IsAlphaNumeric(char c) {
	return (IsAlpha(c) || IsDigit(c));
}

bool IsFilenameChar(char c) {
	return (
		IsAlpha(c) ||
		IsDigit(c) ||
		c == '.'
		);
}

bool IsSign(char c) {
	return (
		c == '-'
		);
}

bool IsVerticalWhite(char c) {
	return (c == '\n' || c == 0x0d);
}

bool IsHorizontalWhite(char c) {
	return (c == ' ' || c == '\t');
}