#include <aas.hpp>

#define DEFOP(NME, OP)                                                                                    \
static std::unique_ptr<aas::Data> s_##NME(aas::Data* a, aas::Data* b){                                    \
	if(a->type == aas::DataType::INTEGER){                                                                \
		aas::Integer* i = dynamic_cast<aas::Integer*>(a);                                                 \
                                                                                                          \
		switch(b->type){                                                                                  \
			case aas::DataType::INTEGER:                                                                  \
			{                                                                                             \
				return std::make_unique<aas::Integer>(i->value OP dynamic_cast<aas::Integer*>(b)->value); \
			} break;                                                                                      \
			default:                                                                                      \
				return std::make_unique<aas::Data>(aas::DataType::ERROR);                                 \
		}                                                                                                 \
	}                                                                                                     \
                                                                                                          \
	return std::make_unique<aas::Data>(aas::DataType::ERROR);                                             \
}

DEFOP(add, +)
DEFOP(sub, -)
DEFOP(mul, *)
DEFOP(div, /)
DEFOP(mod, %)

#undef DEFOP

static std::unique_ptr<aas::Data> s_cat(aas::Data* a, aas::Data* b){
	std::string text;

	switch(a->type){
		case aas::DataType::INTEGER:
		{
			text = std::to_string(dynamic_cast<aas::Integer*>(a)->value);
		} break;
		case aas::DataType::TEXT:
		{
			text = dynamic_cast<aas::Text*>(a)->value;
		} break;
		default:
			return std::make_unique<aas::Data>(aas::DataType::ERROR);
	}

	switch(b->type){
		case aas::DataType::INTEGER:
		{
			return std::make_unique<aas::Text>(text + std::to_string(dynamic_cast<aas::Integer*>(b)->value));
		} break;
		case aas::DataType::TEXT:
		{
			return std::make_unique<aas::Text>(text + dynamic_cast<aas::Text*>(b)->value);
		} break;
		default:
			return std::make_unique<aas::Data>(aas::DataType::ERROR);
	}

	return std::make_unique<aas::Data>(aas::DataType::ERROR);
}

inline bool is_of_type(aas::DataType type, aas::DataType* dts, std::size_t count){
	if(count == 0)
		return true;
	return type != dts[0] && is_of_type(type, dts + 1, count - 1);
}

#define ON(OP, DT...)                                                                                         \
	on(#OP, [](aas::Program& prog, std::size_t& pc){                                                          \
		if(prog.stack.size() <= 0){                                                                           \
			prog.error = "\""#OP"\": The stack is empty: " + prog.src[pc]->strloc();                          \
			return 1;                                                                                         \
		}                                                                                                     \
                                                                                                              \
		if(prog.stack.size() <= 1){                                                                           \
			prog.error = "\""#OP"\": The stack does not have enought elements: " + prog.src[pc]->strloc();    \
			return 2;                                                                                         \
		}                                                                                                     \
                                                                                                              \
		std::unique_ptr<aas::Data> a = std::move(prog.stack.back());                                          \
		prog.stack.pop_back();                                                                                \
                                                                                                              \
		std::unique_ptr<aas::Data> b = std::move(prog.stack.back());                                          \
		prog.stack.pop_back();                                                                                \
                                                                                                              \
		aas::DataType dts[] = {DT};																			  \
		constexpr std::size_t dt_count = sizeof(dts) / sizeof(aas::DataType);								  \
		if(is_of_type(a->type, dts, dt_count)){                                                               \
			prog.error = "\""#OP"\": Bad type of the first element of the stack: " + prog.src[pc]->strloc();  \
			return 3;                                                                                         \
		}                                                                                                     \
                                                                                                              \
		if(is_of_type(b->type, dts, dt_count)){                                                               \
			prog.error = "\""#OP"\": Bad type of the second element of the stack: " + prog.src[pc]->strloc(); \
			return 4;                                                                                         \
		}                                                                                                     \
                                                                                                              \
		prog.stack.emplace_back(s_##OP(b.get(), a.get()));                                                    \
                                                                                                              \
		return 0;                                                                                             \
	});

#define ONV(OP, DT...)                                                                                           \
	on(#OP"v", [](aas::Program& prog, std::size_t& pc){                                                          \
		if(prog.stack.size() <= 0){                                                                              \
			prog.error = "\""#OP"\": The stack is empty: " + prog.src[pc]->strloc();                             \
			return 1;                                                                                            \
		}                                                                                                        \
                                                                                                                 \
		pc++;                                                                                                    \
		if(pc >= prog.src.size()){                                                                               \
			prog.error = "\""#OP"\": Expected a value, but got nothing: " + prog.src[pc - 1]->strloc();          \
			return 2;                                                                                            \
		}                                                                                                        \
                                                                                                                 \
		std::unique_ptr<aas::Data> a = std::move(prog.stack.back());                                             \
		prog.stack.pop_back();                                                                                   \
                                                                                                                 \
		std::unique_ptr<aas::Data> b = aas::toData(prog, prog.src[pc].get());                                    \
		if(b->type == aas::DataType::ERROR)                                                                      \
			return 3;                                                                                            \
                                                                                                                 \
		aas::DataType dts[] = {DT};																			     \
		constexpr std::size_t dt_count = sizeof(dts) / sizeof(aas::DataType);                                    \
		if(is_of_type(a->type, dts, dt_count)){                                                                  \
			prog.error = "\""#OP"\": Bad type of the first element of the stack: " + prog.src[pc - 1]->strloc(); \
			return 4;                                                                                            \
		}                                                                                                        \
                                                                                                                 \
		if(is_of_type(b->type, dts, dt_count)){                                                                  \
			prog.error = "\""#OP"\": Bad type of the argument: " + prog.src[pc]->strloc();                       \
			return 5;                                                                                            \
		}                                                                                                        \
                                                                                                                 \
		prog.stack.emplace_back(s_##OP(a.get(), b.get()));                                                       \
                                                                                                                 \
		return 0;                                                                                                \
	});

void aas::Program::useMath(){
	ON(add, aas::DataType::INTEGER);
	ONV(add, aas::DataType::INTEGER);

	ON(sub, aas::DataType::INTEGER);
	ONV(sub, aas::DataType::INTEGER);

	ON(mul, aas::DataType::INTEGER);
	ONV(mul, aas::DataType::INTEGER);

	ON(div, aas::DataType::INTEGER);
	ONV(div, aas::DataType::INTEGER);

	ON(mod, aas::DataType::INTEGER);
	ONV(mod, aas::DataType::INTEGER);

	ON(cat, aas::DataType::INTEGER, aas::DataType::TEXT);
	ONV(cat, aas::DataType::INTEGER, aas::DataType::TEXT);
}

#undef ON
#undef ONV
