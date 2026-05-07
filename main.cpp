#include <iostream>

#include "lib/src/Lexer.h"
#include "lib/src/token.h"

int main() {
	std::string code = R"(fn return if for while foreach in break continue else(){}[]<><=>=;+-*/,.=!===++--aboba 123 "\r\t\n\\\" fsdfsdf4343" '\r' '\t' '\n' '\\' '\'' 'g')";
	qlang::token::CODE code_lex = qlang::Lexer::Lex(code);
	for (const auto& token : code_lex) {
		std::cout << qlang::token::TokenToString(token.first) << "\n";
	}
}