#pragma once

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

		[[nodiscard]] const std::map<const std::string, Type>& getTypes() const {
			return Types;
		}
		[[nodiscard]] const std::map<const std::string,Function>& getFunctions() const{
			return Functions;
		}
		[[nodiscard]] const std::map<const std::string,Variable>& getVariables() const {
			return  Variables;
		}
	};
}