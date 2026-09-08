#pragma once
#include "Compiler.h"
#include "token.h"

namespace qlang{
	class Parser {
	private:
		const Compiler* compiler;
		Value ParseExpression(token::CODE& code, int left_binding_power) const;
	public:
		explicit Parser(const Compiler* compiler) : compiler(compiler) {}
		[[nodiscard]] Value ParseExpression(token::CODE& code) const {
			return ParseExpression(code, 0);
		}

		static std::string FuncToManglingName(const SpecificFunction& Func) {
			std::string res = "_";
			for (const auto* const type : Func.TypeFunc.first) {
				res += std::to_string(type->type.size()) + '_' + type->type;
			}
			res += std::to_string(Func.name.size()) + '_' + Func.name;
			return res;
		}
		static std::string VarToManglingName(const Variable& var) {
			std::string res = FuncToManglingName(var.ParentFunc);

			res += '_' + std::to_string(var.type->type.size()) + var.type->type + '_' + std::to_string(var.name.size()) + var.name;
			return res;
		}

		Value FunctionCall(token::CODE& code) const;
		Value VariableReference(token::CODE& code) const;
	};
}