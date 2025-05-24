#include <aas_fio.hpp>

void aas::fio::use(aas::Program& prog, bool aliases){
	prog.on("fio.open.in", aas::fio::openin);
	prog.on("fio.open.inv", aas::fio::openinv);

	prog.on("fio.open.out", aas::fio::openout);
	prog.on("fio.open.outv", aas::fio::openoutv);

	prog.on("fio.open.app", aas::fio::openapp);
	prog.on("fio.open.appv", aas::fio::openappv);
	
	prog.on("fio.close", aas::fio::close);

	prog.on("fio.read", aas::fio::read);
	prog.on("fio.readl", aas::fio::readl);
	prog.on("fio.readi", aas::fio::readi);

	prog.on("fio.write", aas::fio::write);
	prog.on("fio.writev", aas::fio::writev);

	if(aliases){
		prog.alias("fio.open.in", "open.in");
		prog.alias("fio.open.inv", "open.inv");

		prog.alias("fio.open.out", "open.out");
		prog.alias("fio.open.outv", "open.outv");

		prog.alias("fio.open.app", "open.app");
		prog.alias("fio.open.appv", "open.appv");

		prog.alias("fio.close", "close");

		prog.alias("fio.read", "read");
		prog.alias("fio.readl", "readl");
		prog.alias("fio.readi", "readi");

		prog.alias("fio.write", "write");
		prog.alias("fio.writev", "writev");
	}
}
