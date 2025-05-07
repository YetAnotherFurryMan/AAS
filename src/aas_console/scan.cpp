#include <aas_console.hpp>

int aas::console::scan(aas::Program& prog, std::size_t& pc){
	std::string text;
	std::cin >> text;
	prog.stack.emplace(new aas::Text(text));

	return 0;
}

int aas::console::scanl(aas::Program& prog, std::size_t& pc){
	std::string text;
	std::getline(std::cin, text);
	prog.stack.emplace(new aas::Text(text));

	return 0;
}

int aas::console::scani(aas::Program& prog, std::size_t& pc){
	int64_t i;
	std::cin >> i;
	prog.stack.emplace(new aas::Integer(i));

	return 0;
}
