#include "highCompiler.h"
#include "Lexer.h"
#include "Parser.h"
#include "lib/test/TestASM.h"

int main() {
	qlang::token::CODE CompilerCode = qlang::Lexer::Lex("var int aboba = 123 + 321 / 3; }");
	qlang::token::CODE ParserCode = qlang::Lexer::Lex("aboba;");

	std::string CodeSection;
	std::string DataSection;
	qlang::highCompiler hc;
	qlang::Compiler compiler = hc.newCompiler("main");
	qlang::SectionsCode sections_code = compiler.Compiling(CompilerCode);
	CodeSection += sections_code.code;
	DataSection += sections_code.data;
	qlang::Parser parser(&compiler);
	CodeSection += parser.ParseExpression(ParserCode).code;
	TestExpressionWithData(CodeSection,DataSection);
}