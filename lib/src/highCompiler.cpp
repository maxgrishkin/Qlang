#include "highCompiler.h"

#include <fstream>

#include "HederCompiler.h"
#include "Lexer.h"
#include "Parser.h"

std::vector<std::string> qlang::highCompiler::Compiling(const std::string &path, bool Main) {
	SectionsCode res{.externSection = "",.data = "", .otherFunks = ""};
	std::vector<std::string> oF;

	std::string subPath;
	std::string nameFile;
	{
		int Index = -1;
		for (unsigned int i = path.size();;--i) {
			if (path[i] == '/') {
				Index = i;
				break;
			}
			if (i == 0) {
				break;
			}
		}
		if (path[0] != '/') {
			subPath = "./";
		}
		if (Index != -1) {
			for (int i = 0;i <= Index;++i) {
				subPath += path[i];
			}
			for (unsigned int i = Index + 1;i < path.size();++i) {
				if (path[i] == '.') {
					break;
				}
				nameFile += path[i];
			}
		}
		else {
			nameFile = path;
		}
	}
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
		    },
		    [&code, subPath, &oF, this, &res](token::Hash)  {
				code.erase(code.begin());
		    	if (std::holds_alternative<token::Keyword>(code[0].first)) {
		    		token::Keyword keyword = std::get<token::Keyword>(code[0].first);
					code.erase(code.begin());
		    		switch (keyword) {
						case token::Keyword::INCLUDE: {
							if (!std::holds_alternative<token::LAngleBracket>(code[0].first)) {
								throw std::runtime_error("Unexpected token:" + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected LAngleBracket");
							}
							code.erase(code.begin());
							std::string IncludeFile;
							while (!std::holds_alternative<token::RAngleBracket>(code[0].first)) {
								if (std::holds_alternative<token::Identifier>(code[0].first)) {
									IncludeFile += std::get<token::Identifier>(code[0].first).value;
									code.erase(code.begin());
									continue;
								}
								IncludeFile += token::TokenToString(code[0].first);
								code.erase(code.begin());
							}
							code.erase(code.begin());
							if (!(IncludeFile[IncludeFile.size() - 4] == '.' &&
							IncludeFile[IncludeFile.size() - 3] == 'h' &&
							IncludeFile[IncludeFile.size() - 2] == 'e' && IncludeFile[IncludeFile.size() - 1] == 'd'))
							{
								throw std::runtime_error("Unexpected file extension, at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected LAngleBracket");
							}

							std::ifstream file(subPath + IncludeFile);
							if (!file.is_open()) {
								throw std::runtime_error("file not exist");
							}
							std::string strCode((std::istreambuf_iterator<char>(file)),
												 std::istreambuf_iterator<char>());
							token::CODE headerCode = Lexer::Lex(strCode);
							HederCompiler heder_compiler(Types);
							auto heders = heder_compiler.Compiling(subPath,headerCode);
							for (const auto& heder : heders.first) {
								oF.push_back(heder);
							}
							for (const auto& type : heders.second.Types) {
								Types.emplace(type);
							}
							for (auto& func : heders.second.Functions) {
								std::pair<std::string, Function> func2;
								func2.first = func.first;
								func2.second.name = func.second.name;
								std::pair<std::vector<const Type*>,const Type*> funcType;
								for (const auto& type : func.second.TypeFunc) {
									funcType.second = &Types[type.second->type];
									for (const auto& typeI : type.first) {
										funcType.first.push_back(&Types[typeI->type]);
									}
									func2.second.TypeFunc.emplace(funcType);
								}
								res.externSection += "extern " + Parser::FuncToManglingName(func.second[funcType.first]) + '\n';
								Functions.emplace(func2);
							}
							for (const auto& var : heders.second.Variables) {
								Variables.emplace(var);
								Variables[var.first].type = &Types[Variables[var.first].type->type];
							}
							break;
						}
						default:
		    				throw std::runtime_error("Unexpected  " + token::TokenToString(keyword) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second));
					}
		    		return;
		    	}
		    	throw std::runtime_error("Unexpected token: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second));
			}
		),code[0].first);
	}

	system(("touch " + nameFile + ".asm").c_str());
	std::ofstream OFile(nameFile + ".asm");
	if (!OFile.is_open()) {
		throw std::runtime_error("file not exist");
	}

	std::vector<const Type*> argMain;
	for (const auto& args : Functions.at("main").TypeFunc) {
		argMain = args.first;
		break;
	}

	if (Main) {
		OFile << "global _start\n";
	}
	OFile << res.externSection;
	OFile << "section .bss\n" + res.data;
	if (Main) {
		OFile << "section .text\n_start:\ncall " + Parser::FuncToManglingName(Functions.at("main")[argMain]) + "\nmov rdi, rax\nmov rax, 60\nsyscall\n";
	}
	OFile << res.otherFunks;
	OFile.close();

	system(("nasm -f elf64 -g " + nameFile + ".asm  -o " + nameFile + ".o").c_str());
	oF.push_back(nameFile + ".o");
	system(("rm " + nameFile + ".asm").c_str());

	return oF;
}

qlang::SectionsCode qlang::highCompiler::FunctionsAssignment(token::CODE &code) {
	if (!std::holds_alternative<token::Keyword>(code[0].first)) {
		throw std::runtime_error("Unexpected  " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected keyword: FN");
	}
	if (std::get<token::Keyword>(code[0].first) != token::Keyword::FN) {
		throw std::runtime_error("Unexpected  " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected keyword: FN");
	}
	code.erase(code.begin());

	Compiler kostilCompiler(Types, Functions, Variables, SpecificFunction {.TypeFunc = std::make_pair(std::vector<const Type *>{},&Types.at("void"))});
	const Type* retTypeFunk = &Types.at(kostilCompiler.take_type(code)->type);
	if (!std::holds_alternative<token::Identifier>(code[0].first)) {
		throw std::runtime_error("Unexpected  " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected identifier");
	}
	std::string nameFunk = std::get<token::Identifier>(code[0].first).value;
	code.erase(code.begin());
	if (!std::holds_alternative<token::LPar>(code[0].first)) {
		throw std::runtime_error("Unexpected  " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected LPar");
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
		throw std::runtime_error("Unexpected  " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected LCurlyBracket");
	}
	code.erase(code.begin());


	std::pair<std::vector<const Type *>, const Type *> TypeFunc;
	std::vector<const Type*> inputTypeFunk;
	inputTypeFunk.reserve(args.size());
	for (const auto& arg : args) {
		inputTypeFunk.push_back(arg.type);
	}
	TypeFunc = std::make_pair(inputTypeFunk,retTypeFunk);

	std::pair<std::vector<const Type*>,std::vector<std::string>> argsFunk;
	std::vector<std::string> inputNameArgFunk;
	inputNameArgFunk.reserve(args.size());
	for (const auto& arg : args) {
		inputNameArgFunk.push_back(arg.name);
	}
	argsFunk = std::make_pair(inputTypeFunk,inputNameArgFunk);


	if (Functions.count(nameFunk) == 0) {
		Function function{.name = nameFunk, .TypeFunc = {TypeFunc}, .args = {argsFunk}};
		Functions.emplace(nameFunk,function);
	}
	else {
		if (Functions[nameFunk].args.count(inputTypeFunk) == 1) { //if a function with the same arguments already exists
			throw std::runtime_error("A function with the same arguments already exists");
		}
		Functions[nameFunk].args.emplace(argsFunk);
		Functions[nameFunk].TypeFunc.emplace(TypeFunc);
	}

	Compiler compiler = newCompiler(nameFunk,inputTypeFunk);
	SectionsCode sections_code = compiler.Compiling(code);

	return SectionsCode{.externSection = "global " + Parser::FuncToManglingName(Functions.at(nameFunk)[inputTypeFunk]) + '\n',.data = sections_code.data, .otherFunks = Parser::FuncToManglingName(Functions.at(nameFunk)[inputTypeFunk]) + ":\n" + sections_code.code};
}

qlang::Argument qlang::highCompiler::ArgAssignment(token::CODE &code, const Compiler &compiler) const {
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
	const std::string nameArg = std::get<token::Identifier>(code2[0].first).value;
	code2.erase(code2.begin()); //delete Identifier

	if (!std::holds_alternative<token::Semicolon>(code2[0].first)) {
		throw std::runtime_error("Unexpected token: " + token::TokenToString(code2[0].first) + ", at line " + std::to_string(code2[0].second.first) + ", at symbol " + std::to_string(code2[0].second.second) + ", expected Semicolon");
	}

	return Argument{.name = nameArg, .type = typeArg};
}