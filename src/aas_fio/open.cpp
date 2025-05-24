#include <aas_fio.hpp>

static int s_open(aas::Program& prog, aas::Data* data, std::ios_base::openmode mode){
	if(data->type == aas::DataType::REFERENCE){
		aas::Reference* r = dynamic_cast<aas::Reference*>(data);
		return s_open(prog, r->ref.get(), mode);
	}

	if(data->type != aas::DataType::TEXT){
		prog.error = "Expected text";
		return 2;
	}

	aas::Text* t = dynamic_cast<aas::Text*>(data);
	prog.stack.emplace_back(new aas::Object("fio.File", std::make_shared<aas::fio::File>(t->value, mode)));

	return 0;
}

inline int s_open(aas::Program& prog, aas::Token* tok, std::ios_base::openmode mode){
	std::unique_ptr<aas::Data> p = aas::toData(prog, tok);
	return s_open(prog, p.get(), mode);
}

int aas::fio::openin(aas::Program& prog, std::size_t& pc){
	if(prog.stack.size() <= 0){
		prog.error = "\"fio.open.in\": Stack is empty: " + prog.src[pc]->strloc();
		return 1;
	}

	int ret = s_open(prog, prog.stack.back().get(), std::ios::in);
	if(ret)
		prog.error = "\"fio.open.in\": " + prog.error + ": " + prog.src[pc]->strloc();
	return ret;
}

int aas::fio::openinv(aas::Program& prog, std::size_t& pc){
	pc++;
	if(pc >= prog.src.size()){
		prog.error = "\"fio.open.inv\": Expected a value, but got nothing: " + prog.src[pc - 1]->strloc();
		return 1;
	}

	int ret = s_open(prog, prog.src[pc].get(), std::ios::in);
	if(ret)
		prog.error = "\"fio.open.inv\": " + prog.error + ": " + prog.src[pc]->strloc();
	return ret;
}

int aas::fio::openout(aas::Program& prog, std::size_t& pc){
	if(prog.stack.size() <= 0){
		prog.error = "\"fio.open.out\": Stack is empty: " + prog.src[pc]->strloc();
		return 1;
	}

	int ret = s_open(prog, prog.stack.back().get(), std::ios::out);
	if(ret)
		prog.error = "\"fio.open.out\": " + prog.error + ": " + prog.src[pc]->strloc();
	return ret;
}

int aas::fio::openoutv(aas::Program& prog, std::size_t& pc){
	pc++;
	if(pc >= prog.src.size()){
		prog.error = "\"fio.open.outv\": Expected a value, but got nothing: " + prog.src[pc - 1]->strloc();
		return 1;
	}

	int ret = s_open(prog, prog.src[pc].get(), std::ios::out);
	if(ret)
		prog.error = "\"fio.open.outv\": " + prog.error + ": " + prog.src[pc]->strloc();
	return ret;
}

int aas::fio::openapp(aas::Program& prog, std::size_t& pc){
	if(prog.stack.size() <= 0){
		prog.error = "\"fio.open.app\": Stack is empty: " + prog.src[pc]->strloc();
		return 1;
	}

	int ret = s_open(prog, prog.stack.back().get(), std::ios::out | std::ios::app);
	if(ret)
		prog.error = "\"fio.open.app\": " + prog.error + ": " + prog.src[pc]->strloc();
	return ret;
}

int aas::fio::openappv(aas::Program& prog, std::size_t& pc){
	pc++;
	if(pc >= prog.src.size()){
		prog.error = "\"fio.open.appv\": Expected a value, but got nothing: " + prog.src[pc - 1]->strloc();
		return 1;
	}

	int ret = s_open(prog, prog.src[pc].get(), std::ios::out | std::ios::app);
	if(ret)
		prog.error = "\"fio.open.appv\": " + prog.error + ": " + prog.src[pc]->strloc();
	return ret;
}
