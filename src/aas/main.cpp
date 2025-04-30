#include <fstream>

#include <aas.hpp>
#include <aas_console.hpp>

void usage(const char* name){
	std::cerr << "USAGE: " << name << " [INPUT]" << std::endl;
}

int main(int argc, const char** argv){
	aas::Program prog;

	if(argc < 2){
		std::cerr << "ERROR: No input file..." << std::endl;
		usage(*argv);
		return 1;
	}

	std::ifstream file(argv[1]);
	if(!file){
		std::cerr << "ERROR: Failed to open input file..." << std::endl;
		std::cerr << "NOTE: File name: " << argv[1] << std::endl;
		return 1;
	}

	if(prog.compile(argv[1], file)){
		std::cerr << "ERROR: Failed to compile input file..." << std::endl;
		std::cerr << "NOTE: " << prog.error << std::endl;

		file.close();

		return 1;
	}

	file.close();

	prog.useStack();
	prog.on("use", [](aas::Program& prog, std::size_t& pc){
		pc++;
		if(pc >= prog.src.size()){
			prog.error = "\"use\": Expected a module name: " + prog.src[pc - 1]->strloc();
			return 1;
		}

		std::string_view name;

		{
			aas::Identifier* id = dynamic_cast<aas::Identifier*>(prog.src[pc].get());
			if(id){
				name = prog.ids[id->index];
			} else{
				aas::String* str = dynamic_cast<aas::String*>(prog.src[pc].get());
				if(str){
					name = str->value;
				} else{
					prog.error = "\"use\": Invalid module name: " + prog.src[pc]->strloc();
					return 2;
				}
			}
		}

		if(name == "console"){
			aas::console::use(prog);
		} else{
			prog.error = "\"use\": Module '" + std::string(name) + "' not found: " + prog.src[pc]->strloc();
			return 3;
		}

		return 0;
	});

	int state = prog.run();
	if(state){
		std::cerr << "ERROR: Failed to execute..." << std::endl;
		std::cerr << "NOTE: " << prog.error << std::endl;
	}

	return state;
}
