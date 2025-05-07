#include <aas.hpp>

void aas::Program::useStack(){
	on("push", [](aas::Program& prog, std::size_t& pc){
		pc++;
		if(pc >= prog.src.size()){
			prog.error = "\"push\": Expected a value, but got nothing: " + prog.src[pc - 1]->strloc();
			return 1;
		}
		
		prog.stack.emplace_back(aas::toData(prog.src[pc].get()));
		return 0;
	});

	on("pop", [](aas::Program& prog, std::size_t& pc){
		if(prog.stack.size() <= 0){
			prog.error = "\"pop\": The stack is empty: " + prog.src[pc]->strloc();
			return 1;
		}

		prog.stack.pop_back();
		return 0;
	});

	on("dup", [](aas::Program& prog, std::size_t& pc){
		if(prog.stack.size() <= 0){
			prog.error = "\"dup\": The stack is empty: " + prog.src[pc]->strloc();
			return 1;
		}

		aas::Data* top = prog.stack.back().get();
		prog.stack.emplace_back(aas::copy(top));

		return 0;
	});

	on("swap", [](aas::Program& prog, std::size_t& pc){
		if(prog.stack.size() <= 0){
			prog.error = "\"swap\": The stack is empty: " + prog.src[pc]->strloc();
			return 1;
		}

		if(prog.stack.size() <= 1){
			prog.error = "\"swap\": The stack does not have enought elements: " + prog.src[pc]->strloc();
			return 2;
		}

		std::unique_ptr<aas::Data> top = std::move(prog.stack.back());
		prog.stack.pop_back();
		prog.stack.insert(prog.stack.end() - 1, std::move(top));

		return 0;
	});

	on("rot", [](aas::Program& prog, std::size_t& pc){
		if(prog.stack.size() <= 0){
			prog.error = "\"rot\": The stack is empty: " + prog.src[pc]->strloc();
			return 1;
		}

		if(prog.stack.size() <= 2){
			prog.error = "\"rot\": The stack does not have enought elements: " + prog.src[pc]->strloc();
			return 2;
		}

		std::unique_ptr<aas::Data> top = std::move(prog.stack.back());
		prog.stack.pop_back();
		prog.stack.insert(prog.stack.end() - 2, std::move(top));

		return 0;
	});
}
