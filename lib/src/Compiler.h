#pragma once

#include <stdexcept>
#include <utility>

#include "Type.h"

namespace qlang{
	class Compiler {
	private:
		std::map<const std::string,Type> Types;
		std::map<const std::string,Function> Functions;
		std::map<const std::string,Variable> Variables;
		SpecificFunction ParentFunc;
	public:

		Compiler(const std::map<const std::string,Type> &Types,
		const std::map<const std::string,Function> &Functions,
		const std::map<const std::string,Variable> &Variables,
		SpecificFunction ParentFunc) :
		Types(Types), Functions(Functions), Variables(Variables), ParentFunc(std::move(ParentFunc)) {}

		SectionsCode Compiling(token::CODE& code);

		[[nodiscard]] const std::map<const std::string, Type>& getTypes() const {
			return Types;
		}
		[[nodiscard]] const std::map<const std::string,Function>& getFunctions() const{
			return Functions;
		}
		[[nodiscard]] const std::map<const std::string,Variable>& getVariables() const {
			return  Variables;
		}

		const Type* take_type(token::CODE& code) const {
			std::string type;
			std::visit(match(
			[code](auto&& a) {
				throw std::runtime_error("Unexpected token: " + token::TokenToString(a) + ", at line " + std::to_string(code[0].second.first) + ", at symbol " + std::to_string(code[0].second.second));
			},
			[&code,&type](token::Identifier identifier) {
				code.erase(code.begin());
				type = identifier.value;
			}
			),code[0].first);
			return &Types.at(type);
		}
	};
}