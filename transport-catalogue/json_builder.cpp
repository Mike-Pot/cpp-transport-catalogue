#include "json_builder.h"

namespace json
{
	Builder& Builder::Value(Node::Value val)
	{		
		Node* node;
		std::visit([&](const auto& val_)
			{
				node = Value_(val_);
			}, val);
		nodes_.push_back(node);
		return *this;
	}

	ArrCntxt& Builder::StartArray()
	{
		cntxt.push_back(2);		
		nodes_.push_back(nullptr);
		return (ArrCntxt&)err_catcher_;
	}

	Builder& Builder::EndArray()
	{		
		if (cntxt.empty() || cntxt.back() != 2)
		{
			throw std::logic_error("");
		}
		cntxt.pop_back();
		Array arr{};
		size_t i = nodes_.size() - 1;
		while (nodes_[i])
		{
			i--;
		}
		for (size_t j = i + 1; j < nodes_.size(); j++)
		{
			arr.push_back(std::move(*nodes_[j]));
		}
		nodes_[i] = new Node(std::move(arr));
		nodes_.resize(i + 1);
		return *this;
	}

	DictCntxt& Builder::StartDict()
	{
		cntxt.push_back(1);		
		nodes_.push_back(nullptr);
		return (DictCntxt&)err_catcher_;
	}

	Builder& Builder::EndDict()
	{
		if (cntxt.empty() || cntxt.back() != 1)
		{
			throw std::logic_error("");
		}
		cntxt.pop_back();
		Dict dict{};
		while (nodes_.back())
		{
			Node* val = nodes_.back();
			nodes_.pop_back();
			dict.insert({ nodes_.back()->AsString(),std::move(*val) });
			nodes_.pop_back();
		}
		nodes_.back() = new Node(std::move(dict));
		return *this;
	}

	KeyCntxt& Builder::Key(std::string key)
	{
		nodes_.push_back(new Node(std::move(key)));
		return (KeyCntxt&)err_catcher_;
	}

	Node Builder::Build()
	{
		if (nodes_.size() != 1)
		{
			throw std::logic_error("Build call for unready node");
		}
		return std::move(*nodes_.back());
	}

	Builder::~Builder()
	{
		for (auto node : nodes_)
		{
			delete node;
		}
	}

	KeyCntxt& DictCntxt::Key(std::string key)
	{
		return builder_.Key(key);		
	}

	Builder& DictCntxt::EndDict()
	{
		return builder_.EndDict();		
	}

	DictCntxt& KeyCntxt::Value(Node::Value val)
	{		
		builder_.Value(val);
		return (DictCntxt&)*this;		
	}

	ArrCntxt& Starter::StartArray()
	{
		return builder_.StartArray();		
	}

	DictCntxt& Starter::StartDict()
	{
		return builder_.StartDict();		
	}

	ArrCntxt& ArrCntxt::Value(Node::Value val)
	{		
		builder_.Value(val);
		return (ArrCntxt&)*this;		
	}

	Builder& ArrCntxt::EndArray()
	{
		return builder_.EndArray();		
	}
}
