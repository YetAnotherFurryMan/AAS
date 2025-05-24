#include <aas_fio.hpp>

static int s_write(aas::fio::File* file, aas::Data* data){
	switch(data->type){
		case aas::DataType::INTEGER:
		{
			aas::Integer* i = dynamic_cast<aas::Integer*>(data);
			file->file << i->value;
		} break;
		case aas::DataType::TEXT:
		{
			aas::Text* t = dynamic_cast<aas::Text*>(data);
			file->file << t->value;
		} break;
		case aas::DataType::OBJECT:
		{
			aas::Object* o = dynamic_cast<aas::Object*>(data);
			file->file << "[" << o->name << ": " << o->object << "]";
		} break;
		case aas::DataType::REFERENCE:
		{
			aas::Reference* r = dynamic_cast<aas::Reference*>(data);
			return s_write(file, r->ref.get());
		}
		default:
			return 1;
	}

	return 0;
}

inline int s_write(aas::Program& prog, aas::fio::File* file, aas::Token* tok){
	if(tok->type == aas::TokenType::IDENTIFIER){
		aas::Identifier* id = dynamic_cast<aas::Identifier*>(tok);
		file->file << "[" << id->index << ": " << prog.ids[id->index] << "]";
		return 0;
	}

	std::unique_ptr<aas::Data> p = aas::toData(prog, tok);
	return s_write(file, p.get());
}

int aas::fio::write(aas::Program& prog, std::size_t& pc){
	if(prog.stack.size() <= 0){
		prog.error = "\"fio.write\": Stack is empty: " + prog.src[pc]->strloc();
		return 1;
	}

	if(prog.stack.size() < 2){
		prog.error = "\"fio.write\": Not enought arguments on the stack: " + prog.src[pc]->strloc();
		return 2;
	}

	std::unique_ptr<aas::Data> toPrint = std::move(prog.stack.back());
	prog.stack.pop_back();

	aas::Data* data = prog.stack.back().get();
	if(data->type != aas::DataType::OBJECT){
		prog.error = "\"fio.write\": Failed to write non-object: " + prog.src[pc]->strloc();
		return 3;
	}

	aas::Object* object = dynamic_cast<aas::Object*>(data);
	if(object->name != "fio.File"){
		prog.error = "\"fio.write\": Failed to write, not a fio.File: " + prog.src[pc]->strloc();
		return 4;
	}

	aas::fio::File* file = dynamic_cast<aas::fio::File*>(object->object.get());
	if(!file->opened){
		prog.error = "\"fio.write\": Failed to write, file closed: " + prog.src[pc]->strloc();
		return 5;
	}

	if(s_write(file, toPrint.get())){
		prog.error = "\"fio.write\": Unwritable stack value: " + prog.src[pc]->strloc();
		return 6;
	}

	return 0;
}

int aas::fio::writev(aas::Program& prog, std::size_t& pc){
	if(prog.stack.size() <= 0){
		prog.error = "\"fio.writev\": Stack is empty: " + prog.src[pc]->strloc();
		return 1;
	}

	aas::Data* data = prog.stack.back().get();
	if(data->type != aas::DataType::OBJECT){
		prog.error = "\"fio.writev\": Failed to write non-object: " + prog.src[pc]->strloc();
		return 2;
	}

	aas::Object* object = dynamic_cast<aas::Object*>(data);
	if(object->name != "fio.File"){
		prog.error = "\"fio.writev\": Failed to write, not a fio.File: " + prog.src[pc]->strloc();
		return 3;
	}

	aas::fio::File* file = dynamic_cast<aas::fio::File*>(object->object.get());
	if(!file->opened){
		prog.error = "\"fio.writev\": Failed to write, file closed: " + prog.src[pc]->strloc();
		return 4;
	}

	pc++;
	if(pc >= prog.src.size()){
		prog.error = "\"fio.writev\": Expected a value, but got nothing: " + prog.src[pc - 1]->strloc();
		return 5;
	}

	if(s_write(prog, file, prog.src[pc].get())){
		prog.error = "\"fio.writev\": Unwritable value: " + prog.src[pc]->strloc();
		return 6;
	}

	return 0;
}
