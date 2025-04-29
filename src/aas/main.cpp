#include <fstream>

#include <aas.hpp>

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

	std::cout << "GOOD" << std::endl;
	return 0;
}
