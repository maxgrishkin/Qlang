#pragma once

#include <algorithm>
#include <map>
#include <stdexcept>

#include "token.h"

namespace qlang::Lexer{
	inline token::CODE Lex(std::string code) {
		unsigned int code_length = code.size();
		unsigned short symbol = 1;
		unsigned int line = 1;
		bool line_comment = false;
		bool multi_comment = false;
		token::CODE result;
		for (unsigned int i = 0;i < code_length;i++) {
			if (line_comment) {
				if (code[i] == '\n') {
					line_comment = false;
					line++;
					symbol = 1;
				}
				continue;
			}
			if (multi_comment) {
				if (code[i] == '\n') {
					line++;
					symbol = 1;
				}
				if (code[i] == '*' && code[i+1] == '/') {
					multi_comment = false;
					symbol++;
					i++;
				}
				symbol++;
				continue;
			}
			switch (code[i]) {
				case ' ':
				case '\r':
				case '\t':
					symbol++;
					break;
				case '\n':
					line++;
					symbol = 1;
					break;
				case '(':
					result.emplace_back(token::LPar{},std::make_pair(line,symbol));
					symbol++;
					break;
				case ')':
					result.emplace_back(std::make_pair(token::RPar{},std::make_pair(line,symbol)));
					symbol++;
					break;
				case '{':
					result.emplace_back(std::make_pair(token::LCurlyBracket{},std::make_pair(line,symbol)));
					symbol++;
					break;
				case '}':
					result.emplace_back(std::make_pair(token::RCurlyBracket{},std::make_pair(line,symbol)));
					symbol++;
					break;
				case '[':
					result.emplace_back(std::make_pair(token::LSquareBracket{},std::make_pair(line,symbol)));
					symbol++;
					break;
				case ']':
					result.emplace_back(std::make_pair(token::RSquareBracket{},std::make_pair(line,symbol)));
					symbol++;
					break;
				case '<':
					if (i != code_length - 1) {
						if (code[i+1] == '=') {
							result.emplace_back(std::make_pair(token::LAngleBracketEqual{},std::make_pair(line,symbol)));
							symbol += 2;
							i++;
							break;
						}
					}
					result.emplace_back(std::make_pair(token::LAngleBracket{},std::make_pair(line,symbol)));
					symbol++;
					break;
				case '>':
					if (i != code_length - 1) {
						if (code[i+1] == '=') {
							result.emplace_back(std::make_pair(token::RAngleBracketEqual{},std::make_pair(line,symbol)));
							symbol += 2;
							i++;
							break;
						}
					}
					result.emplace_back(std::make_pair(token::RAngleBracket{},std::make_pair(line,symbol)));
					symbol++;
					break;
				case ';':
					result.emplace_back(std::make_pair(token::Semicolon{},std::make_pair(line,symbol)));
					symbol++;
					break;
				case '+':
					if (i != code_length - 1) {
						if (code[i+1] == '+') {
							result.emplace_back(std::make_pair(token::PlusPlus{},std::make_pair(line,symbol)));
							symbol += 2;
							i++;
							break;
						}
					}
					result.emplace_back(std::make_pair(token::Plus{},std::make_pair(line,symbol)));
					symbol++;
					break;
				case '-':
					if (i != code_length - 1) {
						if (code[i+1] == '-') {
							result.emplace_back(std::make_pair(token::MinusMinus{},std::make_pair(line,symbol)));
							symbol += 2;
							i++;
							break;
						}
					}
					result.emplace_back(std::make_pair(token::Minus{},std::make_pair(line,symbol)));
					symbol++;
					break;
				case '*':
					result.emplace_back(std::make_pair(token::Asterisk{},std::make_pair(line,symbol)));
					symbol++;
					break;
				case '/':
					if (i != code_length - 1) {
						if (code[i+1] == '/') {
							line_comment = true;
							i++;
							break;
						}
						if (code[i+1] == '*') {
							multi_comment = true;
							symbol += 2;
							i++;
							break;
						}
					}
					result.emplace_back(std::make_pair(token::Slash{},std::make_pair(line,symbol)));
					symbol++;
					break;
				case ',':
					result.emplace_back(std::make_pair(token::Comma{},std::make_pair(line,symbol)));
					symbol++;
					break;
				case '.':
					result.emplace_back(std::make_pair(token::Dot{},std::make_pair(line,symbol)));
					symbol++;
					break;
				case '#':
					result.emplace_back(std::make_pair(token::Hash{},std::make_pair(line,symbol)));
					symbol++;
					break;
				case '=':
					if (i != code_length - 1) {
						if (code[i+1] == '=') {
							result.emplace_back(std::make_pair(token::Equal2{},std::make_pair(line,symbol)));
							symbol += 2;
							i++;
							break;
						}
					}
					result.emplace_back(std::make_pair(token::Equal{},std::make_pair(line,symbol)));
					symbol++;
					break;
				case '!':
					if (i != code_length - 1) {
						if (code[i+1] == '=') {
							result.emplace_back(std::make_pair(token::NoEqual{},std::make_pair(line,symbol)));
							symbol += 2;
							i++;
							break;
						}
					}
					break;
				case '\'': {
					unsigned int presymbol = symbol;
					i++;
					symbol++;
					if (code[i] == '\\') {
						i++;
						symbol++;
						if (code[i] == 'r') {
							result.emplace_back(std::make_pair(token::Char{.value = '\r'},std::make_pair(line,presymbol)));
							i++;
							symbol += 2;
							break;
						}
						if (code[i] == 't') {
							result.emplace_back(std::make_pair(token::Char{.value = '\t'},std::make_pair(line,presymbol)));
							i++;
							symbol += 2;
							break;
						}
						if (code[i] == 'n') {
							result.emplace_back(std::make_pair(token::Char{.value = '\n'},std::make_pair(line,presymbol)));
							i++;
							symbol += 2;
							break;
						}
						if (code[i] == '\\') {
							result.emplace_back(std::make_pair(token::Char{.value = '\\'},std::make_pair(line,presymbol)));
							i++;
							symbol += 2;
							break;
						}
						if (code[i] == '\'') {
							result.emplace_back(std::make_pair(token::Char{.value = '\''},std::make_pair(line,presymbol)));
							i++;
							symbol += 2;
							break;
						}
						std::string character = {code[i]};
						throw std::runtime_error("Unexpected character after \\ : " + character + ", at line " + std::to_string(line) + ", at symbol " + std::to_string(symbol));
					}
					if (code[i + 1] != '\'') {
						std::string character = {code[i + 1]};
						throw std::runtime_error("Expected ' , at line " + std::to_string(line) + ", at symbol " + std::to_string(symbol + 1));
					}
					result.emplace_back(std::make_pair(token::Char{.value = code[i]},std::make_pair(line,presymbol)));
					i++;
					symbol++;
					break;
				}
				case '%':
					result.emplace_back(std::make_pair(token::Percent{},std::make_pair(line,symbol)));
					symbol++;
					break;
				case '"': {
					std::string res;
					unsigned int presymbol = symbol;
					i++;
					symbol++;
					while (code[i] != '"') {
						if (code[i] == '\\') {
							i++;
							symbol++;
							if (code[i] == 'r') {
								res += '\r';
								i++;
								symbol++;
								continue;
							}
							if (code[i] == 't') {
								res += '\t';
								i++;
								symbol++;
								continue;
							}
							if (code[i] == 'n') {
								res += '\n';
								i++;
								symbol++;
								continue;
							}
							if (code[i] == '\\') {
								res += '\\';
								i++;
								symbol++;
								continue;
							}
							if (code[i] == '"') {
								res += '"';
								i++;
								symbol++;
								continue;
							}
							std::string character = {code[i]};
							throw std::runtime_error("Unexpected character after \\ : " + character + ", at line " + std::to_string(line) + ", at symbol " + std::to_string(symbol));
						}
						res += code[i];
						i++;
						symbol++;
					}
					result.emplace_back(std::make_pair(token::String{.value = res},std::make_pair(line,presymbol)));
					break;
				}
				default: {
					unsigned short preSymbol = symbol;
					if (std::isalpha(code[i])) {
						std::string text;
						while (std::isalnum(code[i])) {
							text.push_back(code[i]);
							symbol++;
							i++;
						}
						i--;
						static const std::map<std::string,token::Keyword> Keywords = {
							{"fn",token::Keyword::FN},
							{"var",token::Keyword::VAR},
							{"return",token::Keyword::RETURN},
							{"if",token::Keyword::IF},
							{"for",token::Keyword::FOR},
							{"while",token::Keyword::WHILE},
							{"foreach",token::Keyword::FOREACH},
							{"in",token::Keyword::IN},
							{"break",token::Keyword::BREAK},
							{"continue",token::Keyword::CONTINUE},
							{"else",token::Keyword::ELSE},
							{"include",token::Keyword::INCLUDE},
						};
						if (auto keyword = Keywords.find(text); keyword != Keywords.end()) {
							result.emplace_back(std::make_pair(keyword->second,std::make_pair(line,preSymbol)));
							break;
						}
						result.emplace_back(std::make_pair(token::Identifier{.value = text},std::make_pair(line,preSymbol)));
						break;
					}
					if (isdigit(code[i])) {
						std::string text;
						while (std::isdigit(code[i])) {
							text.push_back(code[i]);
							symbol++;
							i++;
						}
						i--;
						result.emplace_back(std::make_pair(token::Integer{.value = std::atoi(text.c_str())},std::make_pair(line,preSymbol)));
						break;
					}
					std::string character = {code[i]};
					throw std::runtime_error("Unexpected character : " + character + ", at line " + std::to_string(line) + ", at symbol " + std::to_string(symbol));
				}
			}
		}
		return result;
	}
}