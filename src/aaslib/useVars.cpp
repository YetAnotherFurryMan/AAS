#include <aas.hpp>

void aas::Program::useVars(){
	on("set", [](aas::Program& prog, std::size_t& pc){
		pc++;
		if(pc >= prog.src.size()){
			prog.error = "\"set\": Expected two values, but got nothing: " + prog.src[pc - 1]->strloc();
			return 1;
		}

		std::string name;
		switch(prog.src[pc]->type){
			case aas::TokenType::STRING:
			{
				aas::String* str = dynamic_cast<aas::String*>(prog.src[pc].get());
				name = str->value;
			} break;
			case aas::TokenType::FSTRING:
			{
				aas::FString* str = dynamic_cast<aas::FString*>(prog.src[pc].get());
				name = str->value;
			} break;
			case aas::TokenType::IDENTIFIER:
			{
				aas::Identifier* id = dynamic_cast<aas::Identifier*>(prog.src[pc].get());
				name = prog.ids[id->index];
			} break;
			default:
				prog.error = "\"set\": Expected a name (string, formated string or identifier): " + prog.src[pc]->strloc();
				return 2;
		}

		pc++;
		if(pc >= prog.src.size()){
			prog.error = "\"set\": Expected two values, but got only one: " + prog.src[pc - 2]->strloc();
			return 3;
		}

		std::unique_ptr<aas::Data> data = aas::toData(prog, prog.src[pc].get());
		if(data->type == aas::DataType::ERROR)
			return 4;

		prog.vars[name] = std::move(data);
		
		return 0;
	});

	on("get", [](aas::Program& prog, std::size_t& pc){
		pc++;
		if(pc >= prog.src.size()){
			prog.error = "\"get\": Expected a value, but got nothing: " + prog.src[pc - 1]->strloc();
			return 1;
		}

		std::string name;
		switch(prog.src[pc]->type){
			case aas::TokenType::STRING:
			{
				aas::String* str = dynamic_cast<aas::String*>(prog.src[pc].get());
				name = str->value;
			} break;
			case aas::TokenType::FSTRING:
			{
				aas::FString* str = dynamic_cast<aas::FString*>(prog.src[pc].get());
				name = str->value;
			} break;
			case aas::TokenType::IDENTIFIER:
			{
				aas::Identifier* id = dynamic_cast<aas::Identifier*>(prog.src[pc].get());
				name = prog.ids[id->index];
			} break;
			default:
				prog.error = "\"get\": Expected a name (string, formated string or identifier): " + prog.src[pc]->strloc();
				return 2;
		}

		if(prog.vars.find(name) == prog.vars.end()){
			prog.error = "\"get\": No such varable: '" + name + "': " + prog.src[pc - 1]->strloc();
			return 3;
		}

		prog.stack.emplace_back(aas::copy(prog.vars[name].get()));

		return 0;
	});
}
