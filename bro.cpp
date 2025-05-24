#include "bro.hpp"

int main(int argc, const char** argv){
	bro::Bro bro(argc, argv);
	bro.fresh();

	bro.registerCmd("cxx", ".cpp", {bro.getFlag("cxx", "c++"), "-c", "$in", "-o", "$out", "-Iinclude", "-fPIC", "-ggdb", "-Wall", "-Wextra"});

	bro.registerModule(bro::ModType::LIB, "aaslib");
	bro.use("aaslib", "cxx");

	bro.registerModule(bro::ModType::APP, "aas_console");
	bro.use("aas_console", "cxx");

	bro.registerModule(bro::ModType::APP, "aas_fio");
	bro.use("aas_fio", "cxx");

	bro.registerModule(bro::ModType::EXE, "aas");
	bro.link("aas", "-lstdc++");
	bro.link("aas", "-ldl");
	bro.link("aas", "-Wl,-rpath,./,-rpath,./build/app");
	bro.use("aas", "cxx");
	bro.dep("aas", "aaslib");

	bro.build();

	return 0;
}
