#pragma once
#include "Compiler.h"
#include "Type.h"

namespace qlang {
	class highCompiler {
	private:
		std::map<std::string,Type> Types;
		std::map<std::string,Function> Functions;
		std::map<std::string,Variable> Variables;

		BinaryOperator Plus_IntIntToInt_Operator;
		BinaryOperator Minus_IntIntToInt_OOperator;
		BinaryOperator Multiply_IntIntToInt_OOperator;
		BinaryOperator Division_IntIntToInt_OOperator;
		BinaryOperator Remainder_IntIntToInt_OOperator;

		BinaryOperator Less_IntIntToBool_OOperator;
		BinaryOperator Great_IntIntToBool_Operator;
		BinaryOperator LessOrEqual_IntIntToBool_Operator;
		BinaryOperator GreatOrEqual_IntIntToBool_Operator;
		BinaryOperator Equal_IntIntToBool_Operator;
		BinaryOperator NotEqual_IntIntToBool_Operator;

		BinaryOperator Equal_BoolBoolToBool_Operator;
		BinaryOperator NotEqual_BoolBoolToBool_Operator;
		BinaryOperator Or_BoolBoolToBool_Operator;
		BinaryOperator And_BoolBoolToBool_Operator;

		BinaryOperator Equal_CharCharToBool_Operator;
		BinaryOperator NotEqual_CharCharToBool_Operator;

		Argument ArgAssignment(token::CODE& code, const Compiler& compiler) const;
	public:
		highCompiler() {
			Types = {
				{"char",Type{.type = "char"}},
				{"void",Type{.type = "void"}},
				{"bool",Type{.type = "bool"}},
				{"int",Type{.type = "int"}}
			};

			const std::map<const Type*,const Type*,TypeComparator> IntIntToInt_Operators({{&Types["int"],&Types["int"]}},TypeComparator{});
			const std::map<const Type*,const Type*,TypeComparator> IntIntToBool_Operators({{&Types["int"],&Types["bool"]}},TypeComparator{});

			const std::map<const Type*,const Type*,TypeComparator> BoolBoolToBool_Operators({{&Types["bool"],&Types["bool"]}},TypeComparator{});

			const std::map<const Type*,const Type*,TypeComparator> CharCharToBool_Operators({{&Types["char"],&Types["bool"]}},TypeComparator{});

			Plus_IntIntToInt_Operator = BinaryOperator{.TypeOper = IntIntToInt_Operators,.asmCode = {{ &Types["int"], "add rax, rbx\n"}}};
			Minus_IntIntToInt_OOperator = BinaryOperator{.TypeOper = IntIntToInt_Operators,.asmCode = {{ &Types["int"], "sub rax, rbx\n"}}};
			Multiply_IntIntToInt_OOperator = BinaryOperator{.TypeOper = IntIntToInt_Operators,.asmCode = {{ &Types["int"], "mul rbx\n"}}};
			Division_IntIntToInt_OOperator = BinaryOperator{.TypeOper = IntIntToInt_Operators,.asmCode = {{ &Types["int"], "mov rdx, 0\ndiv rbx\n"}}};
			Remainder_IntIntToInt_OOperator = BinaryOperator{.TypeOper = IntIntToInt_Operators,.asmCode = {{ &Types["int"], "mov rdx, 0\ndiv rbx\nmov rax, rdx\n"}}};

			Less_IntIntToBool_OOperator = BinaryOperator{.TypeOper = IntIntToBool_Operators,.asmCode = {{ &Types["int"], "cmp rax, rbx\nsetl bl\nmovzx rax, bl\n"}}};
			Great_IntIntToBool_Operator = BinaryOperator{.TypeOper = IntIntToBool_Operators,.asmCode = {{ &Types["int"], "cmp rax, rbx\nsetg bl\nmovzx rax, bl\n"}}};
			Equal_IntIntToBool_Operator = BinaryOperator{.TypeOper = IntIntToBool_Operators,.asmCode = {{ &Types["int"], "cmp rax, rbx\nsete bl\nmovzx rax, bl\n"}}};
			NotEqual_IntIntToBool_Operator = BinaryOperator{.TypeOper = IntIntToBool_Operators,.asmCode = {{ &Types["int"], "cmp rax, rbx\nsetne bl\nmovzx rax, bl\n"}}};
			LessOrEqual_IntIntToBool_Operator = BinaryOperator{.TypeOper = IntIntToBool_Operators,.asmCode = {{ &Types["int"], "cmp rax, rbx\nsetle bl\nmovzx rax, bl\n"}}};
			GreatOrEqual_IntIntToBool_Operator = BinaryOperator{.TypeOper = IntIntToBool_Operators,.asmCode = {{ &Types["int"], "cmp rax, rbx\nsetge bl\nmovzx rax, bl\n"}}};

			Equal_BoolBoolToBool_Operator = BinaryOperator{.TypeOper = BoolBoolToBool_Operators,.asmCode = {{ &Types["bool"], "cmp rax, rbx\nsete bl\nmovzx rax, bl\n"}}};
			NotEqual_BoolBoolToBool_Operator = BinaryOperator{.TypeOper = BoolBoolToBool_Operators,.asmCode = {{ &Types["bool"], "cmp rax, rbx\nsetne bl\nmovzx rax, bl\n"}}};
			Or_BoolBoolToBool_Operator = BinaryOperator{.TypeOper = BoolBoolToBool_Operators,.asmCode = {{ &Types["bool"], "or rax, rbx\n"}}};
			And_BoolBoolToBool_Operator = BinaryOperator{.TypeOper = BoolBoolToBool_Operators,.asmCode = {{ &Types["bool"], "and rax, rbx\n"}}};

			Equal_CharCharToBool_Operator = BinaryOperator{.TypeOper = CharCharToBool_Operators,.asmCode = {{ &Types["char"], "cmp rax, rbx\nsete bl\nmovzx rax, bl\n"}}};
			NotEqual_CharCharToBool_Operator = BinaryOperator{.TypeOper = CharCharToBool_Operators,.asmCode = {{ &Types["char"], "cmp rax, rbx\nsetne bl\nmovzx rax, bl\n"}}};

			const std::map<std::string,BinaryOperator*> intOperators = {
				{"+",&Plus_IntIntToInt_Operator},
				{"-",&Minus_IntIntToInt_OOperator},
				{"*",&Multiply_IntIntToInt_OOperator},
				{"/",&Division_IntIntToInt_OOperator},
				{"%",&Remainder_IntIntToInt_OOperator},

				{"<",&Less_IntIntToBool_OOperator},
				{">",&Great_IntIntToBool_Operator},
				{"==",&Equal_IntIntToBool_Operator},
				{"!=",&NotEqual_IntIntToBool_Operator},
				{"<=",&LessOrEqual_IntIntToBool_Operator},
				{">=",&GreatOrEqual_IntIntToBool_Operator}
			};
			const std::map<std::string,BinaryOperator*> boolOperators = {
				{"==",&Equal_BoolBoolToBool_Operator},
				{"!=",&NotEqual_BoolBoolToBool_Operator},
				{"||",&Or_BoolBoolToBool_Operator},
				{"&&",&And_BoolBoolToBool_Operator},
			};
			const std::map<std::string,BinaryOperator*> charOperators = {
				{"==",&Equal_CharCharToBool_Operator},
				{"!=",&NotEqual_CharCharToBool_Operator},
			};

			Types["int"].binaryOperators = intOperators;
			Types["bool"].binaryOperators = boolOperators;
			Types["char"].binaryOperators = charOperators;
		}

		[[nodiscard]] Compiler newCompiler(const std::string& nameFunc, const std::vector<const Type*>& input = {}) const {
			Compiler compiler = Compiler(Types,Functions,Variables,Functions.at(nameFunc)[input]);
			return compiler;
		}

		std::vector<std::string> Compiling(const std::string& path, bool Main = true);

		SectionsCode FunctionsAssignment(token::CODE& code);

		void MakeMain() { //function for only tests
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
		[[nodiscard]] Context getContext() const {
			return Context{.Types = Types,.Functions = Functions,.Variables = Variables};
		}
	};
}
