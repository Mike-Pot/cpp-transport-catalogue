#pragma once
#include "json.h"

namespace json
{
	class Builder;

	class ErrCatcher
	{
	public:
		ErrCatcher(Builder& b) : builder_(b) {}
	protected:
		Builder& builder_;		
	};

	class KeyCntxt;
	class DictCntxt;
	class ArrCntxt;
	class Starter;

	class Builder
	{
	public:
		Builder() : err_catcher_(*this) {}
		Builder& Value(Node::Value val);
		ArrCntxt& StartArray();
		Builder& EndArray();		
		DictCntxt& StartDict();
		Builder& EndDict();		
		KeyCntxt& Key(std::string key);
		Node Build();
		~Builder();
	private:
		template<typename T>
		Node* Value_(T val)
		{
			return new Node(std::move(val));
		}
		ErrCatcher err_catcher_;
		std::vector<Node*> nodes_;
		enum class State { ARR, DICT };
		std::vector<State> cntxt;
	};

	class Starter : public ErrCatcher
	{
	public:
		ArrCntxt& StartArray();
		DictCntxt& StartDict();
	};

	class DictCntxt : public ErrCatcher
	{
	public:
		KeyCntxt& Key(std::string key);
		Builder& EndDict();
	};

	class KeyCntxt : public Starter
	{
	public:
		DictCntxt& Value(Node::Value val);
	};

	class ArrCntxt : public Starter
	{
	public:
		ArrCntxt& Value(Node::Value val);
		Builder& EndArray();
	};
}
