#include <aas_fio.hpp>

int aas::fio::read(aas::Program& prog, std::size_t& pc){
	if(prog.stack.size() <= 0){
		prog.error = "\"fio.read\": Stack is empty: " + prog.src[pc]->strloc();
		return 1;
	}

	aas::Data* data = prog.stack.back().get();
	if(data->type != aas::DataType::OBJECT){
		prog.error = "\"fio.read\": Failed to read non-object: " + prog.src[pc]->strloc();
		return 2;
	}

	aas::Object* object = dynamic_cast<aas::Object*>(data);
	if(object->name != "fio.File"){
		prog.error = "\"fio.read\": Failed to read, not a fio.File: " + prog.src[pc]->strloc();
		return 3;
	}

	aas::fio::File* file = dynamic_cast<aas::fio::File*>(object->object.get());
	if(!file->opened){
		prog.error = "\"fio.read\": Failed to read, file closed: " + prog.src[pc]->strloc();
		return 4;
	}

	std::string text;
	file->file >> text;
	prog.stack.emplace_back(new aas::Text(text));

	return 0;
}

int aas::fio::readl(aas::Program& prog, std::size_t& pc){
	if(prog.stack.size() <= 0){
		prog.error = "\"fio.readl\": Stack is empty: " + prog.src[pc]->strloc();
		return 1;
	}

	aas::Data* data = prog.stack.back().get();
	if(data->type != aas::DataType::OBJECT){
		prog.error = "\"fio.readl\": Failed to read non-object: " + prog.src[pc]->strloc();
		return 2;
	}

	aas::Object* object = dynamic_cast<aas::Object*>(data);
	if(object->name != "fio.File"){
		prog.error = "\"fio.readl\": Failed to read, not a fio.File: " + prog.src[pc]->strloc();
		return 3;
	}

	aas::fio::File* file = dynamic_cast<aas::fio::File*>(object->object.get());
	if(!file->opened){
		prog.error = "\"fio.readl\": Failed to read, file closed: " + prog.src[pc]->strloc();
		return 4;
	}

	std::string text;
	std::getline(file->file, text);
	prog.stack.emplace_back(new aas::Text(text));

	return 0;
}

int aas::fio::readi(aas::Program& prog, std::size_t& pc){
	if(prog.stack.size() <= 0){
		prog.error = "\"fio.readi\": Stack is empty: " + prog.src[pc]->strloc();
		return 1;
	}

	aas::Data* data = prog.stack.back().get();
	if(data->type != aas::DataType::OBJECT){
		prog.error = "\"fio.readi\": Failed to read non-object: " + prog.src[pc]->strloc();
		return 2;
	}

	aas::Object* object = dynamic_cast<aas::Object*>(data);
	if(object->name != "fio.File"){
		prog.error = "\"fio.readi\": Failed to read, not a fio.File: " + prog.src[pc]->strloc();
		return 3;
	}

	aas::fio::File* file = dynamic_cast<aas::fio::File*>(object->object.get());
	if(!file->opened){
		prog.error = "\"fio.readi\": Failed to read, file closed: " + prog.src[pc]->strloc();
		return 4;
	}

	int64_t i;
	file->file >> i;
	prog.stack.emplace_back(new aas::Integer(i));

	return 0;
}
