#include <aas_console.hpp>

void aas::console::use(aas::Program& prog){
	prog.on("console.print", aas::console::print);

	prog.alias("console.print", "print");
}
