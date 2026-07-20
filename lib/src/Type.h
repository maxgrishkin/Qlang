#pragma once
#include <map>
#include <string>
#include <vector>
#include <stdexcept>

#include "token.h"

namespace qlang{
	struct BinaryOperator;
	struct Variable;

	struct  Type {
		const std::string type;
		std::vector<Variable*> Fields;
		std::map<std::string,BinaryOperator*> operators;
	};

	struct TypeComparator {
		bool operator()(const Type& a, const Type& b) const {
			const unsigned int as = a.type.size();
			const unsigned int bs = b.type.size();

			if(as != bs) {
				return as > bs;
			}
			for (unsigned int i = 0;i < as;i++) {
				if (static_cast<int>(a.type[i]) != static_cast<int>(b.type[i])) {
					return static_cast<int>(a.type[i]) > static_cast<int>(b.type[i]);
				}
			}
			return false;
		}
		bool operator()(const Type* a, const Type* b) const {
			const unsigned int as = a->type.size();
			const unsigned int bs = b->type.size();

			if(as != bs) {
				return as > bs;
			}
			for (unsigned int i = 0;i < as;i++) {
				if (static_cast<int>(a->type[i]) != static_cast<int>(b->type[i])) {
					return static_cast<int>(a->type[i]) > static_cast<int>(b->type[i]);
				}
			}
			return false;
		}
		bool operator()(const std::vector<const Type*>& a, const std::vector<const Type*>& b) const {
			const unsigned int as = a.size();
			const unsigned int bs = b.size();

			if(as != bs) {
				return as > bs;
			}
			for (unsigned int i = 0;i < as;i++) {
				if (!operator()(a[i],b[i]) && !operator()(b[i],a[i])) {
					continue;
				}
				return operator()(a[i],b[i]);
			}
			return false;
		}
	};

	struct SpecificFunction {
		const std::string name;
		std::pair<std::vector<const Type*>,const Type*> TypeFunc;
		std::pair<std::vector<const Type*>,std::vector<std::string>> args;
	};
	struct Function {
		std::string name;
		std::map<std::vector<const Type*>,const Type*,TypeComparator> TypeFunc;
		std::map<std::vector<const Type*>,std::vector<std::string>> args;

		SpecificFunction operator[](const std::vector<const Type*>& input) const {
			SpecificFunction res{.name = name};

			bool exist = false;
			for (const auto& type : TypeFunc) {
				if (input.size() != type.first.size()) {
					continue;
				}
				bool matching = true;
				for (unsigned int i = 0;i < type.first.size();++i) {
					if (input[i]->type != type.first[i]->type) {
						matching = false;
						break;
					}
				}
				if (!matching) {
					continue;
				}
				exist = true;
				res.TypeFunc = type;
				break;
			}
			if (!exist) {
				throw std::runtime_error("this SpecificFunction not exist");
			}
			for (const auto& arg : args) {
				if (input.size() != arg.first.size()) {
					continue;
				}
				bool matching = true;
				for (unsigned int i = 0;i < arg.first.size();++i) {
					if (input[i]->type != arg.first[i]->type) {
						matching = false;
						break;
					}
				}
				if (!matching) {
					continue;
				}
				res.args = arg;
				break;
			}
			return res;
		}
	};

	struct  Variable {
		const std::string name;
		const Type* type;
		SpecificFunction ParentFunc;
		bool initialize = false;
	};

	struct  Argument {
		const std::string name;
		const Type* type;
	};

	struct BinaryOperator {
		std::map<const Type*,const Type*,TypeComparator> TypeOper;
		std::map<const Type*,std::string,TypeComparator> asmCode;
	};

	struct Value {
		std::string code;
		const Type* type;
	};

	struct SectionsCode {
		std::string externSection;
		std::string code;
		std::string data;
		std::string otherFunks;
	};

	struct Context {
		std::map<std::string,Type> Types;
		std::map<std::string,Function> Functions;
		std::map<std::string,Variable> Variables;
	};

	inline std::map<token::Any,int> operatorsPower = {
		{token::Equal2{},1},
		{token::NoEqual{},1},
		{token::LAngleBracket{},1},
		{token::RAngleBracket{},1},
		{token::LAngleBracketEqual{},1},
		{token::RAngleBracketEqual{},1},
		{token::Plus{},2},
		{token::Minus{},2},
		{token::Asterisk{},3},
		{token::Slash{},3},
		{token::Percent{},3},
	};
}
