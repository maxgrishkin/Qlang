#include "Compiler.h"

#include <stdexcept>
#include "Parser.h"

qlang::SectionsCode qlang::Compiler::Compiling(token::CODE& code) {
	SectionsCode res{.code = "", .data = ""};

	for (;;) {
		bool breaker = false;
		std::visit(match(
		[code](auto&& a)  {
			throw std::runtime_error("Unexpected token: " + token::TokenToString(a) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second));
		},
		[&code, &breaker](token::RCurlyBracket)  {
			code.erase(code.begin());
			breaker = true;
		},
		[&code, this, &breaker, &res](token::Keyword keyword) {
			code.erase(code.begin());
			switch(keyword) {
				case token::Keyword::VAR: {
					//next token(s) must be a type
					if (!std::holds_alternative<token::Identifier>(code[0].first)) {
						throw std::runtime_error("Unexpected token: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected Identifier");
					}
					const Type* typeVar = take_type(code);

					//next token must be a name
					if (!std::holds_alternative<token::Identifier>(code[0].first)) {
						throw std::runtime_error("Unexpected token: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected Identifier");
					}
					Variable var{.name = std::get<token::Identifier>(code[0].first).value, .type = typeVar, .ParentFunc = ParentFunc};
					code.erase(code.begin()); //delete Identifier
					const std::string nameVar = Parser::VarToManglingName(var);

					res.data += nameVar + " resq 1\n";
					Variables.emplace(var.name, var);

					if (std::holds_alternative<token::Equal>(code[0].first)) {
						code.erase(code.begin());
						const Parser parser(this);
						Value val = parser.ParseExpression(code);
						if (typeVar->type != val.type->type) {
							throw std::runtime_error("Unexpected variable value type: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected: " + typeVar->type);
						}
						if (!std::holds_alternative<token::Semicolon>(code[0].first)) { //next token must be a Semicolon
							throw std::runtime_error("Unexpected token: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected Semicolon");
						}
						code.erase(code.begin());
						res.code += val.code;
						res.code += "mov [" + nameVar + "], rax\n";
						break;
					}
					throw std::runtime_error("Unexpected token: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second));
				}
				default:
					throw std::runtime_error("Unexpected : " + token::TokenToString(keyword) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second));
			}
		}),code[0].first);
		if (breaker) {
			break;
		}
	}

	return res;
}
