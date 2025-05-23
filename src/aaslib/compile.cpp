#include <aas.hpp>

bool aas::Program::compile(std::string_view name, std::istream& in){
	std::size_t lineno = 1;
	std::size_t charno = 0;

	std::size_t pc = src.size();
	std::unique_ptr<aas::Token> tok = next(in, name, lineno, charno, pc++);

	while(tok->type != aas::TokenType::ERROR && tok->type != aas::TokenType::ENDOF){
		src.push_back(std::move(tok));
		tok = std::move(next(in, name, lineno, charno, pc++));
	}

	if(tok->type == aas::TokenType::ERROR){
		error = "Unparsable: " + tok->strloc();
		return true;
	}

	return false;
}
