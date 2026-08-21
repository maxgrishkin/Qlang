#include <iostream>

#include "highCompiler.h"
#include "Lexer.h"
#include "lib/test/TestASM.h"

int main() {
	qlang::highCompiler hc;
	std::vector<std::string> objectFiles = hc.Compiling("Test.q");
	const int exitCode = TestObjectFiles(objectFiles);
	std::cout << "\nProcess finished with exit code -> " << exitCode << "\n";
}