#include "highCompiler.h"

#include <fstream>
#include <iostream>

#include "Lexer.h"
#include "Parser.h"

qlang::SectionsCode qlang::highCompiler::Compiling(const std::string& path) {
	SectionsCode res{.data = "", .otherFunks = ""};

	std::ifstream file(path);
	if (!file.is_open()) {
		throw std::runtime_error("file not exist");
	}
	std::string strCode((std::istreambuf_iterator<char>(file)),
						 std::istreambuf_iterator<char>());
	file.close();

	token::CODE code = Lexer::Lex(strCode);

	while (!code.empty()) {
		std::visit(match(
		    [code](auto&& a)  {
		        throw std::runtime_error("Unexpected token: " + token::TokenToString(a) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second));
		    },
		    [&code, this, &res](token::Keyword keyword) {
		    	switch (keyword) {
		    		case token::Keyword::FN: {
		    			SectionsCode funk = FunctionsAssignment(code);
		    			res.data += funk.data;
		    			res.otherFunks += funk.otherFunks;
		    			break;
		    		}
		    		default:
		    			throw std::runtime_error("Unexpected  " + token::TokenToString(keyword) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second));
		    	}
		    }
		),code[0].first);
	}

	return res;
}

qlang::SectionsCode qlang::highCompiler::FunctionsAssignment(token::CODE &code) {
	if (!std::holds_alternative<token::Keyword>(code[0].first)) {
		throw std::runtime_error("Unexpected  " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + "expected keyword: FN");
	}
	if (std::get<token::Keyword>(code[0].first) != token::Keyword::FN) {
		throw std::runtime_error("Unexpected  " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + "expected keyword: FN");
	}
	code.erase(code.begin());

	Compiler kostilCompiler(Types, Functions, Variables, SpecificFunction {.TypeFunc = std::make_pair(std::vector<const Type *>{},&Types.at("void"))});
	const Type* retTypeFunk = &Types.at(kostilCompiler.take_type(code)->type);
	if (!std::holds_alternative<token::Identifier>(code[0].first)) {
		throw std::runtime_error("Unexpected  " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + "expected identifier");
	}
	std::string nameFunk = std::get<token::Identifier>(code[0].first).value;
	code.erase(code.begin());
	if (!std::holds_alternative<token::LPar>(code[0].first)) {
		throw std::runtime_error("Unexpected  " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + "expected LPar");
	}
	code.erase(code.begin());

	std::vector<Argument> args;
	for (;;) {
		if (std::holds_alternative<token::RPar>(code[0].first)) {
			code.erase(code.begin());
			break;
		}
		args.push_back(ArgAssignment(code,kostilCompiler));
	}
	if (!std::holds_alternative<token::LCurlyBracket>(code[0].first)) {
		throw std::runtime_error("Unexpected  " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + "expected LCurlyBracket");
	}
	code.erase(code.begin());


	std::map<std::vector<const Type *>, const Type *, TypeComparator> TypeFunc;
	std::vector<const Type*> inputTypeFunk;
	inputTypeFunk.reserve(args.size());
	for (const auto& arg : args) {
		inputTypeFunk.push_back(arg.type);
	}
	TypeFunc.emplace(inputTypeFunk,retTypeFunk);

	std::map<std::vector<const Type*>,std::vector<std::string>> argsFunk;
	std::vector<std::string> inputNameArgFunk;
	inputNameArgFunk.reserve(args.size());
	for (const auto& arg : args) {
		inputNameArgFunk.push_back(arg.name);
	}
	argsFunk.emplace(inputTypeFunk,inputNameArgFunk);

	Function function{.name = nameFunk, .TypeFunc = TypeFunc, .args = argsFunk};
	Functions.emplace(nameFunk,function);

	Compiler compiler = newCompiler(nameFunk,inputTypeFunk);
	SectionsCode sections_code = compiler.Compiling(code);
	if (args.size() > 6) {
		sections_code.otherFunks = "sub rsp, " + std::to_string((args.size() - 6) * 8) + sections_code.otherFunks;
	}
	return SectionsCode{.data = sections_code.data, .otherFunks = Parser::FuncToManglingName(Functions.at(nameFunk)[inputTypeFunk]) + ":\n" + sections_code.code};
}

qlang::Argument qlang::highCompiler::ArgAssignment(token::CODE &code, const Compiler &compiler) {
	token::CODE code2;
	for (;;) {
		if (std::holds_alternative<token::Comma>(code[0].first) || std::holds_alternative<token::RPar>(code[0].first)) {
			code2.emplace_back(token::Semicolon {},code[0].second);
			if (std::holds_alternative<token::Comma>(code[0].first)) {
				code.erase(code.begin());
			}
			break;
		}
		code2.push_back(code[0]);
		code.erase(code.begin());
	}

	if (!std::holds_alternative<token::Keyword>(code2[0].first)) {
		throw std::runtime_error("Unexpected  " + token::TokenToString(code2[0].first) + ", at line " + std::to_string(code2[0].second.first) + ", at symbol " + std::to_string(code2[0].second.second) + ", expected keyword: VAR");
	}
	if (std::get<token::Keyword>(code2[0].first) != token::Keyword::VAR) {
		throw std::runtime_error("Unexpected  " + token::TokenToString(code2[0].first) + ", at line " + std::to_string(code2[0].second.first) + ", at symbol " + std::to_string(code2[0].second.second) + ", expected keyword: VAR");
	}
	code2.erase(code2.begin());

	if (!std::holds_alternative<token::Identifier>(code2[0].first)) {
		throw std::runtime_error("Unexpected token: " + token::TokenToString(code2[0].first) + ", at line " + std::to_string(code2[0].second.first) + ", at symbol " + std::to_string(code2[0].second.second) + ", expected Identifier");
	}
	const Type* typeArg = &Types.at(compiler.take_type(code2)->type);

	//next token must be a name
	if (!std::holds_alternative<token::Identifier>(code2[0].first)) {
		throw std::runtime_error("Unexpected token: " + token::TokenToString(code2[0].first) + ", at line " + std::to_string(code2[0].second.first) + ", at symbol " + std::to_string(code2[0].second.second) + ", expected Identifier");
	}
	std::string nameArg = std::get<token::Identifier>(code2[0].first).value;
	code2.erase(code2.begin()); //delete Identifier

	if (!std::holds_alternative<token::Semicolon>(code2[0].first)) {
		throw std::runtime_error("Unexpected token: " + token::TokenToString(code2[0].first) + ", at line " + std::to_string(code2[0].second.first) + ", at symbol " + std::to_string(code2[0].second.second) + ", expected Semicolon");
	}

	return Argument{.name = nameArg, .type = typeArg};
}