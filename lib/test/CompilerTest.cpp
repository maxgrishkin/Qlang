#include <gtest/gtest.h>

#include "highCompiler.h"
#include "Lexer.h"
#include "Parser.h"
#include "TestASM.h"
#include "token.h"

TEST(CompilerTest, Case1) {
	qlang::token::CODE CompilerCode = qlang::Lexer::Lex("var int aboba = 123; }");
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
	EXPECT_EQ(TestExpressionWithData(CodeSection,DataSection),123);
}

TEST(CompilerTest, Case2) {
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
	EXPECT_EQ(TestExpressionWithData(CodeSection,DataSection),230);
}