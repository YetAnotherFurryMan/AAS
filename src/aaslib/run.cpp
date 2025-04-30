#include <aas.hpp>

int aas::Program::run(){
	int state = 0;

	for(std::size_t pc = 0; pc < src.size() && state == 0; pc++){
		aas::Identifier* id = dynamic_cast<aas::Identifier*>(src[pc].get());

		if(!id){
			state = -1;
			error = "Cannot execute data: " + src[pc]->strloc() + " - " + std::to_string(pc);
			break;
		}

		std::size_t index = id->index;
		if(!commands[index])
			index = id_dict[ids[index]];

		if(!commands[index]){
			state = -2;
			error = "Identifier (" + std::to_string(id->index) + ":\"" + ids[id->index] + "\") has no asigned command: " + id->strloc() + " - " + std::to_string(pc);
			break;
		}

		state = commands[index](*this, pc);
	}

	return state;
}
