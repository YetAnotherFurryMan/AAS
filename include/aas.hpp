#pragma once

#include <set>
#include <vector>
#include <string>
#include <iostream>
#include <string_view>
#include <unordered_map>

namespace aas{
	enum class TokenType{
		ERROR = 0,
		ENDOF,
		NUMBER,
		STRING,
		FSTRING,
		IDENTIFIER
	};

	struct Token{
		TokenType type;
		std::string_view filename;
		std::size_t lineno;
		std::size_t charno;
	};

	struct Number: public Token{
		int value;

		Number() = default;
		Number(std::string_view filename, std::size_t lineno, std::size_t charno, int value):
			Token{
				.type = TokenType::NUMBER,
				.filename = filename,
				.lineno = lineno,
				.charno = charno
			}, value{value}
		{}
	};

	struct String: public Token{
		std::string value;

		String() = default;
		String(std::string_view filename, std::size_t lineno, std::size_t charno, std::string_view value):
			Token{
				.type = TokenType::STRING,
				.filename = filename,
				.lineno = lineno,
				.charno = charno
			}, value{value}
		{}
	};

	struct FString: public Token{
		std::string value;

		FString() = default;
		FString(std::string_view filename, std::size_t lineno, std::size_t charno, std::string_view value):
			Token{
				.type = TokenType::FSTRING,
				.filename = filename,
				.lineno = lineno,
				.charno = charno
			}, value{value}
		{}
	};

	struct Identifier: public Token{
		std::size_t index;

		Identifier() = default;
		Identifier(std::string_view filename, std::size_t lineno, std::size_t charno, std::size_t index):
			Token{
				.type = TokenType::IDENTIFIER,
				.filename = filename,
				.lineno = lineno,
				.charno = charno
			}, index{index}
		{}
	};

	struct Program{
		std::vector<std::string> ids;
		std::unordered_map<std::string, std::size_t> id_dict;
		std::unordered_map<std::string, std::size_t> labels;
		std::vector<Token> src;
		std::string error;

		bool compile(std::string_view name, std::istream& in);
		aas::Token next(std::istream& in, std::string_view filename, std::size_t& lineno, std::size_t& charno);
	};
}
