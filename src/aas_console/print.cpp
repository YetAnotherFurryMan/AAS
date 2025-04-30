#include <aas_console.hpp>

static int s_print(aas::Program& prog, aas::Token* tok){
	switch(tok->type){
		case aas::TokenType::NUMBER:
		{
			aas::Number* num = dynamic_cast<aas::Number*>(tok);
			std::cout << num->value;
		} break;
		case aas::TokenType::STRING:
		{
			aas::String* str = dynamic_cast<aas::String*>(tok);
			std::cout << str->value;
		} break;
		case aas::TokenType::FSTRING:
		{
			aas::FString* fstr = dynamic_cast<aas::FString*>(tok);
			std::cout << fstr->value;
		} break;
		case aas::TokenType::IDENTIFIER:
		{
			aas::Identifier* id = dynamic_cast<aas::Identifier*>(tok);
			std::cout << prog.ids[id->index];
		} break;
		default:
			return 1;
	}

	return 0;
}

int aas::console::print(aas::Program& prog, std::size_t& pc){
	if(prog.stack.size() <= 0){
		prog.error = "\"console.print\": Stack is empty: " + prog.src[pc]->strloc();
		return 1;
	}

	if(s_print(prog, prog.stack.top().get())){
		prog.error = "\"console.print\": Unprintable stack value: " + prog.src[pc]->strloc();
		return 2;
	}

	return 0;
}

int aas::console::printv(aas::Program& prog, std::size_t& pc){
	pc++;
	if(pc >= prog.src.size()){
		prog.error = "\"console.printv\": Expected a module name: " + prog.src[pc - 1]->strloc();
		return 1;
	}

	if(s_print(prog, prog.src[pc].get())){
		prog.error = "\"console.printv\": Unprintable value: " + prog.src[pc]->strloc();
		return 2;
	}

	return 0;
}
