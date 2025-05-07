#include <aas.hpp>

static std::unique_ptr<aas::Data> s_add(aas::Data* a, aas::Data* b){
	if(a->type == aas::DataType::INTEGER){
		aas::Integer* i = dynamic_cast<aas::Integer*>(a);

		switch(b->type){
			case aas::DataType::INTEGER:
			{
				return std::make_unique<aas::Integer>(i->value + dynamic_cast<aas::Integer*>(b)->value);
			} break;
			case aas::DataType::TEXT:
			{
				return std::make_unique<aas::Text>(std::to_string(i->value) + dynamic_cast<aas::Text*>(b)->value);
			} break;
			default:
				return std::make_unique<aas::Data>(aas::DataType::ERROR);
		}
	} else if(a->type == aas::DataType::TEXT){
		aas::Text* t = dynamic_cast<aas::Text*>(a);

		switch(b->type){
			case aas::DataType::INTEGER:
			{
				return std::make_unique<aas::Text>(t->value + std::to_string(dynamic_cast<aas::Integer*>(b)->value));
			} break;
			case aas::DataType::TEXT:
			{
				return std::make_unique<aas::Text>(t->value + dynamic_cast<aas::Text*>(b)->value);
			} break;
			default:
				return std::make_unique<aas::Data>(aas::DataType::ERROR);
		}
	}

	return std::make_unique<aas::Data>(aas::DataType::ERROR);
}

void aas::Program::useMath(){
	on("add", [](aas::Program& prog, std::size_t& pc){
		if(prog.stack.size() <= 0){
			prog.error = "\"add\": The stack is empty: " + prog.src[pc]->strloc();
			return 1;
		}

		if(prog.stack.size() <= 1){
			prog.error = "\"add\": The stack does not have enought elements: " + prog.src[pc]->strloc();
			return 2;
		}

		std::unique_ptr<aas::Data> a = std::move(prog.stack.back());
		prog.stack.pop_back();

		std::unique_ptr<aas::Data> b = std::move(prog.stack.back());
		prog.stack.pop_back();

		if(a->type != aas::DataType::INTEGER && a->type != aas::DataType::TEXT){
			prog.error = "\"add\": Bad type of the first element of the stack: " + prog.src[pc]->strloc();
			return 3;
		}

		if(b->type != aas::DataType::INTEGER && b->type != aas::DataType::TEXT){
			prog.error = "\"add\": Bad type of the second element of the stack: " + prog.src[pc]->strloc();
			return 4;
		}

		prog.stack.emplace_back(s_add(a.get(), b.get()));

		return 0;
	});

	on("addv", [](aas::Program& prog, std::size_t& pc){
		if(prog.stack.size() <= 0){
			prog.error = "\"add\": The stack is empty: " + prog.src[pc]->strloc();
			return 1;
		}

		pc++;
		if(pc >= prog.src.size()){
			prog.error = "\"push\": Expected a value, but got nothing: " + prog.src[pc - 1]->strloc();
			return 1;
		}

		std::unique_ptr<aas::Data> a = std::move(prog.stack.back());
		prog.stack.pop_back();

		std::unique_ptr<aas::Data> b = aas::toData(prog.src[pc].get());

		if(a->type != aas::DataType::INTEGER && a->type != aas::DataType::TEXT){
			prog.error = "\"add\": Bad type of the first element of the stack: " + prog.src[pc]->strloc();
			return 3;
		}

		prog.stack.emplace_back(s_add(a.get(), b.get()));

		return 0;
	});
}
