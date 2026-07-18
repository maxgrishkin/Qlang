#pragma once

#include <iostream>
#include <stdexcept>
#include <utility>

#include "Type.h"

namespace qlang{
	class Compiler {
	private:
		std::map<std::string,Type> Types;
		std::map<std::string,Function> Functions;
		std::map<std::string,Variable> Variables;
		std::vector<std::string> PseudoVariables; //deleted variable
		SpecificFunction ParentFunc;
		bool isMustReturn = true;
		bool isFunk = true;
		Type *typeReturn;
		unsigned int countIf = 0;
		bool isReturns = false;

		Compiler(const std::map<std::string,Type> &Types,
		const std::map<std::string,Function> &Functions,
		const std::map<std::string,Variable> &Variables,
		const std::vector<std::string> &PseudoVariables,
		const SpecificFunction &ParentFunc,
		Type* typeReturn,
		const bool isMustReturn = false,
		const bool isFunk = true) :
		Types(Types), Functions(Functions), Variables(Variables), PseudoVariables(PseudoVariables), ParentFunc(ParentFunc), isMustReturn(isMustReturn), isFunk(isFunk), typeReturn(typeReturn) {}

	public:
		Compiler(const std::map<std::string,Type> &Types,
		const std::map<std::string,Function> &Functions,
		const std::map<std::string,Variable> &Variables,
		const SpecificFunction& ParentFunc) :
		Types(Types), Functions(Functions), Variables(Variables), ParentFunc(ParentFunc), typeReturn(const_cast<Type *>(ParentFunc.TypeFunc.second)) {isMustReturn = typeReturn->type != "void";}

		Compiler() = delete;

		SectionsCode Compiling(token::CODE& code);
		SectionsCode VariableDefinition(token::CODE& code);
		SectionsCode VariableAssignment(token::CODE& code);
		void DeleteVariable(const std::string& name);

		[[nodiscard]] const std::map<std::string, Type>& getTypes() const {
			return Types;
		}
		[[nodiscard]] const std::map<std::string,Function>& getFunctions() const{
			return Functions;
		}
		[[nodiscard]] const std::map<std::string,Variable>& getVariables() const {
			return Variables;
		}
		[[nodiscard]] const std::vector<std::string>& getPseudoVariables() const {
			return PseudoVariables;
		}
		[[nodiscard]] bool getIsReturns() const{
			return isReturns;
		}
		[[nodiscard]] SpecificFunction getParentFunc() const {
			return ParentFunc;
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