#include <aas_console.hpp>

void aas::console::use(aas::Program& prog, bool aliases){
	prog.on("console.print", aas::console::print);
	prog.on("console.printv", aas::console::printv);

	prog.on("console.scan", aas::console::scan);
	prog.on("console.scanl", aas::console::scanl);
	prog.on("console.scani", aas::console::scani);

	if(aliases){
		prog.alias("console.print", "print");
		prog.alias("console.printv", "printv");

		prog.alias("console.scan", "scan");
		prog.alias("console.scanl", "scanl");
		prog.alias("console.scani", "scani");
	}
}
