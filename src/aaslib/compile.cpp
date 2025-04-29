#include <aas.hpp>

bool aas::Program::compile(std::string_view name, std::istream& in){
	std::size_t lineno = 1;
	std::size_t charno = 0;

	aas::Token tok = next(in, name, lineno, charno);

	while(tok.type != aas::TokenType::ERROR && tok.type != aas::TokenType::ENDOF){
		src.push_back(tok);
		tok = next(in, name, lineno, charno);
	}

	if(tok.type == aas::TokenType::ERROR){
		error = "Unparsable: [" + std::to_string(tok.lineno) + ":" + std::to_string(tok.charno) + "]";
		return true;
	}

	return false;
}
