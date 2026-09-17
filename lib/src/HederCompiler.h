#pragma once

#include <string>
#include <utility>

#include "token.h"
#include "Type.h"

namespace qlang {
	class HederCompiler {
	private:
		std::map<std::string,Type> Types;

		std::pair<std::string, Context> AssemblyProcessing(const std::string &Directory, token::CODE &code);
		Context ContextProcessing(token::CODE &code);
		std::pair<std::vector<std::string>, Context> QlangProcessing(const std::string &Directory, token::CODE &code);
	public:
		std::pair<std::vector<std::string>, Context> Compiling(const std::string &Directory, token::CODE &code, const std::string& target);
		std::pair<std::vector<std::string>, Context> CompilingTarget(const std::string &Directory, token::CODE &code);

		explicit HederCompiler(std::map<std::string,Type> Types) : Types(std::move(Types)) {}
	};
}