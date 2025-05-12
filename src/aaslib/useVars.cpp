#include <aas.hpp>

void aas::Program::useVars(){
	on("set", [](aas::Program& prog, std::size_t& pc){
		pc++;
		if(pc >= prog.src.size()){
			prog.error = "\"set\": Expected two values, but got nothing: " + prog.src[pc - 1]->strloc();
			return 1;
		}

		std::string name;
		if(prog.src[pc]->type == aas::TokenType::IDENTIFIER){
			aas::Identifier* id = dynamic_cast<aas::Identifier*>(prog.src[pc].get());
			name = prog.ids[id->index];
		} else {
			std::unique_ptr<aas::Data> data = aas::toData(prog, prog.src[pc].get());
			if(data->type == aas::DataType::ERROR){
				return 2;
			} 

			if(data->type != aas::DataType::TEXT){
				prog.error = "\"set\": Expected a name (identifier or text): " + prog.src[pc]->strloc();
				return 3;
			}

			name = dynamic_cast<aas::Text*>(data.get())->value;
		}

		pc++;
		if(pc >= prog.src.size()){
			prog.error = "\"set\": Expected two values, but got only one: " + prog.src[pc - 2]->strloc();
			return 4;
		}

		std::unique_ptr<aas::Data> data = aas::toData(prog, prog.src[pc].get());
		if(data->type == aas::DataType::ERROR)
			return 5;

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
		if(prog.src[pc]->type == aas::TokenType::IDENTIFIER){
			aas::Identifier* id = dynamic_cast<aas::Identifier*>(prog.src[pc].get());
			name = prog.ids[id->index];
		} else {
			std::unique_ptr<aas::Data> data = aas::toData(prog, prog.src[pc].get());
			if(data->type == aas::DataType::ERROR){
				return 2;
			} 

			if(data->type != aas::DataType::TEXT){
				prog.error = "\"get\": Expected a name (identifier or text): " + prog.src[pc]->strloc();
				return 3;
			}

			name = dynamic_cast<aas::Text*>(data.get())->value;
		}

		if(prog.vars.find(name) == prog.vars.end()){
			prog.error = "\"get\": No such varable: '" + name + "': " + prog.src[pc - 1]->strloc();
			return 3;
		}

		prog.stack.emplace_back(aas::copy(prog.vars[name].get()));

		return 0;
	});
}
