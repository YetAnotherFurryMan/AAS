#pragma once

#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <functional>
#include <string_view>
#include <unordered_map>

namespace aas{
	enum class TokenType{
		ERROR = 0,
		ENDOF,
		NUMBER,
		STRING,
		FSTRING,
		IDENTIFIER,
		STACKREF,
		LABEL
	};

	struct Token{
		TokenType type;
		std::string_view filename;
		std::size_t lineno;
		std::size_t charno;

		Token() = default;
		Token(TokenType type, std::string_view filename, std::size_t lineno, std::size_t charno):
			type{type}, filename{filename}, lineno{lineno}, charno{charno}
		{}

		virtual ~Token() = default;

		inline std::string strloc(){
			return "[" + std::to_string(lineno) + ":" + std::to_string(charno) + "]";
		}

		
	};

	struct Number: public Token{
		int value;

		Number() = default;
		Number(std::string_view filename, std::size_t lineno, std::size_t charno, int value):
			Token{TokenType::NUMBER, filename, lineno, charno},
			value{value}
		{}
	};

	struct String: public Token{
		std::string value;

		String() = default;
		String(std::string_view filename, std::size_t lineno, std::size_t charno, std::string_view value):
			Token{TokenType::STRING, filename, lineno, charno},
			value{value}
		{}
	};

	struct FString: public Token{
		std::string value;

		FString() = default;
		FString(std::string_view filename, std::size_t lineno, std::size_t charno, std::string_view value):
			Token{TokenType::FSTRING, filename, lineno, charno},
			value{value}
		{}
	};

	struct Identifier: public Token{
		std::size_t index;

		Identifier() = default;
		Identifier(std::string_view filename, std::size_t lineno, std::size_t charno, std::size_t index):
			Token{TokenType::IDENTIFIER, filename, lineno, charno},
			index{index}
		{}
	};

	struct StackRef: public Token{
		std::size_t index;

		StackRef() = default;
		StackRef(std::string_view filename, std::size_t lineno, std::size_t charno, std::size_t index):
			Token{TokenType::STACKREF, filename, lineno, charno},
			index{index}
		{}
	};

	struct Label: public Token{
		std::size_t index;

		Label() = default;
		Label(std::string_view filename, std::size_t lineno, std::size_t charno, std::size_t index):
			Token{TokenType::LABEL, filename, lineno, charno},
			index{index}
		{}
	};

	enum class DataType{
		ERROR = 0,
		INTEGER,
		TEXT,
		OBJECT
	};

	struct Data{
		DataType type;

		Data() = default;
		Data(DataType type):
			type{type}
		{}

		virtual ~Data() = default;
	};

	struct Integer: public Data{
		int64_t value;

		Integer() = default;
		Integer(int64_t value):
			Data{DataType::INTEGER},
			value{value}
		{}
	};

	struct Text: public Data{
		std::string value;

		Text() = default;
		Text(const std::string& value):
			Data{DataType::TEXT},
			value{value}
		{}
	};

	// TODO: Destructor? std::shared_ptr?
	struct Object: public Data{
		std::string name;
		void* object;

		Object() = default;
		Object(const std::string& name, void* object):
			Data{DataType::OBJECT},
			name{name},
			object{object}
		{}
	};

	struct Program{
		std::vector<std::string> ids = {""};
		std::unordered_map<std::string, std::size_t> id_dict;
		std::unordered_map<std::size_t, std::function<int(Program&, std::size_t&)>> commands;
		std::unordered_map<std::size_t, std::size_t> labels;
		std::vector<std::unique_ptr<Token>> src;
		std::string error;

		std::vector<std::unique_ptr<Data>> stack;
		std::unordered_map<std::string, std::unique_ptr<Data>> vars;

		bool compile(std::string_view name, std::istream& in);
		std::unique_ptr<Token> next(std::istream& in, std::string_view filename, std::size_t& lineno, std::size_t& charno, std::size_t pc);
		int run();

		void useStack();
		void useVars();
		void useTypes();
		void useMath();
		void useFlow();

		inline void on(const std::string& id, std::function<int(Program&, std::size_t&)> cmd){
			if(!id_dict[id]){
				id_dict[id] = ids.size();
				ids.push_back(id);
			}

			commands[id_dict[id]] = cmd;
		}

		inline void alias(const std::string& id, const std::string& alias){
			if(!id_dict[id]){
				id_dict[id] = ids.size();
				ids.push_back(id);
			}

			id_dict[alias] = id_dict[id];
		}
	};

	inline std::unique_ptr<Data> copy(Data* data){
		switch(data->type){
			case DataType::INTEGER:
			{
				Integer* i = dynamic_cast<Integer*>(data);
				return std::make_unique<Integer>(i->value);
			} break;
			case DataType::TEXT:
			{
				Text* t = dynamic_cast<Text*>(data);
				return std::make_unique<Text>(t->value);
			} break;
			case DataType::OBJECT:
			{
				Object* o = dynamic_cast<Object*>(data);
				return std::make_unique<Object>(o->name, o->object);
			} break;
			default:
				return std::make_unique<Data>(data->type);
		}
	}

	inline std::unique_ptr<Data> toData(Program& prog, Token* tok){
		switch(tok->type){
			case TokenType::NUMBER:
			{
				Number* num = dynamic_cast<Number*>(tok);
				return std::make_unique<Integer>(num->value);
			} break;
			case TokenType::STRING:
			{
				String* str = dynamic_cast<String*>(tok);
				return std::make_unique<Text>(str->value);
			} break;
			case TokenType::FSTRING:
			{
				FString* fstr = dynamic_cast<FString*>(tok);
				return std::make_unique<Text>(fstr->value);
			} break;
			case TokenType::IDENTIFIER:
			{
				Identifier* id = dynamic_cast<Identifier*>(tok);
				return std::make_unique<Integer>(id->index);
			} break;
			case TokenType::STACKREF:
			{
				StackRef* sr = dynamic_cast<StackRef*>(tok);
				if(prog.stack.size() < sr->index){
					prog.error = "Failed to resolve stack reference: " + tok->strloc();
					return std::make_unique<Data>(DataType::ERROR);
				}

				return copy(prog.stack[prog.stack.size() - sr->index].get());
			} break;
			case TokenType::LABEL:
			{
				Label* label = dynamic_cast<Label*>(tok);
				return std::make_unique<Integer>(label->index);
			} break;
			default:
				prog.error = "Failed to convert token to data: " + tok->strloc();
				return std::make_unique<Data>(DataType::ERROR);
		}
	}
}
