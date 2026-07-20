#include <iostream>

#include "highCompiler.h"
#include "Lexer.h"
#include "Parser.h"
#include "lib/test/TestASM.h"

int main() {
	qlang::highCompiler hc;
	const std::vector<std::string> objectFiles = hc.Compiling("./Test/Test.q", true);
	int exitCode = TestObjectFiles(objectFiles);
	std::cout << "\nProcess finished with exit code -> " << exitCode << "\n";
}