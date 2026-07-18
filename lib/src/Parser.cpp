#include "Parser.h"

#include <memory>
#include <stdexcept>

qlang::Value qlang::Parser::ParseExpression(token::CODE& code, int left_binding_power) const {
	int right_binding_power = 0;
	auto Element = [&code, this](const int rbp) -> Value {
		return std::visit(match(
			[&](auto&&) -> Value {
				throw std::runtime_error("Unexpected token: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second));
			},
			[&](token::Integer Integer) -> Value {
				code.erase(code.begin());
				return Value{.code = "mov rax," + std::to_string(Integer.value) + '\n',.type =  &compiler->getTypes().at("int")};
			},
			[&](token::Char Char) -> Value {
				code.erase(code.begin());
				const std::string value = std::to_string(static_cast<int>(Char.value));
				return Value{.code = "mov rax," + value + '\n',.type = &compiler->getTypes().at("char")};
			},
			[&](token::Identifier identifier) -> Value {
				if (compiler->getFunctions().find(identifier.value) != compiler->getFunctions().end()) {
					if (std::holds_alternative<token::LPar>(code[1].first)) {
						return FunctionCall(code);
					}
				}
				return VariableReference(code);
			},
			[&](token::LPar) -> Value {
				code.erase(code.begin());
				auto value = ParseExpression(code,rbp);
				return value;
			}
		),code[0].first);
	};
	auto generateBinaryOperator = [&code, this](const Type* lType, const std::string& _operator, const bool leftAssociativity, const int rbp) -> Value {
		const unsigned int line = code[0].second.first;
		const unsigned int symbol = code[0].second.second;
		code.erase(code.begin());
		auto val = ParseExpression(code,rbp + static_cast<int>(leftAssociativity));
		auto typeOper = lType->operators.find(_operator);
		if (typeOper == lType->operators.end())
			throw std::runtime_error("This operator not exists, at line " + std::to_string(line) + ", at symbol " + std::to_string(symbol));
		std::string resStr = "push rax\n";
		resStr += val.code;
		resStr += "mov rbx,rax\n";
		resStr += "pop rax\n";
		auto op = lType->operators.find(_operator);
		resStr += op->second->asmCode[val.type];
		return Value{.code = resStr, .type = typeOper->second->TypeOper.at(val.type)};
	};
	auto Operator = [&code, generateBinaryOperator](const Type* lType, const int rbp, const int lbp) -> Value {
		if (lbp > rbp) {
			return Value{.code = "",.type = nullptr};
		}
		return std::visit(match(
		[&](auto&&) -> Value {
			throw std::runtime_error("Unexpected token: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second));
		},
		[&](token::Semicolon) -> Value {
			return Value{.code = "", .type = nullptr};
		},
		[&](token::Plus) -> Value {
			return generateBinaryOperator(lType, "+", true, rbp);
		},
		[&](token::Minus) -> Value {
			return generateBinaryOperator(lType, "-", true, rbp);
		},
		[&](token::Asterisk) -> Value {
			return generateBinaryOperator(lType, "*", true, rbp);
		},
		[&](token::Slash) -> Value {
			return generateBinaryOperator(lType, "/", true, rbp);
		},
		[&](token::Percent) -> Value {
			return generateBinaryOperator(lType, "%", true, rbp);
		},
		[&](token::Equal) -> Value {
			return generateBinaryOperator(lType, "==", true, rbp);
		},
		[&](token::NoEqual) -> Value {
			return generateBinaryOperator(lType, "!=", true, rbp);
		},
		[&](token::LAngleBracket) -> Value {
			return generateBinaryOperator(lType, "<", true, rbp);
		},
		[&](token::RAngleBracket) -> Value {
			return generateBinaryOperator(lType, ">", true, rbp);
		},
		[&](token::LAngleBracketEqual) -> Value {
			return generateBinaryOperator(lType, "<=", true, rbp);
		},
		[&](token::RAngleBracketEqual) -> Value {
			return generateBinaryOperator(lType, ">=", true, rbp);
		},
		[&](token::RPar) -> Value {
			code.erase(code.begin());
			return Value{.code = "", .type = nullptr};
		}
		),code[0].first);
	};

	std::string ASMCode;

	auto el = Element(right_binding_power);
	ASMCode += el.code;
	const Type *type = el.type;
	if (!code.empty()) {
		for (;;) {
			right_binding_power = operatorsPower[code[0].first];
			auto op = Operator(type, right_binding_power, left_binding_power);
			if (op.type == nullptr)
				break;
			ASMCode += op.code;
			type = op.type;
		}
	}

	return Value{.code = ASMCode, .type = type};
}

qlang::Value qlang::Parser::VariableReference(token::CODE &code) const {
	auto position = code[0].second;
	const std::string name = std::visit(
	match(
		[&](auto&&) -> std::string {
			throw std::runtime_error("Unexpected token: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second));
		},
		[&](token::Identifier identifier) -> std::string {
			code.erase(code.begin());
			return identifier.value;
		}
		), code[0].first
	);
	auto Variables = compiler->getVariables();
	auto var = Variables.find(name);
	if (var != Variables.end()) {
		if (!var->second.initialize) {
			throw std::runtime_error("Use not initialize variable (you invalid), at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second));
		}
		return Value{.code = "mov rax, qword [" + VarToManglingName(var->second) + "]\n", .type = var->second.type};
	}
	const auto args = compiler->getParentFunc().args.second;
	for (unsigned int i = 0;i < args.size();++i) {
		if (args[i] == name) {
			auto type = const_cast<Type*>(compiler->getParentFunc().args.first[i]);
			std::string asmCode;
			if (i + 1 == 1) {
				asmCode += "mov rax, rdi \n";
			}
			else if (i + 1 == 2) {
				asmCode += "mov rax,rsi\n";
			}
			else if (i + 1 == 3) {
				asmCode += "mov rax,rdx\n";
			}
			else if (i + 1 == 4) {
				asmCode += "mov rax,rcx\n";
			}
			else if (i + 1 == 5) {
				asmCode += "mov rax,r8\n";
			}
			else if (i + 1 == 6) {
				asmCode += "mov rax,r9\n";
			}
			else if (i + 1 > 6) {
				asmCode += "mov rax,[rbp + " + std::to_string((args.size() - i) * 8) + "]\n";
			}
			return Value{.code = asmCode,.type = type};
		}
	}
	throw std::runtime_error("Variable is not exist (you invalid), at line " + std::to_string(position.first) + ", at symbol " + std::to_string(position.second));
}

qlang::Value qlang::Parser::FunctionCall(token::CODE &code) const {
	const std::string name = std::visit(
	match(
		[&](auto&&) -> std::string {
			throw std::runtime_error("Unexpected token: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second));
		},
		[&](token::Identifier identifier) -> std::string {
			code.erase(code.begin());
			return identifier.value;
		}
		), code[0].first
	);
	if (!std::holds_alternative<token::LPar>(code[0].first)) { //next token must be a Semicolon
		throw std::runtime_error("Unexpected token: " + token::TokenToString(code[0].first) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second) + ", expected LPar");
	}
	code.erase(code.begin());

	std::string finishCode = "push rdi\npush rsi\npush rdx\npush rcx\npush r8\npush r9\n";
	std::vector<const Type*> TypesFunc;
	unsigned char arguments = 0;

	for (;;) {
		token::CODE expression{};
		{
			int bracketLevel = 0;
			while (!(bracketLevel == 0 && (std::holds_alternative<token::Comma>(code[0].first) || std::holds_alternative<token::RPar>(code[0].first)))) {
				if (std::holds_alternative<token::LPar>(code[0].first) ||
				    std::holds_alternative<token::LSquareBracket>(code[0].first) ||
				    std::holds_alternative<token::LCurlyBracket>(code[0].first)) {
					++bracketLevel;
				}
				else if (std::holds_alternative<token::RPar>(code[0].first) ||
				std::holds_alternative<token::RSquareBracket>(code[0].first) ||
				std::holds_alternative<token::RCurlyBracket>(code[0].first)) {
					--bracketLevel;
				}
				expression.push_back(code[0]);
				code.erase(code.begin());
			}
		}
		if (expression.empty() && std::holds_alternative<token::Comma>(code[0].first)) {
			throw std::runtime_error("empty argument, at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second));
		}
		if (expression.empty() && std::holds_alternative<token::RPar>(code[0].first) && arguments != 0) {
			throw std::runtime_error("empty argument, at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second));
		}
		if (expression.empty() && std::holds_alternative<token::RPar>(code[0].first) && arguments == 0) {
			code.erase(code.begin());
			break;
		}
		bool isEmptyExpression = expression.empty();
		++arguments;
		expression.emplace_back(token::Semicolon {},std::make_pair(0,0));
		const Value val = ParseExpression(expression, 0);
		TypesFunc.push_back(val.type);
		finishCode += val.code;
		if (arguments == 1) {
			finishCode += "mov rdi, rax\n";
		}
		else if (arguments == 2) {
			finishCode += "mov rsi, rax\n";
		}
		else if (arguments == 3) {
			finishCode += "mov rdx, rax\n";
		}
		else if (arguments == 4) {
			finishCode += "mov rcx, rax\n";
		}
		else if (arguments == 5) {
			finishCode += "mov r8, rax\n";
		}
		else if (arguments == 6) {
			finishCode += "mov r9, rax\n";
		}
		else {
			finishCode += "push rax\n";
		}
		if (!isEmptyExpression && std::holds_alternative<token::RPar>(code[0].first)) {
			code.erase(code.begin());
			break;
		}
		code.erase(code.begin());
	}
	if (arguments > 6) {
		finishCode += "sub rsp, 8\n" + finishCode + "add rsp, " + std::to_string((arguments - 6) * 8 + 8);
	}
	auto Functions = compiler->getFunctions();
	auto func = Functions.find(name);
	if (func == Functions.end()) {
		throw std::runtime_error("Function missing, at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second));
	}
	finishCode += "call " + FuncToManglingName(func->second[TypesFunc]) + "\n";
	finishCode += "pop r9\npop r8\npop rcx\npop rdx\npop rsi\npop rdi\n";
	return Value {.code = finishCode,.type = func->second[TypesFunc].TypeFunc.second};
}