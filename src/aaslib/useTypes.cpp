#include <aas.hpp>

void aas::Program::useTypes(){
	on("text", [](aas::Program& prog, std::size_t& pc){
		if(prog.stack.size() <= 0){
			prog.error = "\"text\": The stack is empty: " + prog.src[pc]->strloc();
			return 1;
		}


		if(prog.stack.back()->type == aas::DataType::TEXT)
			return 0;
		
		std::unique_ptr<aas::Data> top = std::move(prog.stack.back());
		prog.stack.pop_back();

		switch(top->type){
			case aas::DataType::INTEGER:
			{
				aas::Integer* i = dynamic_cast<aas::Integer*>(top.get());
				prog.stack.emplace_back(new aas::Text(std::to_string(i->value)));
			} break;
			case aas::DataType::OBJECT:
			{
				// TODO
			} break;
			default:
				prog.stack.emplace_back(new aas::Data(aas::DataType::ERROR));
				return 1;
		}

		return 0;
	});

	on("int", [](aas::Program& prog, std::size_t& pc){
		if(prog.stack.size() <= 0){
			prog.error = "\"int\": The stack is empty: " + prog.src[pc]->strloc();
			return 1;
		}


		if(prog.stack.back()->type == aas::DataType::INTEGER)
			return 0;
		
		std::unique_ptr<aas::Data> top = std::move(prog.stack.back());
		prog.stack.pop_back();

		switch(top->type){
			case aas::DataType::TEXT:
			{
				aas::Text* t = dynamic_cast<aas::Text*>(top.get());
				prog.stack.emplace_back(new aas::Integer(atol(t->value.c_str())));
			} break;
			case aas::DataType::OBJECT:
			{
				// TODO
			} break;
			default:
				prog.stack.emplace_back(new aas::Data(aas::DataType::ERROR));
				return 1;
		}

		return 0;

	});
}
