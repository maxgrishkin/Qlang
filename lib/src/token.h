#pragma once

#include <string>
#include <variant>
#include <vector>

#include "util.h"

namespace qlang::token {
    enum class Keyword {
        FN,
        VAR,
        RETURN,
        IF,
        FOR,
        WHILE,
        FOREACH,
        IN,
        BREAK,
        CONTINUE,
        ELSE,
    };
    struct LPar {};   // (
    struct RPar {};   // (
    struct LCurlyBracket {};   // {
    struct RCurlyBracket {};   // {
    struct LSquareBracket {};   // [
    struct RSquareBracket {};   // [
    struct LAngleBracket {};   // <
    struct RAngleBracket {};   // <
    struct LAngleBracketEqual {};   // <=
    struct RAngleBracketEqual {};   // >=
    struct Semicolon {};   // ;
    struct Plus {};   // +
    struct Minus {};   // -
    struct Asterisk {};   // *
    struct Slash {};   // /
    struct Percent {};   // /
    struct Comma {};   // ,
    struct Dot {};   // .
    struct Equal {};   // =
    struct NoEqual {};   // !=
    struct Equal2 {};   // ==
    struct PlusPlus {};   // ++
    struct MinusMinus {};   // --
    struct Char {
        char value;
    };
    struct Identifier {
        std::string value;
    };
    struct String {
        std::string value;
    };
    struct Integer {
        int32_t value;
    };

    using Any = std::variant<
    Keyword, LPar, RPar, Equal, Equal2, LCurlyBracket, RCurlyBracket, Semicolon, Comma, Plus, Minus, Asterisk, Slash, Identifier,
    Integer, String,LSquareBracket,RSquareBracket,LAngleBracket,RAngleBracket,LAngleBracketEqual,RAngleBracketEqual,PlusPlus,MinusMinus,
    NoEqual,Char,Dot,Percent>;

    using CODE = std::vector<std::pair<Any,std::pair<unsigned int,unsigned short>>>;

    inline std::string TokenToString(const Any& token) {
        return std::visit(
            match {
                [](Keyword kw) -> std::string {
                    switch (kw) {
                        case Keyword::FN: return "fn";
                        case Keyword::VAR: return "var";
                        case Keyword::RETURN: return "return";
                        case Keyword::IF: return "if";
                        case Keyword::ELSE: return "else";
                        case Keyword::FOR: return "for";
                        case Keyword::WHILE: return "while";
                        case Keyword::FOREACH: return "foreach";
                        case Keyword::IN: return "in";
                        case Keyword::BREAK: return "break";
                        case Keyword::CONTINUE: return "continue";
                    }
                    return "unknown keyword";
                },
                [](LPar)-> std::string { return "("; },
                [](RPar)-> std::string  { return ")"; },
                [](LCurlyBracket)-> std::string  { return "{"; },
                [](RCurlyBracket)-> std::string  { return "}"; },
                [](LSquareBracket)-> std::string  { return "["; },
                [](RSquareBracket)-> std::string  { return "]"; },
                [](Semicolon)-> std::string  { return ";"; },
                [](Plus)-> std::string  { return "+"; },
                [](Minus)-> std::string  { return "-"; },
                [](Asterisk)-> std::string  { return "*"; },
                [](Slash)-> std::string  { return "/"; },
                [](Percent)-> std::string  { return "%"; },
                [](Comma)-> std::string  { return ","; },
                [](Dot)-> std::string  { return "."; },
                [](Equal)-> std::string  { return "="; },
                [](Equal2)-> std::string  { return "=="; },
                [](NoEqual)-> std::string  { return "!="; },
                [](LAngleBracket)-> std::string  { return "<"; },
                [](RAngleBracket)-> std::string  { return ">"; },
                [](LAngleBracketEqual)-> std::string  { return "<="; },
                [](RAngleBracketEqual)-> std::string  { return ">="; },
                [](PlusPlus)-> std::string  { return "++"; },
                [](MinusMinus)-> std::string  { return "--"; },
                [](const Identifier& id)-> std::string  { return "identifier:" + id.value; },
                [](const String& s)-> std::string  { return "string:\"" + s.value + "\""; },
                [](const Char& c) -> std::string  { return "char:'" + std::string(1, c.value) + "'"; },
                [](const Integer& i)-> std::string  { return "integer:" + std::to_string(i.value); },
                [](auto&&) { return "unknown token"; },
            },
            token);
    }

    inline bool operator==(const Any& a,const Any& b) {
        return a.index() == b.index();
    }
    inline bool operator<(const Any& a,const Any& b) {
        return a.index() < b.index();
    }
    inline bool operator>(const Any& a,const Any& b) {
        return a.index() > b.index();
    }
}