#include <gtest/gtest.h>
#include "../src/Lexer.h"

TEST(LexerTest,Case1) {
	qlang::token::CODE expected = {
		std::make_pair(qlang::token::Keyword::FN,std::make_pair(1,1)),
		std::make_pair(qlang::token::Keyword::VAR,std::make_pair(1,1)),
		std::make_pair(qlang::token::Keyword::RETURN,std::make_pair(1,1)),
		std::make_pair(qlang::token::Keyword::IF,std::make_pair(1,1)),
		std::make_pair(qlang::token::Keyword::FOR,std::make_pair(1,1)),
		std::make_pair(qlang::token::Keyword::WHILE,std::make_pair(1,1)),
		std::make_pair(qlang::token::Keyword::FOREACH,std::make_pair(1,1)),
		std::make_pair(qlang::token::Keyword::IN,std::make_pair(1,1)),
		std::make_pair(qlang::token::Keyword::BREAK,std::make_pair(1,1)),
		std::make_pair(qlang::token::Keyword::CONTINUE,std::make_pair(1,1)),
		std::make_pair(qlang::token::Keyword::ELSE,std::make_pair(1,1)),
		std::make_pair(qlang::token::LPar{},std::make_pair(1,1)),
		std::make_pair(qlang::token::RPar{},std::make_pair(1,1)),
		std::make_pair(qlang::token::LCurlyBracket{},std::make_pair(1,1)),
		std::make_pair(qlang::token::RCurlyBracket{},std::make_pair(1,1)),
		std::make_pair(qlang::token::LSquareBracket{},std::make_pair(1,1)),
		std::make_pair(qlang::token::RSquareBracket{},std::make_pair(1,1)),
		std::make_pair(qlang::token::LAngleBracket{},std::make_pair(1,1)),
		std::make_pair(qlang::token::RAngleBracket{},std::make_pair(1,1)),
		std::make_pair(qlang::token::LAngleBracketEqual{},std::make_pair(1,1)),
		std::make_pair(qlang::token::RAngleBracketEqual{},std::make_pair(1,1)),
		std::make_pair(qlang::token::Semicolon{},std::make_pair(1,1)),
		std::make_pair(qlang::token::Plus{},std::make_pair(1,1)),
		std::make_pair(qlang::token::Minus{},std::make_pair(1,1)),
		std::make_pair(qlang::token::Asterisk{},std::make_pair(1,1)),
		std::make_pair(qlang::token::Slash{},std::make_pair(1,1)),
		std::make_pair(qlang::token::Percent{},std::make_pair(1,1)),
		std::make_pair(qlang::token::Comma{},std::make_pair(1,1)),
		std::make_pair(qlang::token::Dot{},std::make_pair(1,1)),
		std::make_pair(qlang::token::Equal{},std::make_pair(1,1)),
		std::make_pair(qlang::token::NoEqual{},std::make_pair(1,1)),
		std::make_pair(qlang::token::Equal2{},std::make_pair(1,1)),
		std::make_pair(qlang::token::PlusPlus{},std::make_pair(1,1)),
		std::make_pair(qlang::token::MinusMinus{},std::make_pair(1,1)),
		std::make_pair(qlang::token::Identifier{.value = "aboba"},std::make_pair(1,1)),
		std::make_pair(qlang::token::Integer{.value = 123},std::make_pair(1,1)),
		std::make_pair(qlang::token::String{.value = "\r\t\n\\\" fsdfsdf4343"},std::make_pair(1,1)),
		std::make_pair(qlang::token::Char{.value = '\r'},std::make_pair(1,1)),
		std::make_pair(qlang::token::Char{.value = '\t'},std::make_pair(1,1)),
		std::make_pair(qlang::token::Char{.value = '\n'},std::make_pair(1,1)),
		std::make_pair(qlang::token::Char{.value = '\\'},std::make_pair(1,1)),
		std::make_pair(qlang::token::Char{.value = '\''},std::make_pair(1,1)),
		std::make_pair(qlang::token::Char{.value = 'g'},std::make_pair(1,1))
	};
	std::string code = R"(fn var return if for while foreach in break continue else(){}[]<><=>=;+-*/%,. = != ==++--aboba 123 "\r\t\n\\\" fsdfsdf4343" '\r' '\t' '\n' '\\' '\'' 'g')";
	qlang::token::CODE code_lex = qlang::Lexer::Lex(code);
	for (int i = 0;i<expected.size();i++) {
		EXPECT_EQ(code_lex[i].first,expected[i].first);
	}
}

TEST(LexerTest,Case2) {
	qlang::token::CODE expected = {
		std::make_pair(qlang::token::Keyword::FN,std::make_pair(1,1)),
		std::make_pair(qlang::token::Keyword::FN,std::make_pair(1,1)),
	};
	std::string code = "//fdfdfdfd /*()*&%$#$%&*&^&%{}[]\nfn/*fjdjfndjnffsfdsfdsfdffsddfsdffsfssff\ndsdsddssdssdds*/fn";
	qlang::token::CODE code_lex = qlang::Lexer::Lex(code);
	for (int i = 0;i<expected.size();i++) {
		EXPECT_EQ(code_lex[i].first,expected[i].first);
	}
}