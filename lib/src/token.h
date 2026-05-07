#pragma once

#include <string>
#include <variant>
#include <vector>

#include "util.h"

namespace qlang::token {
    enum class Keyword {
        FN,
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
    struct Char {
        auto operator<=>(const Char&) const = default;
        char value;
    };
    struct LPar {
        auto operator<=>(const LPar&) const = default;
    };   // (
    struct RPar {
        auto operator<=>(const RPar&) const = default;
    };   // (
    struct LCurlyBracket {
        auto operator<=>(const LCurlyBracket&) const = default;
    };   // {
    struct RCurlyBracket {
        auto operator<=>(const RCurlyBracket&) const = default;
    };   // {
    struct LSquareBracket {
        auto operator<=>(const LSquareBracket&) const = default;
    };   // [
    struct RSquareBracket {
        auto operator<=>(const RSquareBracket&) const = default;
    };   // [
    struct LAngleBracket {
        auto operator<=>(const LAngleBracket&) const = default;
    };   // <
    struct RAngleBracket {
        auto operator<=>(const RAngleBracket&) const = default;
    };   // <
    struct LAngleBracketEqual {
        auto operator<=>(const LAngleBracketEqual&) const = default;
    };   // <=
    struct RAngleBracketEqual {
        auto operator<=>(const RAngleBracketEqual&) const = default;
    };   // >=
    struct Semicolon {
        auto operator<=>(const Semicolon&) const = default;
    };   // ;
    struct Plus {
        auto operator<=>(const Plus&) const = default;
    };   // +
    struct Minus {
        auto operator<=>(const Minus&) const = default;
    };   // -
    struct Asterisk {
        auto operator<=>(const Asterisk&) const = default;
    };   // *
    struct Slash {
        auto operator<=>(const Slash&) const = default;
    };   // /
    struct Comma {
        auto operator<=>(const Comma&) const = default;
    };   // ,
    struct Dot {
        auto operator<=>(const Dot&) const = default;
    };   // .
    struct Equal {
        auto operator<=>(const Equal&) const = default;
    };   // =
    struct NoEqual {
        auto operator<=>(const NoEqual&) const = default;
    };   // !=
    struct Equal2 {
        auto operator<=>(const Equal2&) const = default;
    };   // ==
    struct PlusPlus {
        auto operator<=>(const PlusPlus&) const = default;
    };   // ++
    struct MinusMinus {
        auto operator<=>(const MinusMinus&) const = default;
    };   // --
    struct Identifier {
        auto operator<=>(const Identifier&) const = default;
        std::string value;
    };
    struct String {
        auto operator<=>(const String&) const = default;
        std::string value;
    };
    struct Integer {
        auto operator<=>(const Integer&) const = default;
        int32_t value;
    };

using Any = std::variant<
    Keyword, LPar, RPar, Equal, Equal2, LCurlyBracket, RCurlyBracket, Semicolon, Comma, Plus, Minus, Asterisk, Slash, Identifier,
    Integer, String,LSquareBracket,RSquareBracket,LAngleBracket,RAngleBracket,LAngleBracketEqual,RAngleBracketEqual,PlusPlus,MinusMinus,
    NoEqual,Char,Dot>;

    using CODE = std::vector<std::pair<Any,std::pair<int,int>>>;

    inline std::string TokenToString(const Any& token) {
        return std::visit(
            match {
                [](Keyword kw) -> std::string {
                    switch (kw) {
                        case Keyword::FN: return "fn";
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
}
