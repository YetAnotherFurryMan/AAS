#include <aas.hpp>

void aas::Program::useStack(){
	on("push", [](aas::Program& prog, std::size_t& pc){
		pc++;
		if(pc >= prog.src.size()){
			prog.error = "\"push\": Expected a value, but got nothing: " + prog.src[pc - 1]->strloc();
			return 1;
		}
		
		prog.stack.emplace(prog.src[pc]->copy());
		return 0;
	});

	on("pop", [](aas::Program& prog, std::size_t& pc){
		if(prog.stack.size() <= 0){
			prog.error = "\"pop\": The stack is empty: " + prog.src[pc]->strloc();
			return 1;
		}

		prog.stack.pop();
		return 0;
	});
}
