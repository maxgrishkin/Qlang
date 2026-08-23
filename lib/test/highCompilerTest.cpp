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

TEST(highCompilerTest, Case5) {
	qlang::highCompiler hc;
	std::vector<std::string> objectFiles = hc.Compiling("./highCompilerTestSource/Case5.q");
	EXPECT_EQ(TestObjectFiles(objectFiles), 228);
}

TEST(highCompilerTest, Case6) {
	qlang::highCompiler hc;
	std::vector<std::string> objectFiles = hc.Compiling("./highCompilerTestSource/Case6.q");
	EXPECT_EQ(TestObjectFiles(objectFiles), 228);
}

TEST(highCompilerTest, Case7) {
	qlang::highCompiler hc;
	std::vector<std::string> objectFiles = hc.Compiling("./highCompilerTestSource/Case7.q");
	EXPECT_EQ(TestObjectFiles(objectFiles), 228);
}

TEST(highCompilerTest, Case8) {
	qlang::highCompiler hc;
	std::vector<std::string> objectFiles = hc.Compiling("./highCompilerTestSource/Case8.q");
	EXPECT_EQ(TestObjectFiles(objectFiles), 228);
}