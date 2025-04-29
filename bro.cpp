#include "bro.hpp"

int main(int argc, const char** argv){
	bro::Bro bro(argc, argv);
	bro.fresh();

	bro.registerCmd("cxx", ".cpp", {bro.getFlag("cxx", "c++"), "-c", "$in", "-o", "$out", "-Iinclude", "-fPIC"});

	bro.registerModule(bro::ModType::LIB, "aaslib");
	bro.use("aaslib", "cxx");

	bro.registerModule(bro::ModType::EXE, "aas");
	bro.link("aas", "-lstdc++");
	bro.use("aas", "cxx");
	bro.dep("aas", "aaslib");

	bro.build();

	return 0;
}
