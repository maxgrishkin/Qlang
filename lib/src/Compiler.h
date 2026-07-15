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
		std::vector<std::string> PseudoVariables; //deleted variable
		SpecificFunction ParentFunc;
		unsigned int& countIf;
	public:

		Compiler(const std::map<const std::string,Type> &Types,
		const std::map<const std::string,Function> &Functions,
		const std::map<const std::string,Variable> &Variables,
		const std::vector<std::string> &PseudoVariables,
		SpecificFunction ParentFunc,
		unsigned int& countIf) :
		Types(Types), Functions(Functions), Variables(Variables), PseudoVariables(PseudoVariables), ParentFunc(std::move(ParentFunc)), countIf(countIf) {}

		Compiler(const std::map<const std::string,Type> &Types,
		const std::map<const std::string,Function> &Functions,
		const std::map<const std::string,Variable> &Variables,
		SpecificFunction ParentFunc,
		unsigned int& countIf) :
		Types(Types), Functions(Functions), Variables(Variables), ParentFunc(std::move(ParentFunc)), countIf(countIf) {}

		Compiler() = delete;

		SectionsCode Compiling(token::CODE& code);
		SectionsCode VariableDefinition(token::CODE& code);
		SectionsCode VariableAssignment(token::CODE& code);
		void DeleteVariable(const std::string& name);

		[[nodiscard]] const std::map<const std::string, Type>& getTypes() const {
			return Types;
		}
		[[nodiscard]] const std::map<const std::string,Function>& getFunctions() const{
			return Functions;
		}
		[[nodiscard]] const std::map<const std::string,Variable>& getVariables() const {
			return Variables;
		}
		[[nodiscard]] const std::vector<std::string>& getPseudoVariables() const {
			return PseudoVariables;
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