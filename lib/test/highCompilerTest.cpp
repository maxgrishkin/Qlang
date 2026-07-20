#include <gtest/gtest.h>

#include "highCompiler.h"
#include "Lexer.h"
#include "TestASM.h"
#include "token.h"

TEST(highCompilerTest, Case1) {
	qlang::highCompiler hc;
	std::vector<std::string> objectFiles = hc.Compiling("./highCompilerTestSource/Case1.q");
	EXPECT_EQ(TestObjectFiles(objectFiles), 228);
}

TEST(highCompilerTest, Case2) {
	qlang::highCompiler hc;
	std::vector<std::string> objectFiles = hc.Compiling("./highCompilerTestSource/Case2.q");
	EXPECT_EQ(TestObjectFiles(objectFiles), 228);
}

TEST(highCompilerTest, Case3) {
	qlang::highCompiler hc;
	std::vector<std::string> objectFiles = hc.Compiling("./highCompilerTestSource/Case3.q");
	EXPECT_EQ(TestObjectFiles(objectFiles), 228);
}

TEST(highCompilerTest, Case4) {
	qlang::highCompiler hc;
	std::vector<std::string> objectFiles = hc.Compiling("./highCompilerTestSource/Case4.q");
	EXPECT_EQ(TestObjectFiles(objectFiles), 228);
}