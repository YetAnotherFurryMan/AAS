#pragma once

#include "aas.hpp"

#include <fstream>

namespace aas::fio{
	void use(Program& prog, bool aliases = true);

	int openin(Program& prog, std::size_t& pc);
	int openinv(Program& prog, std::size_t& pc);

	int openout(Program& prog, std::size_t& pc);
	int openoutv(Program& prog, std::size_t& pc);

	int openapp(Program& prog, std::size_t& pc);
	int openappv(Program& prog, std::size_t& pc);

	int close(Program& prog, std::size_t& pc);

	int read(Program& prog, std::size_t& pc);
	int readl(Program& prog, std::size_t& pc);
	int readi(Program& prog, std::size_t& pc);

	int write(Program& proh, std::size_t& pc);
	int writev(Program& proh, std::size_t& pc);

	struct File: public ObjectProto{
		std::fstream file;
		std::ios_base::openmode mode;
		bool opened;

		File(const std::string& path, std::ios_base::openmode mode):
			file{path, mode},
			mode{mode}
		{
			opened = file.good();
		}

		~File(){
			if(opened)
				file.close();
			opened = false;
		}
	};
}
