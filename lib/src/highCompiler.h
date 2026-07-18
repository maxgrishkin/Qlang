#pragma once
#include "Compiler.h"
#include "Type.h"

namespace qlang {
	class highCompiler {
	private:
		std::map<std::string,Type> Types;
		std::map<std::string,Function> Functions;
		std::map<std::string,Variable> Variables;

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

		BinaryOperator EqualCharOperator;
		BinaryOperator NotEqualCharOperator;

		BinaryOperator LessOrEqualIntOperator;
		BinaryOperator GreatOrEqualIntOperator;

		Argument ArgAssignment(token::CODE& code, const Compiler& compiler);
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

			const std::map<const Type*,const Type*,TypeComparator> TypeOpers4({{&Types["char"],&Types["bool"]}},TypeComparator{});

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

			EqualIntOperator = BinaryOperator{.TypeOper = TypeOpers3,.asmCode = {{ &Types["bool"], "cmp rax, rbx\nsete bl\nmovzx rax, bl\n"}}};
			NotEqualIntOperator = BinaryOperator{.TypeOper = TypeOpers3,.asmCode = {{ &Types["bool"], "cmp rax, rbx\nsetne bl\nmovzx rax, bl\n"}}};

			EqualCharOperator = BinaryOperator{.TypeOper = TypeOpers4,.asmCode = {{ &Types["char"], "cmp rax, rbx\nsete bl\nmovzx rax, bl\n"}}};
			NotEqualCharOperator = BinaryOperator{.TypeOper = TypeOpers4,.asmCode = {{ &Types["char"], "cmp rax, rbx\nsetne bl\nmovzx rax, bl\n"}}};

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
			const std::map<std::string,BinaryOperator*> charOperators = {
				{"==",&EqualCharOperator},
				{"!=",&NotEqualCharOperator},
			};

			Types["int"].operators = intOperators;
			Types["bool"].operators = boolOperators;
			Types["char"].operators = charOperators;
		}

		[[nodiscard]] Compiler newCompiler(const std::string& nameFunc, const std::vector<const Type*>& input = {}) const {
			Compiler compiler = Compiler(Types,Functions,Variables,Functions.at(nameFunc)[input]);
			return compiler;
		}

		SectionsCode Compiling(const std::string& path);
		SectionsCode FunctionsAssignment(token::CODE& code);

		void MakeMain() {
			Function Main{.name = "main"};
			std::vector<const Type*> inputMain = {};
			std::vector<std::string> inputArgMain = {};
			Main.TypeFunc.emplace(inputMain,&Types.at("int"));
			Main.args.emplace(inputMain,inputArgMain);
			Functions.emplace("main",Main);
		}

		[[nodiscard]] const std::map<std::string, Type>& getTypes() const {
			return Types;
		}
		[[nodiscard]] const std::map<std::string,Function>& getFunctions() const{
			return Functions;
		}
		[[nodiscard]] const std::map<std::string,Variable>& getVariables() const {
			return Variables;
		}
	};
}
