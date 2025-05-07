#pragma once

#include "aas.hpp"

namespace aas::console{
	void use(Program& prog);

	int print(Program& prog, std::size_t& pc);
	int printv(Program& prog, std::size_t& pc);

	int scan(Program& prog, std::size_t& pc);
	int scanl(Program& prog, std::size_t& pc);
	int scani(Program& prog, std::size_t& pc);
}
