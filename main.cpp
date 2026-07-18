#include <iostream>

#include "highCompiler.h"
#include "Lexer.h"
#include "Parser.h"
#include "lib/test/TestASM.h"

int main() {
	qlang::highCompiler hc;
	qlang::SectionsCode asmCode = hc.Compiling("./highCompilerTestSource/Case2.q");
	TestFullCode(asmCode,hc,{});
}