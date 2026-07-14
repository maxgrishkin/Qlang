#pragma once
#include "Compiler.h"
#include "Type.h"

namespace qlang {
	class highCompiler {
	private:
		std::map<const std::string,Type> Types;
		std::map<const std::string,Function> Functions;
		std::map<const std::string,Variable> Variables;

		BinaryOperator PlusIntOperator;
		BinaryOperator MinusIntOperator;
		BinaryOperator MultiplyIntOperator;
		BinaryOperator DivisionIntOperator;
		BinaryOperator RemainderIntOperator;
	public:
		highCompiler() {
			Types = {
				{"char",Type{.type = "char"}},
				{"void",Type{.type = "void"}},
				{"int",Type{.type = "int"}}
			};

			const std::map<const Type*,const Type*,TypeComparator> TypeOpers({{&Types["int"],&Types["int"]}},TypeComparator{});

			PlusIntOperator = BinaryOperator{.TypeOper = TypeOpers,.asmCode = {{ &Types["int"], "add rax, rbx\n"}}};
			MinusIntOperator = BinaryOperator{.TypeOper = TypeOpers,.asmCode = {{ &Types["int"], "sub rax, rbx\n"}}};
			MultiplyIntOperator = BinaryOperator{.TypeOper = TypeOpers,.asmCode = {{ &Types["int"], "mul rbx\n"}}};
			DivisionIntOperator = BinaryOperator{.TypeOper = TypeOpers,.asmCode = {{ &Types["int"], "mov rdx, 0\ndiv rbx\n"}}};
			RemainderIntOperator = BinaryOperator{.TypeOper = TypeOpers,.asmCode = {{ &Types["int"], "mov rdx, 0\ndiv rbx\nmov rax, rdx\n"}}};

			const std::map<std::string,BinaryOperator*> intOperators = {
				{"+",&PlusIntOperator},
				{"-",&MinusIntOperator},
				{"*",&MultiplyIntOperator},
				{"/",&DivisionIntOperator},
				{"%",&RemainderIntOperator}
			};

			Types["int"].operators = intOperators;

			//TODO ubrat kostil
			Function Main{.name = "main"};
			std::map<std::vector<const Type*>,const Type*,TypeComparator> TypeFunc;
			std::vector<const Type*> inputMain = {};
			TypeFunc.emplace(inputMain,&Types.at("int"));
			Functions.emplace("main",Main);
		}

		Compiler newCompiler(const std::string& nameFunc) const {
			Compiler compiler = Compiler(Types,Functions,Variables,Functions.at(nameFunc)[{}]);
			return compiler;
		}

		std::map<const std::string,Type> getTypes() {
			return Types;
		}
		std::map<const std::string,Function> getFunctions() {
			return Functions;
		}
		std::map<const std::string,Variable> getVariables() {
			return  Variables;
		}
	};
}
