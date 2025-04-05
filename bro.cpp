#include "bro.hpp"

int main(int argc, const char** argv){
	bro::Bro bro(argc, argv);
	bro.fresh();

	// TODO: Read the fortran compiler name from bro (cli args)
	
	std::string cmd_rm[] = {"rm", "-fr", "build"};
	bro::Cmd clean(cmd_rm, 3);
	clean.sync(bro.log);
	
	std::string cmd_fc[] = {"gfortran", "--sysroot=./src/", "-J./include", "-I./include", "-c", "$in", "-o", "$out", "-ggdb"};
	bro.registerCmd("fc", ".f90", cmd_fc, 9);

	bro.registerModule(bro::ModType::LIB, "aaslib");
	bro.use("aaslib", "fc");

	bro.registerModule(bro::ModType::EXE, "aas");
	bro.use("aas", "fc");
	bro.dep("aas", "aaslib");
	bro.link("aas", "-lgfortran");

	std::filesystem::create_directory("include");
	
	bro.build();

	return 0;
}
