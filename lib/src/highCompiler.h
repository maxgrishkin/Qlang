#pragma once
#include "Compiler.h"
#include "Type.h"

namespace qlang {
	class highCompiler {
	private:
		std::map<const std::string,Type> Types;
		std::map<const std::string,Function> Functions;
		std::map<const std::string,Variable> Variables;
		unsigned int countIf = 0;

		BinaryOperator PlusIntOperator;
		BinaryOperator MinusIntOperator;
		BinaryOperator MultiplyIntOperator;
		BinaryOperator DivisionIntOperator;
		BinaryOperator RemainderIntOperator;

		BinaryOperator LessIntOperator;
		BinaryOperator GreatIntOperator;
		BinaryOperator EqualIntOperator;
		BinaryOperator NotEqualIntOperator;
		BinaryOperator EqualBoolOperator;
		BinaryOperator NotEqualBoolOperator;
		BinaryOperator LessOrEqualIntOperator;
		BinaryOperator GreatOrEqualIntOperator;

	public:
		highCompiler() {
			Types = {
				{"char",Type{.type = "char"}},
				{"void",Type{.type = "void"}},
				{"bool",Type{.type = "bool"}},
				{"int",Type{.type = "int"}}
			};

			const std::map<const Type*,const Type*,TypeComparator> TypeOpers({{&Types["int"],&Types["int"]}},TypeComparator{});
			const std::map<const Type*,const Type*,TypeComparator> TypeOpers2({{&Types["int"],&Types["bool"]}},TypeComparator{});

			const std::map<const Type*,const Type*,TypeComparator> TypeOpers3({{&Types["bool"],&Types["bool"]}},TypeComparator{});

			PlusIntOperator = BinaryOperator{.TypeOper = TypeOpers,.asmCode = {{ &Types["int"], "add rax, rbx\n"}}};
			MinusIntOperator = BinaryOperator{.TypeOper = TypeOpers,.asmCode = {{ &Types["int"], "sub rax, rbx\n"}}};
			MultiplyIntOperator = BinaryOperator{.TypeOper = TypeOpers,.asmCode = {{ &Types["int"], "mul rbx\n"}}};
			DivisionIntOperator = BinaryOperator{.TypeOper = TypeOpers,.asmCode = {{ &Types["int"], "mov rdx, 0\ndiv rbx\n"}}};
			RemainderIntOperator = BinaryOperator{.TypeOper = TypeOpers,.asmCode = {{ &Types["int"], "mov rdx, 0\ndiv rbx\nmov rax, rdx\n"}}};

			LessIntOperator = BinaryOperator{.TypeOper = TypeOpers2,.asmCode = {{ &Types["int"], "cmp rax, rbx\nsetl bl\nmovzx rax, bl\n"}}};
			GreatIntOperator = BinaryOperator{.TypeOper = TypeOpers2,.asmCode = {{ &Types["int"], "cmp rax, rbx\nsetg bl\nmovzx rax, bl\n"}}};
			EqualIntOperator = BinaryOperator{.TypeOper = TypeOpers2,.asmCode = {{ &Types["int"], "cmp rax, rbx\nsete bl\nmovzx rax, bl\n"}}};
			NotEqualIntOperator = BinaryOperator{.TypeOper = TypeOpers2,.asmCode = {{ &Types["int"], "cmp rax, rbx\nsetne bl\nmovzx rax, bl\n"}}};
			LessOrEqualIntOperator = BinaryOperator{.TypeOper = TypeOpers2,.asmCode = {{ &Types["int"], "cmp rax, rbx\nsetle bl\nmovzx rax, bl\n"}}};
			GreatOrEqualIntOperator = BinaryOperator{.TypeOper = TypeOpers2,.asmCode = {{ &Types["int"], "cmp rax, rbx\nsetge bl\nmovzx rax, bl\n"}}};

			EqualIntOperator = BinaryOperator{.TypeOper = TypeOpers2,.asmCode = {{ &Types["bool"], "cmp rax, rbx\nsete bl\nmovzx rax, bl\n"}}};
			NotEqualIntOperator = BinaryOperator{.TypeOper = TypeOpers2,.asmCode = {{ &Types["bool"], "cmp rax, rbx\nsetne bl\nmovzx rax, bl\n"}}};

			const std::map<std::string,BinaryOperator*> intOperators = {
				{"+",&PlusIntOperator},
				{"-",&MinusIntOperator},
				{"*",&MultiplyIntOperator},
				{"/",&DivisionIntOperator},
				{"%",&RemainderIntOperator},

				{"<",&LessIntOperator},
				{">",&GreatIntOperator},
				{"==",&EqualIntOperator},
				{"!=",&NotEqualIntOperator},
				{"<=",&LessOrEqualIntOperator},
				{">=",&GreatOrEqualIntOperator}
			};
			const std::map<std::string,BinaryOperator*> boolOperators = {
				{"==",&EqualBoolOperator},
				{"!=",&NotEqualBoolOperator},
			};

			Types["int"].operators = intOperators;
			Types["bool"].operators = boolOperators;

			//TODO ubrat kostil
			Function Main{.name = "main"};
			std::map<std::vector<const Type*>,const Type*,TypeComparator> TypeFunc;
			std::vector<const Type*> inputMain = {};
			TypeFunc.emplace(inputMain,&Types.at("int"));
			Functions.emplace("main",Main);
		}

		Compiler newCompiler(const std::string& nameFunc) {
			Compiler compiler = Compiler(Types,Functions,Variables,Functions.at(nameFunc)[{}],countIf);
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
