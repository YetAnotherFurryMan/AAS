#include <fstream>
#include <sstream>

#include <aas.hpp>

#include <dlfcn.h>

typedef void(use_t)(aas::Program&, bool);

struct DLL{
	void* hnd;
	std::string name;
	std::string error;

	DLL() = default;
	DLL(std::string_view name):
		name{name}
	{
		std::stringstream ss;
		std::string n(name);
		dlerror();
		hnd = dlopen(n.c_str(), RTLD_NOW | RTLD_LAZY);
		ss << "NOTE: While loading: " << dlerror() << std::endl;
		n += ".so";
		if(!hnd)
			hnd = dlopen(n.c_str(), RTLD_NOW | RTLD_LAZY);
		ss << "NOTE: While loading: " << dlerror() << std::endl;
		n = "aas_" + n;
		if(!hnd)
			hnd = dlopen(n.c_str(), RTLD_NOW | RTLD_LAZY);
		ss << "NOTE: While loading: " << dlerror() << std::endl;
		if(!hnd)
			error = "DLL: Failed to load: " + this->name + "\n" + ss.str();
	}

	~DLL(){
		if(hnd){
			dlclose(hnd);
			hnd = nullptr;
		}
	}

	inline int use(aas::Program& prog, bool aliases){
		use_t* fn = (use_t*) dlsym(hnd, ("_ZN3aas" + std::to_string(name.length()) + name + "3useERNS_7ProgramEb").c_str());
		if(!fn)
			return 1;
		fn(prog, aliases);
		return 0;
	}
};

void usage(const char* name){
	std::cerr << "USAGE: " << name << " [INPUT]" << std::endl;
}

std::function<int(aas::Program&, std::size_t&)> createUseOp(bool aliases){
	static std::unordered_map<std::string, std::unique_ptr<DLL>> dlls;

	return [aliases](aas::Program& prog, std::size_t& pc){
		pc++;
		if(pc >= prog.src.size()){
			prog.error = "\"use\": Expected a module name: " + prog.src[pc - 1]->strloc();
			return 1;
		}

		std::string name;

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

		if(dlls.find(name) != dlls.end()){
			// The module is used
			return 0;
		}

		DLL* dll = new DLL(name);

		if(!dll->hnd){
			prog.error = "\"use\": Module '" + std::string(name) + "' not found: " + prog.src[pc]->strloc() + "\n" + dll->error;
			delete dll;
			return 3;
		}

		if(dll->use(prog, aliases)){
			prog.error = "\"use\": Bad module '" + std::string(name) + "': " + prog.src[pc]->strloc();
			delete dll;
			return 4;
		}

		dlls[name] = std::unique_ptr<DLL>(dll);

		return 0;
	};
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
	prog.useVars();
	prog.useTypes();
	prog.useMath();
	prog.useFlow();
	prog.on("use", createUseOp(true));
	prog.on("import", createUseOp(false));

	int state = prog.run();
	if(state){
		std::cerr << "ERROR: Failed to execute..." << std::endl;
		std::cerr << "NOTE: " << prog.error << std::endl;
	}

	if(prog.stack.size() != 0){
		std::cerr << "ERROR: Stack not empty:" << std::endl;
		for(std::size_t i = prog.stack.size(); i > 0; i--){
			std::cerr << i << ": ";
			aas::Data* data =  prog.stack[i - 1].get();
			if(data->type == aas::DataType::REFERENCE){
				std::cerr << "&";
				data = dynamic_cast<aas::Reference*>(prog.stack[i - 1].get())->ref.get();
			}
			switch(data->type){
				case aas::DataType::ERROR:
					std::cerr << "(ERROR)";
					break;
				case aas::DataType::INTEGER:
					std::cerr << "(INTEGER) " << dynamic_cast<aas::Integer*>(data)->value;
					break;
				case aas::DataType::TEXT:
					std::cerr << "(TEXT) " << dynamic_cast<aas::Text*>(data)->value;
					break;
				case aas::DataType::OBJECT:
					std::cerr << "(OBJECT) " << dynamic_cast<aas::Object*>(data)->name << ": " << dynamic_cast<aas::Object*>(data)->object;
					break;
				case aas::DataType::REFERENCE:
					std::cerr << "(!!!)";
					break;
			}
			std::cerr << std::endl;
		}
	}

	return state;
}
