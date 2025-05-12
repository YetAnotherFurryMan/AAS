#include <aas.hpp>

static inline bool s_trim(std::istream& in, std::size_t& lineno, std::size_t& charno){
	char c = in.peek();
	while(std::isspace(c) && !in.eof()){
		c = in.get();
		if(c == '\n'){
			lineno++;
			charno = 0;
		}
		charno++;
		c = in.peek();
	}

	return in.eof();
}

std::unique_ptr<aas::Token> aas::Program::next(std::istream& in, std::string_view filename, std::size_t& lineno, std::size_t& charno, std::size_t pc){
	if(s_trim(in, lineno, charno))
		return std::make_unique<aas::Token>(aas::TokenType::ENDOF, filename, lineno, charno);

	char c = in.peek();
	if(c == '#'){
		while(c != '\n' && !in.eof())
			c = in.get();

		charno = 0;
		return next(in, filename, ++lineno, charno, pc);
	}
	
	if(c == '-' || c == '$' || (c >= '0' && c <= '9')){
		std::string txt = "";
		std::size_t cno = charno;
		do{
			charno++;
			txt += in.get();
			c = in.peek();
		} while(c >= '0' && c <= '9');

		if(txt == "-"){
			charno = cno;
		} else if(txt[0] == '$'){
			return std::make_unique<aas::StackRef>(filename, lineno, cno, (txt == "$" ? 0 : std::stoul(txt.c_str() + 1)) + 1);
		} else {
			return std::make_unique<aas::Number>(filename, lineno, cno, std::stol(txt.c_str()));
		}
	} else if(c == '@' || c == '_' || c == '.' || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')){
		std::string txt = "";
		std::size_t cno = charno;
		bool label = c == '@';
		do{
			charno++;
			txt += in.get();
			c = in.peek();
		} while(c == '_' || c == '.' || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'));

		if(label)
			txt = txt.substr(1);

		if(txt.empty())
			return std::make_unique<aas::Number>(filename, lineno, cno, pc);

		if(!id_dict[txt]){
			id_dict[txt] = ids.size();
			ids.emplace_back(txt);
		}

		if(label){
			labels[id_dict[txt]] = pc;
			return std::make_unique<aas::Label>(filename, lineno, cno, id_dict[txt]);
		}

		return std::make_unique<aas::Identifier>(filename, lineno, cno, id_dict[txt]);
	} else if(c == '\''){
		// Parse string (no escapes)
		std::string txt = "";
		std::size_t cno = charno;
		in.get();
		charno++;
		c = in.peek();
		while(c != '\'' && !in.eof()){
			if(c == '\n'){
				lineno++;
				charno = 0;
			}

			txt += in.get();
			c = in.peek();
			charno++;
		}

		if(!in.eof()){
			charno++;
			in.get();
			return std::make_unique<aas::String>(filename, lineno, cno, txt);
		}
	} else if(c == '\"'){
		// Parse string (no escapes)
		std::string txt = "";
		std::size_t cno = charno;
		in.get();
		charno++;
		c = in.peek();
		while(c != '\"' && !in.eof()){
			switch(c){
				case '\\':
				{
					in.get();
					charno++;
					c = in.peek();
					switch(c){
						case 'x':
						{
							char x = 0;
							c = in.get();
							charno++;
							if(c >= '0' && c <= '9') x = c - '0';
							else if(c >= 'A' && c <= 'F') x = c - 'A' + 10;
							else if(c >= 'a' && c <= 'f') x = c - 'a' + 10;
							else goto err;

							x *= 16;

							c = in.peek(); // Will be consumed later
							if(c >= '0' && c <= '9') x += c - '0';
							else if(c >= 'A' && c <= 'F') x += c - 'A' + 10;
							else if(c >= 'a' && c <= 'f') x += c - 'a' + 10;
							else goto err;

							txt += x;
						} break;
						case 't':
						{
							txt += "\t";
						} break;
						case 'n':
						{
							txt += "\n";
						} break;
						case 'r':
						{
							txt += "\r";
						} break;
						case '\\':
						{
							txt += c;
						} break;
						default:
							return std::make_unique<aas::Token>(aas::TokenType::ERROR, filename, lineno, cno);
					}
					
					in.get();
					charno++;
				} break;
				case '\n':
					lineno++;
					charno = 0;
				default:
					txt += in.get();
					charno++;
			}
			
			c = in.peek();
		}

		if(!in.eof()){
			charno++;
			in.get();
			return std::make_unique<aas::FString>(filename, lineno, cno, txt);
		}
	}

err:
	return std::make_unique<aas::Token>(aas::TokenType::ERROR, filename, lineno, charno);
}
