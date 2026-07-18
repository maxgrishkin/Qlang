#include <iostream>

#include "highCompiler.h"
#include "Lexer.h"
#include "Parser.h"
#include "lib/test/TestASM.h"

int main() {
	qlang::highCompiler hc;
	qlang::SectionsCode asmCode = hc.Compiling("./Test.q");
	std::cout << TestFullCode(asmCode,hc,{});
}