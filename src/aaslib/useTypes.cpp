#include <aas.hpp>

void aas::Program::useTypes(){
	on("text", [](aas::Program& prog, std::size_t& pc){
		if(prog.stack.size() <= 0){
			prog.error = "\"text\": The stack is empty: " + prog.src[pc]->strloc();
			return 1;
		}

		if(prog.stack.back()->type == aas::DataType::TEXT)
			return 0;
		
		std::unique_ptr<aas::Data> _top = std::move(prog.stack.back());
		prog.stack.pop_back();

		aas::Data* top = _top.get();
		if(top->type == aas::DataType::REFERENCE){
			top = dynamic_cast<aas::Reference*>(top)->ref.get();
		}

		switch(top->type){
			case aas::DataType::INTEGER:
			{
				aas::Integer* i = dynamic_cast<aas::Integer*>(top);
				prog.stack.emplace_back(new aas::Text(std::to_string(i->value)));
			} break;
			case aas::DataType::OBJECT:
			{
				// TODO
			} break;
			default:
				prog.stack.emplace_back(new aas::Data(aas::DataType::ERROR));
				return 2;
		}

		return 0;
	});

	on("int", [](aas::Program& prog, std::size_t& pc){
		if(prog.stack.size() <= 0){
			prog.error = "\"int\": The stack is empty: " + prog.src[pc]->strloc();
			return 1;
		}

		if(prog.stack.back()->type == aas::DataType::INTEGER)
			return 0;
		
		std::unique_ptr<aas::Data> _top = std::move(prog.stack.back());
		prog.stack.pop_back();

		aas::Data* top = _top.get();
		if(top->type == aas::DataType::REFERENCE){
			top = dynamic_cast<aas::Reference*>(top)->ref.get();
		}

		switch(top->type){
			case aas::DataType::TEXT:
			{
				aas::Text* t = dynamic_cast<aas::Text*>(top);
				prog.stack.emplace_back(new aas::Integer(atol(t->value.c_str())));
			} break;
			case aas::DataType::OBJECT:
			{
				// TODO
			} break;
			default:
				prog.stack.emplace_back(new aas::Data(aas::DataType::ERROR));
				return 2;
		}

		return 0;

	});

	on("val", [](aas::Program& prog, std::size_t& pc){
		if(prog.stack.size() <= 0){
			prog.error = "\"val\": The stack is empty: " + prog.src[pc]->strloc();
			return 1;
		}

		if(prog.stack.back()->type != aas::DataType::REFERENCE)
			return 0;
		
		std::unique_ptr<aas::Data> _top = std::move(prog.stack.back());
		prog.stack.pop_back();

		aas::Data* top = _top.get();
		top = dynamic_cast<aas::Reference*>(top)->ref.get();

		prog.stack.emplace_back(aas::copy(top));

		return 0;

	});

	on("isText", [](aas::Program& prog, std::size_t& pc){
		if(prog.stack.size() <= 0){
			prog.error = "\"isText\": The stack is empty: " + prog.src[pc]->strloc();
			return 1;
		}

		std::unique_ptr<aas::Data>& top = prog.stack.back();

		if(top->type == aas::DataType::TEXT)
			prog.stack.emplace_back(new aas::Integer(1));
		else if(top->type == aas::DataType::REFERENCE && dynamic_cast<aas::Reference*>(top.get())->ref->type == aas::DataType::TEXT)
			prog.stack.emplace_back(new aas::Integer(1));
		else
			prog.stack.emplace_back(new aas::Integer(0));

		return 0;
	});

	on("isInt", [](aas::Program& prog, std::size_t& pc){
		if(prog.stack.size() <= 0){
			prog.error = "\"isInt\": The stack is empty: " + prog.src[pc]->strloc();
			return 1;
		}

		std::unique_ptr<aas::Data>& top = prog.stack.back();

		if(top->type == aas::DataType::INTEGER)
			prog.stack.emplace_back(new aas::Integer(1));
		else if(top->type == aas::DataType::REFERENCE && dynamic_cast<aas::Reference*>(top.get())->ref->type == aas::DataType::INTEGER)
			prog.stack.emplace_back(new aas::Integer(1));
		else
			prog.stack.emplace_back(new aas::Integer(0));

		return 0;
	});

	on("isObj", [](aas::Program& prog, std::size_t& pc){
		if(prog.stack.size() <= 0){
			prog.error = "\"isObj\": The stack is empty: " + prog.src[pc]->strloc();
			return 1;
		}

		std::unique_ptr<aas::Data>& top = prog.stack.back();

		if(top->type == aas::DataType::OBJECT)
			prog.stack.emplace_back(new aas::Integer(1));
		else if(top->type == aas::DataType::REFERENCE && dynamic_cast<aas::Reference*>(top.get())->ref->type == aas::DataType::OBJECT)
			prog.stack.emplace_back(new aas::Integer(1));
		else
			prog.stack.emplace_back(new aas::Integer(0));

		return 0;
	});

	on("ofType", [](aas::Program& prog, std::size_t& pc){
		if(prog.stack.size() <= 0){
			prog.error = "\"ofType\": The stack is empty: " + prog.src[pc]->strloc();
			return 1;
		}

		pc++;
		if(pc >= prog.src.size()){
			prog.error = "\"ofType\": Expected a value: " + prog.src[pc - 1]->strloc();
			return 2;
		}

		std::unique_ptr<aas::Data> data = aas::toData(prog, prog.src[pc].get());
		aas::Text* text = dynamic_cast<aas::Text*>(data.get());
		if(!text){
			prog.error = "\"ofType\": Expected a text: " + prog.src[pc]->strloc();
			return 3;
		}

		aas::Data* top = prog.stack.back().get();
		if(data->type == aas::DataType::REFERENCE){
			top = dynamic_cast<aas::Reference*>(top)->ref.get();
		}

		if(top->type != aas::DataType::OBJECT)
			prog.stack.emplace_back(new aas::Integer(0));
		else if(dynamic_cast<aas::Object*>(top)->name == text->value)
			prog.stack.emplace_back(new aas::Integer(1));
		else
			prog.stack.emplace_back(new aas::Integer(0));

		return 0;
	});
}
