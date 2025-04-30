#include <aas_console.hpp>

void aas::console::use(aas::Program& prog){
	prog.on("console.print", aas::console::print);
	prog.on("console.printv", aas::console::printv);

	prog.alias("console.print", "print");
	prog.alias("console.printv", "printv");
}
