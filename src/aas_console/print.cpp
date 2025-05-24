#include <aas_console.hpp>

static int s_print(aas::Data* data){
	switch(data->type){
		case aas::DataType::INTEGER:
		{
			aas::Integer* i = dynamic_cast<aas::Integer*>(data);
			std::cout << i->value;
		} break;
		case aas::DataType::TEXT:
		{
			aas::Text* t = dynamic_cast<aas::Text*>(data);
			std::cout << t->value;
		} break;
		case aas::DataType::OBJECT:
		{
			aas::Object* o = dynamic_cast<aas::Object*>(data);
			std::cout << "[" << o->name << ": " << o->object << "]";
		} break;
		case aas::DataType::REFERENCE:
		{
			aas::Reference* r = dynamic_cast<aas::Reference*>(data);
			return s_print(r->ref.get());
		}
		default:
			return 1;
	}

	return 0;
}

inline int s_print(aas::Program& prog, aas::Token* tok){
	if(tok->type == aas::TokenType::IDENTIFIER){
		aas::Identifier* id = dynamic_cast<aas::Identifier*>(tok);
		std::cout << "[" << id->index << ": " << prog.ids[id->index] << "]";
		return 0;
	}

	std::unique_ptr<aas::Data> p = aas::toData(prog, tok);
	return s_print(p.get());
}

int aas::console::print(aas::Program& prog, std::size_t& pc){
	if(prog.stack.size() <= 0){
		prog.error = "\"console.print\": Stack is empty: " + prog.src[pc]->strloc();
		return 1;
	}

	if(s_print(prog.stack.back().get())){
		prog.error = "\"console.print\": Unprintable stack value: " + prog.src[pc]->strloc();
		return 2;
	}

	return 0;
}

int aas::console::printv(aas::Program& prog, std::size_t& pc){
	pc++;
	if(pc >= prog.src.size()){
		prog.error = "\"console.printv\": Expected a value, but got nothing: " + prog.src[pc - 1]->strloc();
		return 1;
	}

	if(s_print(prog, prog.src[pc].get())){
		prog.error = "\"console.printv\": Unprintable value: " + prog.src[pc]->strloc();
		return 2;
	}

	return 0;
}
