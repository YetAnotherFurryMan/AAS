#include <aas_fio.hpp>

int aas::fio::close(aas::Program& prog, std::size_t& pc){
	if(prog.stack.size() <= 0){
		prog.error = "\"fio.close\": Stack is empty: " + prog.src[pc]->strloc();
		return 1;
	}

	aas::Data* data = prog.stack.back().get();
	if(data->type != aas::DataType::OBJECT){
		prog.error = "\"fio.close\": Failed to close non-object: " + prog.src[pc]->strloc();
		return 2;
	}

	aas::Object* object = dynamic_cast<aas::Object*>(data);
	if(object->name != "fio.File"){
		prog.error = "\"fio.close\": Failed to close, not a fio.File: " + prog.src[pc]->strloc();
		return 3;
	}

	aas::fio::File* file = dynamic_cast<aas::fio::File*>(object->object.get());
	if(!file->opened){
		file->file.close();
		file->opened = false;
	}

	return 0;
}
