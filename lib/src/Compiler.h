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
		std::map<std::string,Variable> LocalVariables;
		std::map<std::string,Variable> PseudoVariables; //deleted variable
		std::vector<std::string> Labels;
		std::map<std::string, std::pair<unsigned int,unsigned short>> AntiLabels; //pair of name of label and location of label
		SpecificFunction ParentFunc;
		bool isMustReturn = true;
		bool isFunk = true;
		Type *typeReturn;
		unsigned int countIf = 0;
		bool isReturns = false;

		Compiler(const std::map<std::string,Type> &Types,
		const std::map<std::string,Function> &Functions,
		const std::map<std::string,Variable> &Variables,
		const std::map<std::string,Variable> &PseudoVariables,
		const std::vector<std::string> &Labels,
		SpecificFunction ParentFunc,
		Type* typeReturn,
		unsigned int countIf,
		const bool isMustReturn,
		const bool isFunk) :
		Types(Types), Functions(Functions), Variables(Variables), PseudoVariables(PseudoVariables), Labels(Labels), ParentFunc(std::move(ParentFunc)), isMustReturn(isMustReturn), countIf(countIf), isFunk(isFunk), typeReturn(typeReturn) {}

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
		void DeleteVariable(const std::pair<std::string,Variable>& var);

		[[nodiscard]] const std::map<std::string, Type>& getTypes() const {
			return Types;
		}
		[[nodiscard]] const std::map<std::string,Function>& getFunctions() const{
			return Functions;
		}
		[[nodiscard]] const std::map<std::string,Variable>& getVariables() const {
			return Variables;
		}
		[[nodiscard]] const std::map<std::string,Variable>& getPseudoVariables() const {
			return PseudoVariables;
		}
		[[nodiscard]] bool getIsReturns() const{
			return isReturns;
		}
		[[nodiscard]] SpecificFunction getParentFunc() const {
			return ParentFunc;
		}
		[[nodiscard]] const std::vector<std::string>& getLabels() const {
			return Labels;
		}
		[[nodiscard]] const std::map<std::string, std::pair<unsigned int,unsigned short>>& getAntiLabels() const {
			return AntiLabels;
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