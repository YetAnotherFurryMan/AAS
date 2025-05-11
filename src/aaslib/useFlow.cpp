#include <aas.hpp>

void aas::Program::useFlow(){
	on("label", [](aas::Program& prog, std::size_t& pc){
		pc++;
		if(pc >= prog.src.size()){
			prog.error = "\"label\": Expected an identifier, but got nothing: " + prog.src[pc - 1]->strloc();
			return 1;
		}

		if(prog.src[pc]->type != aas::TokenType::IDENTIFIER){
			prog.error = "\"label\": Expected an identifier: " + prog.src[pc]->strloc();
			return 2;
		}

		std::size_t index = dynamic_cast<aas::Identifier*>(prog.src[pc].get())->index;

		if(prog.labels.find(index) == prog.labels.end()){
			prog.error = "\"label\": No such label: " + prog.src[pc]->strloc();
			return 3;
		}

		prog.stack.emplace_back(std::make_unique<aas::Integer>(prog.labels[index]));
	
		return 0;
	});

	on("jump", [](aas::Program& prog, std::size_t& pc){
		pc++;
		if(pc >= prog.src.size()){
			prog.error = "\"jump\": Expected a value, but got nothing: " + prog.src[pc - 1]->strloc();
			return 1;
		}

		std::unique_ptr<aas::Data> data = aas::toData(prog, prog.src[pc].get());

		if(data->type == aas::DataType::ERROR){
			return 2;
		} else if(data->type != aas::DataType::INTEGER){
			prog.error = "\"jump\": Expected an integer: " + prog.src[pc]->strloc();
			return 3;
		}

		pc = dynamic_cast<aas::Integer*>(data.get())->value;

		return 0;
	});

	on("jumpz", [](aas::Program& prog, std::size_t& pc){
		pc++;
		if(pc >= prog.src.size()){
			prog.error = "\"jumpz\": Expected a value, but got nothing: " + prog.src[pc - 1]->strloc();
			return 1;
		}

		std::unique_ptr<aas::Data> data = aas::toData(prog, prog.src[pc].get());

		if(data->type == aas::DataType::ERROR){
			return 2;
		} else if(data->type != aas::DataType::INTEGER){
			prog.error = "\"jumpz\": Expected an integer: " + prog.src[pc]->strloc();
			return 3;
		}

		if(prog.stack.size() <= 0){
			prog.error = "\"jumpz\": The stack is empty: " + prog.src[pc]->strloc();
			return 4;
		}

		std::unique_ptr<aas::Data> top = std::move(prog.stack.back());
		prog.stack.pop_back();

		if(top->type == aas::DataType::INTEGER){
			if(dynamic_cast<aas::Integer*>(top.get())->value != 0)
				return 0;
		}

		pc = dynamic_cast<aas::Integer*>(data.get())->value;

		return 0;
	});

	on("goto", [](aas::Program& prog, std::size_t& pc){
		pc++;
		if(pc >= prog.src.size()){
			prog.error = "\"goto\": Expected an identifier, but got nothing: " + prog.src[pc - 1]->strloc();
			return 1;
		}

		if(prog.src[pc]->type != aas::TokenType::IDENTIFIER){
			prog.error = "\"goto\": Expected an identifier: " + prog.src[pc]->strloc();
			return 2;
		}

		std::size_t index = dynamic_cast<aas::Identifier*>(prog.src[pc].get())->index;

		if(prog.labels.find(index) == prog.labels.end()){
			prog.error = "\"goto\": No such label: " + prog.src[pc]->strloc();
			return 3;
		}

		pc = prog.labels[index];

		return 0;
	});

	on("gotoz", [](aas::Program& prog, std::size_t& pc){
		pc++;
		if(pc >= prog.src.size()){
			prog.error = "\"gotoz\": Expected an identifier, but got nothing: " + prog.src[pc - 1]->strloc();
			return 1;
		}

		if(prog.src[pc]->type != aas::TokenType::IDENTIFIER){
			prog.error = "\"gotoz\": Expected an identifier: " + prog.src[pc]->strloc();
			return 2;
		}

		std::size_t index = dynamic_cast<aas::Identifier*>(prog.src[pc].get())->index;

		if(prog.labels.find(index) == prog.labels.end()){
			prog.error = "\"gotoz\": No such label: " + prog.src[pc]->strloc();
			return 3;
		}

		if(prog.stack.size() <= 0){
			prog.error = "\"gotoz\": The stack is empty: " + prog.src[pc]->strloc();
			return 4;
		}

		std::unique_ptr<aas::Data> top = std::move(prog.stack.back());
		prog.stack.pop_back();

		if(top->type == aas::DataType::INTEGER){
			if(dynamic_cast<aas::Integer*>(top.get())->value != 0)
				return 0;
		}

		pc = prog.labels[index];

		return 0;
	});

	on("call", [](aas::Program& prog, std::size_t& pc){
		pc++;
		if(pc >= prog.src.size()){
			prog.error = "\"call\": Expected an identifier, but got nothing: " + prog.src[pc - 1]->strloc();
			return 1;
		}

		if(prog.src[pc]->type != aas::TokenType::IDENTIFIER){
			prog.error = "\"call\": Expected an identifier: " + prog.src[pc]->strloc();
			return 2;
		}

		std::size_t index = dynamic_cast<aas::Identifier*>(prog.src[pc].get())->index;

		if(prog.labels.find(index) == prog.labels.end()){
			prog.error = "\"call\": No such label: " + prog.src[pc]->strloc();
			return 3;
		}

		prog.stack.emplace_back(std::make_unique<aas::Integer>(pc));

		pc = prog.labels[index];

		return 0;
	});

	on("callz", [](aas::Program& prog, std::size_t& pc){
		pc++;
		if(pc >= prog.src.size()){
			prog.error = "\"callz\": Expected an identifier, but got nothing: " + prog.src[pc - 1]->strloc();
			return 1;
		}

		if(prog.src[pc]->type != aas::TokenType::IDENTIFIER){
			prog.error = "\"callz\": Expected an identifier: " + prog.src[pc]->strloc();
			return 2;
		}

		std::size_t index = dynamic_cast<aas::Identifier*>(prog.src[pc].get())->index;

		if(prog.labels.find(index) == prog.labels.end()){
			prog.error = "\"callz\": No such label: " + prog.src[pc]->strloc();
			return 3;
		}

		if(prog.stack.size() <= 0){
			prog.error = "\"callz\": The stack is empty: " + prog.src[pc]->strloc();
			return 4;
		}

		std::unique_ptr<aas::Data> top = std::move(prog.stack.back());
		prog.stack.pop_back();

		if(top->type == aas::DataType::INTEGER){
			if(dynamic_cast<aas::Integer*>(top.get())->value != 0)
				return 0;
		}

		prog.stack.emplace_back(std::make_unique<aas::Integer>(pc));

		pc = prog.labels[index];

		return 0;
	});

	on("ret", [](aas::Program& prog, std::size_t& pc){
		if(prog.stack.size() <= 0){
			prog.error = "\"ret\": The stack is empty: " + prog.src[pc]->strloc();
			return 1;
		}

		std::unique_ptr<aas::Data> data = std::move(prog.stack.back());
		prog.stack.pop_back();

		if(data->type != aas::DataType::INTEGER){
			prog.error = "\"ret\": Expected an integer on the stack: " + prog.src[pc]->strloc();
			return 2;
		}

		pc = dynamic_cast<aas::Integer*>(data.get())->value;

		return 0;
	});
}
