#include "Compiler.h"

#include <algorithm>
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
		[&code, &breaker, this, &res](token::RCurlyBracket)  {
			if (isMustReturn) {
				throw std::runtime_error("Unexpected end of block of code, at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second));
			}
			if (isFunk)
			{
				res.code += "ret\n";
			}
			code.erase(code.begin());
			std::vector<std::string> varsNames;
			varsNames.reserve(Variables.size());
			for (const auto& var : Variables) {
				varsNames.push_back(var.first);
			}
			for (const auto& var : varsNames) {
				DeleteVariable(var);
			}
			breaker = true;
		},
		[&code, &breaker](token::Semicolon)  {
			code.erase(code.begin());
			breaker = true;
		},
		[&code, this, &res](token::Identifier)  {
			if (std::holds_alternative<token::LPar>(code[1].first)) {
				//function call
				const Parser parser(this);
				res.code += parser.FunctionCall(code).code;
				if (!std::holds_alternative<token::Semicolon>(code[0].first)) {
					throw std::runtime_error("Unexpected token: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected Semicolon");
				}
				code.erase(code.begin());
				return;
			}
			//variable definition
			SectionsCode var = VariableDefinition(code);
			res.code += var.code;
			res.data += var.data;
		},
		[&code, this, &res, &breaker](token::Keyword keyword) {
			code.erase(code.begin());
			switch(keyword) {
				case token::Keyword::VAR: {
					SectionsCode var = VariableAssignment(code);
					res.code += var.code;
					res.data += var.data;
					break;
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
					ifExpression.emplace_back(token::Semicolon {},position);

					Parser parser(this);

					Value val = parser.ParseExpression(ifExpression);
					if (val.type->type != "bool") {
						throw std::runtime_error("Unexpected type if expression, at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + " expected bool");
					}
					if (!std::holds_alternative<token::LCurlyBracket>(code[0].first)) {
						throw std::runtime_error("Unexpected token: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected LCurlyBracket");
					}
					code.erase(code.begin());
					Compiler ifCompiler(Types,Functions,Variables,PseudoVariables,ParentFunc,typeReturn,false, false);
					SectionsCode ifSectionsCode = ifCompiler.Compiling(code);
					res.data += ifSectionsCode.data;
					res.code += val.code;
					res.code += "cmp rax, 1\n";
					res.code += "jnz .no_if_label" + std::to_string(countIf - 1) + '\n';
					res.code += ifSectionsCode.code;
					res.code += ".no_if_label" + std::to_string(countIf - 1) + ":\n";
					break;
				}
				case token::Keyword::WHILE: {
					++countIf;
					if (!std::holds_alternative<token::LPar>(code[0].first)) {
						throw std::runtime_error("Unexpected token: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected LPar");
					}
					code.erase(code.begin());
					token::CODE ifExpression;
					std::pair<unsigned int,unsigned int> position;
					unsigned int levelBracket = 0;
					for (;;) {
						if (std::holds_alternative<token::RPar>(code[0].first) && levelBracket == 0) {
							position = code[0].second;
							break;
						}
						if (std::holds_alternative<token::LPar>(code[0].first) ||
							std::holds_alternative<token::LCurlyBracket>(code[0].first) ||
							std::holds_alternative<token::LSquareBracket>(code[0].first))
						{
							++levelBracket;
						}
						if (std::holds_alternative<token::RPar>(code[0].first) ||
							std::holds_alternative<token::RCurlyBracket>(code[0].first) ||
							std::holds_alternative<token::RSquareBracket>(code[0].first))
						{
							--levelBracket;
						}
						ifExpression.push_back(code[0]);
						code.erase(code.begin());
					}
					code.erase(code.begin());
					ifExpression.emplace_back(token::Semicolon {},position);

					Parser parser(this);

					Value val = parser.ParseExpression(ifExpression);
					if (val.type->type != "bool") {
						throw std::runtime_error("Unexpected type while expression, at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + " expected bool");
					}
					if (!std::holds_alternative<token::LCurlyBracket>(code[0].first)) {
						throw std::runtime_error("Unexpected token: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected LCurlyBracket");
					}
					code.erase(code.begin());
					Compiler whileCompiler(Types,Functions,Variables,PseudoVariables,ParentFunc,typeReturn,false, false);
					SectionsCode whileSectionsCode = whileCompiler.Compiling(code);
					res.data += whileSectionsCode.data;

					res.code += val.code;
					res.code += "cmp rax, 1\n";
					res.code += "jnz .no_while_label" + std::to_string(countIf - 1) + '\n';

					if (!whileCompiler.getIsReturns()) {
						res.code += ".while_label" + std::to_string(countIf - 1) + ":\n";
					}
					res.code += whileSectionsCode.code;

					if (!whileCompiler.getIsReturns()) {
						res.code += val.code;
						res.code += "cmp rax, 1\n";
						res.code += "jz .while_label" + std::to_string(countIf - 1) + '\n';
					}
					res.code += ".no_while_label" + std::to_string(countIf - 1) + ":\n";
					break;
				}
				case token::Keyword::FOR: {
					++countIf;
					if (!std::holds_alternative<token::LPar>(code[0].first)) {
						throw std::runtime_error("Unexpected token: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected LPar");
					}
					code.erase(code.begin());
					token::CODE varExpression;
					std::pair<unsigned int,unsigned int> position;
					unsigned int levelBracket = 0;
					for (;;) {
						if (std::holds_alternative<token::Semicolon>(code[0].first) && levelBracket == 0) {
							position = code[0].second;
							break;
						}
						if (std::holds_alternative<token::LPar>(code[0].first) ||
							std::holds_alternative<token::LCurlyBracket>(code[0].first) ||
							std::holds_alternative<token::LSquareBracket>(code[0].first))
						{
							++levelBracket;
						}
						if (std::holds_alternative<token::RPar>(code[0].first) ||
							std::holds_alternative<token::RCurlyBracket>(code[0].first) ||
							std::holds_alternative<token::RSquareBracket>(code[0].first))
						{
							--levelBracket;
						}
						varExpression.push_back(code[0]);
						code.erase(code.begin());
					}
					code.erase(code.begin());
					varExpression.emplace_back(token::Semicolon {},position);
					varExpression.emplace_back(token::Semicolon {},position);

					if (!std::holds_alternative<token::Keyword>(varExpression[0].first)) {
						throw std::runtime_error("Unexpected token: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected Keyword:VAR");
					}
					if (std::get<token::Keyword>(varExpression[0].first) != token::Keyword::VAR) {
						throw std::runtime_error("Unexpected token: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected Keyword:VAR");
					}
					varExpression.erase(varExpression.begin());

					SectionsCode varSections = VariableAssignment(varExpression);
					res.data += varSections.data;
					res.code += varSections.code;

					token::CODE ifExpression;
					for (;;) {
						if (std::holds_alternative<token::Semicolon>(code[0].first) && levelBracket == 0) {
							ifExpression.push_back(code[0]);
							code.erase(code.begin());
							break;
						}
						if (std::holds_alternative<token::LPar>(code[0].first) ||
							std::holds_alternative<token::LCurlyBracket>(code[0].first) ||
							std::holds_alternative<token::LSquareBracket>(code[0].first))
						{
							++levelBracket;
						}
						if (std::holds_alternative<token::RPar>(code[0].first) ||
							std::holds_alternative<token::RCurlyBracket>(code[0].first) ||
							std::holds_alternative<token::RSquareBracket>(code[0].first))
						{
							--levelBracket;
						}
						ifExpression.push_back(code[0]);
						code.erase(code.begin());
					}
					Parser parser(this);

					Value val = parser.ParseExpression(ifExpression);
					if (val.type->type != "bool") {
						throw std::runtime_error("Unexpected type for expression, at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + " expected bool");
					}

					token::CODE actionExpression;
					for (;;) {
						if (std::holds_alternative<token::RPar>(code[0].first) && levelBracket == 0) {
							position = code[0].second;
							break;
						}
						if (std::holds_alternative<token::LPar>(code[0].first) ||
							std::holds_alternative<token::LCurlyBracket>(code[0].first) ||
							std::holds_alternative<token::LSquareBracket>(code[0].first))
						{
							++levelBracket;
						}
						if (std::holds_alternative<token::RPar>(code[0].first) ||
							std::holds_alternative<token::RCurlyBracket>(code[0].first) ||
							std::holds_alternative<token::RSquareBracket>(code[0].first))
						{
							--levelBracket;
						}
						actionExpression.push_back(code[0]);
						code.erase(code.begin());
					}
					actionExpression.emplace_back(token::Semicolon {},position);
					actionExpression.emplace_back(token::Semicolon {},position);
					SectionsCode actionSectionsCode = Compiling(actionExpression);
					res.data += actionSectionsCode.data;

					if (!std::holds_alternative<token::RPar>(code[0].first)) {
						throw std::runtime_error("Unexpected token: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected RPar");
					}
					code.erase(code.begin());
					if (!std::holds_alternative<token::LCurlyBracket>(code[0].first)) {
						throw std::runtime_error("Unexpected token: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected LCurlyBracket");
					}
					code.erase(code.begin());

					Compiler forCompiler(Types,Functions,Variables,PseudoVariables,ParentFunc,typeReturn,false, false);
					SectionsCode forSectionsCode = forCompiler.Compiling(code);
					res.data += forSectionsCode.data;

					res.code += val.code;
					res.code += "cmp rax, 1\n";
					res.code += "jnz .no_for_label" + std::to_string(countIf - 1) + '\n';

					if (!forCompiler.getIsReturns()) {
						res.code += ".for_label" + std::to_string(countIf - 1) + ":\n";
					}
					res.code += forSectionsCode.code;
					if (!forCompiler.getIsReturns()) {
						res.code += actionSectionsCode.code;

						res.code += val.code;
						res.code += "cmp rax, 1\n";
						res.code += "jz .for_label" + std::to_string(countIf - 1) + '\n';
					}
					res.code += ".no_for_label" + std::to_string(countIf - 1) + ":\n";
					break;
				}
				case token::Keyword::RETURN: {
					isReturns = true;
					breaker = true;
					if (typeReturn->type == "void") {
						if (!std::holds_alternative<token::Semicolon>(code[0].first)) { //next token must be a Semicolon
							throw std::runtime_error("Unexpected token: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected Semicolon");
						}
						code.erase(code.begin());
					}
					else {
						Parser parser(this);
						Value val = parser.ParseExpression(code);
						if (val.type->type != typeReturn->type) {
							throw std::runtime_error("Unexpected type return expression, at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + " expected " + typeReturn->type);
						}
						if (!std::holds_alternative<token::Semicolon>(code[0].first)) { //next token must be a Semicolon
							throw std::runtime_error("Unexpected token: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected Semicolon");
						}
						code.erase(code.begin());
						res.code += val.code;
					}
					res.code += "ret\n";
					Compiler compiler(Types,Functions,Variables,PseudoVariables,ParentFunc,typeReturn);
					compiler.Compiling(code);
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

qlang::SectionsCode qlang::Compiler::VariableDefinition(token::CODE &code) {
	SectionsCode res{.code = "", .data = ""};
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
	return res;
}

qlang::SectionsCode qlang::Compiler::VariableAssignment(token::CODE &code) {
	SectionsCode res{.code = "", .data = ""};

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

	if (std::find(PseudoVariables.begin(),PseudoVariables.end(),nameVar) == PseudoVariables.end()) {
		res.data += nameVar + " resq 1\n";
	}
	else {
		PseudoVariables.erase(std::remove(PseudoVariables.begin(), PseudoVariables.end(), nameVar), PseudoVariables.end());
	}

	Variables.emplace(var.name, var);
	PseudoVariables.push_back(var.name);

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
		return res;
	}
	if (std::holds_alternative<token::Semicolon>(code[0].first)) {
		code.erase(code.begin());
		Variables[var.name].initialize = false;
		return res;
	}
	throw std::runtime_error("Unexpected token: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second));
}

void qlang::Compiler::DeleteVariable(const std::string& name) {
	PseudoVariables.push_back(name);
	Variables.erase(name);
}