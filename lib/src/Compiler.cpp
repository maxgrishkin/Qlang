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
		[&code, this, &res](token::Identifier identifier)  {
			if (std::holds_alternative<token::LPar>(code[1].first)) {
				//function call
				const Parser parser(this);
				res.code += parser.FunctionCall(code).code;
				return;
			}
			//variable definition
			std::string varName = std::get<token::Identifier>(code[0].first).value;
			code.erase(code.begin());
			if (!std::holds_alternative<token::Equal>(code[0].first)) {
				throw std::runtime_error("Unexpected token: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected Equal");
			}
			code.erase(code.begin());
			const Parser parser(this);
			Value val = parser.ParseExpression(code);
			if (Variables[varName].type->type != val.type->type) {
				throw std::runtime_error("Unexpected variable value type: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected: " + Variables[varName].type->type);
			}
			if (!std::holds_alternative<token::Semicolon>(code[0].first)) { //next token must be a Semicolon
				throw std::runtime_error("Unexpected token: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected Semicolon");
			}
			code.erase(code.begin());
			res.code += val.code;
			res.code += "mov [" + Parser::VarToManglingName(Variables[varName]) + "], rax\n";
			Variables[varName].initialize = true;
		},
		[&code, this, &res](token::Keyword keyword) {
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
						Variables[var.name].initialize = true;
						break;
					}
					if (std::holds_alternative<token::Semicolon>(code[0].first)) {
						code.erase(code.begin());
						Variables[var.name].initialize = false;
						break;
					}
					throw std::runtime_error("Unexpected token: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second));
				}
				case token::Keyword::IF: {
					++countIf;
					if (!std::holds_alternative<token::LPar>(code[0].first)) {
						throw std::runtime_error("Unexpected token: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected LPar");
					}
					code.erase(code.begin());
					token::CODE ifExpression;
					std::pair<unsigned int,unsigned int> position;
					unsigned int levelBracket = 0;
					for (;;) {
						if (std::holds_alternative<token::RPar>(code[0].first)) {
							if (levelBracket == 0) {
								position = code[0].second;
								break;
							}
							--levelBracket;
						}
						if (std::holds_alternative<token::LPar>(code[0].first) ||
							std::holds_alternative<token::LCurlyBracket>(code[0].first) ||
							std::holds_alternative<token::LSquareBracket>(code[0].first))
						{
							++levelBracket;
						}
						if (std::holds_alternative<token::RCurlyBracket>(code[0].first) ||
							std::holds_alternative<token::RSquareBracket>(code[0].first))
						{
							--levelBracket;
						}
						ifExpression.push_back(code[0]);
						code.erase(code.begin());
					}
					code.erase(code.begin());
					ifExpression.push_back(std::make_pair(token::Semicolon {},position));

					Parser parser(this);

					Value val = parser.ParseExpression(ifExpression);
					if (val.type->type != "bool") {
						throw std::runtime_error("Unexpected type if expression, at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + " expected bool");
					}
					if (!std::holds_alternative<token::LCurlyBracket>(code[0].first)) {
						throw std::runtime_error("Unexpected token: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected LCurlyBracket");
					}
					code.erase(code.begin());
					Compiler ifCompiler(Types,Functions,Variables,ParentFunc,countIf);
					SectionsCode ifSectionsCode = ifCompiler.Compiling(code);
					res.data += ifSectionsCode.data;
					res.code += val.code;
					res.code += "cmp rax, 1\n";
					res.code += "jnz .no_if_label" + std::to_string(countIf - 1) + '\n';
					res.code += ifSectionsCode.code;
					res.code += ".no_if_label" + std::to_string(countIf - 1) + ":\n";
					break;
				}
				default:
					throw std::runtime_error("Unexpected keyword: " + token::TokenToString(keyword) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second));
			}
		}),code[0].first);
		if (breaker) {
			break;
		}
	}

	return res;
}
