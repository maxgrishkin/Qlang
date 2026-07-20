#include "HederCompiler.h"

#include <fstream>

#include "Compiler.h"

std::pair<std::vector<std::string>, qlang::Context> qlang::HederCompiler::Compiling(const std::string &Directory, token::CODE &code) {
	std::vector<std::string> oFiles;
	Context context;

	while (!code.empty()) {
		if (!std::holds_alternative<token::Dot>(code[0].first)) {
			throw std::runtime_error("Unexpected  " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected Dot");
		}
		code.erase(code.begin());
		std::visit(match(
		    [code](auto&& a)  {
		        throw std::runtime_error("Unexpected token: " + token::TokenToString(a) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second));
		    },
		    [&code, this, &oFiles, &context, Directory](token::Identifier identifier) {
		    	code.erase(code.begin());
				if (identifier.value == "asm") {
					auto assembly = AssemblyProcessing(Directory, code);
					oFiles.push_back(assembly.first);
					for (const auto& func : assembly.second.Functions) {
						for (auto& function : assembly.second.Functions) {
							context.Functions.emplace(function);
						}
					}
					return;
				}
		    	throw std::runtime_error("Unexpected  " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second));
		    }
		),code[0].first);
	}

	return std::make_pair(oFiles, context);
}

std::pair<std::string, qlang::Context> qlang::HederCompiler::AssemblyProcessing(const std::string &Directory, token::CODE &code) {
	if (!std::holds_alternative<token::LPar>(code[0].first)) {
		throw std::runtime_error("Unexpected  token: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected LPar");
	}
	code.erase(code.begin());
	token::CODE name;
	for (;;) {
		if (std::holds_alternative<token::Comma>(code[0].first)) {
			code.erase(code.begin());
			break;
		}
		name.push_back(code[0]);
		code.erase(code.begin());
	}
	std::string strName;
	std::string strNameWithoutDot;
	bool didDot = false;
	for (auto token : name) {
		if (std::holds_alternative<token::Identifier>(token.first)) {
			if (!didDot) {
				strNameWithoutDot += std::get<token::Identifier>(token.first).value;
			}
			strName += std::get<token::Identifier>(token.first).value;
			continue;
		}
		if (std::holds_alternative<token::Dot>(token.first)) {
			didDot = true;
		}
		strName += token::TokenToString(token.first);
		if (!didDot) {
			strNameWithoutDot += token::TokenToString(token.first);
		}
	}
	system(("nasm -f elf64 " + Directory + strName + " -o " + strNameWithoutDot + ".o").c_str());

	std::string nameOFile = strNameWithoutDot + ".o";
	Context context = ContextProcessing(code);
	if (!std::holds_alternative<token::Semicolon>(code[0].first)) {
		throw std::runtime_error("Unexpected  " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected Semicolon");
	}
	code.erase(code.begin());

	return std::make_pair(nameOFile,context);
}

qlang::Context qlang::HederCompiler::ContextProcessing(token::CODE &code) {
	Context context;

	for (;;) {
		if (!std::holds_alternative<token::Keyword>(code[0].first)) {
			throw std::runtime_error("Unexpected  " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected Keyword: fn");
		}
		if (std::get<token::Keyword>(code[0].first) != token::Keyword::FN) {
			throw std::runtime_error("Unexpected  " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected Keyword: fn");
		}
		code.erase(code.begin());

		Type* typeRetFunc;
		std::string nameFunc;
		std::vector<const Type*> typeArgFunc;
		{
			Type kostilType{.type = "void"};
			Compiler kostilCompiler(Types,context.Functions,context.Variables, SpecificFunction{.TypeFunc = std::make_pair<std::vector<const Type *>, const Type *>({},&kostilType)});
			typeRetFunc = &Types[kostilCompiler.take_type(code)->type];
			if (!std::holds_alternative<token::Identifier>(code[0].first)) {
				throw std::runtime_error("Unexpected  " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected Identifier");
			}
			nameFunc = std::get<token::Identifier>(code[0].first).value;
			code.erase(code.begin());
			if (!std::holds_alternative<token::LPar>(code[0].first)) {
				throw std::runtime_error("Unexpected token:" + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected LPar");
			}
			code.erase(code.begin());
			for (;;) {
				typeArgFunc.push_back(&Types[kostilCompiler.take_type(code)->type]);
				if (std::holds_alternative<token::RPar>(code[0].first)) {
					code.erase(code.begin());
					break;
				}
				if (std::holds_alternative<token::Comma>(code[0].first)) {
					code.erase(code.begin());
					continue;
				}
				throw std::runtime_error("Unexpected token:" + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second));
			}
		}
		Function function{.name = nameFunc,.TypeFunc = {std::make_pair(typeArgFunc,const_cast<const Type*>(typeRetFunc))} };
		context.Functions.emplace(nameFunc,function);
		if (std::holds_alternative<token::Comma>(code[0].first)) {
			code.erase(code.begin());
			continue;
		}
		if (std::holds_alternative<token::RPar>(code[0].first)) {
			code.erase(code.begin());
			break;
		}
	}
	return context;
}