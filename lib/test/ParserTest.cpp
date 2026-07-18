#include <gtest/gtest.h>

#include "highCompiler.h"
#include "Lexer.h"
#include "Parser.h"
#include "TestASM.h"

TEST(ParserTest, Case1) {
	qlang::token::CODE code = qlang::Lexer::Lex("16 * 7 + 3 + 12 / 4 - 7 % 6;");

	qlang::highCompiler hc;
	hc.MakeMain();
	qlang::Compiler compiler = hc.newCompiler("main");
	qlang::Parser parser(&compiler);
	std::string asmCode = parser.ParseExpression(code).code;
	int res = TestOnlyExpression(asmCode);
	EXPECT_EQ(res,117);
}

TEST(ParserTest, Case2) {
	qlang::token::CODE code = qlang::Lexer::Lex("(5 + 3) * 2;");

	qlang::highCompiler hc;
	hc.MakeMain();
	qlang::Compiler compiler = hc.newCompiler("main");
	qlang::Parser parser(&compiler);
	std::string asmCode = parser.ParseExpression(code).code;
	int res = TestOnlyExpression(asmCode);
	EXPECT_EQ(res,16);
}

TEST(ParserTest, Case3) {
	qlang::token::CODE code = qlang::Lexer::Lex("5 + (3 * 2);");

	qlang::highCompiler hc;
	hc.MakeMain();
	qlang::Compiler compiler = hc.newCompiler("main");
	qlang::Parser parser(&compiler);
	std::string asmCode = parser.ParseExpression(code).code;
	int res = TestOnlyExpression(asmCode);
	EXPECT_EQ(res,11);
}