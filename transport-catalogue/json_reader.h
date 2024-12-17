#pragma once
#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "json_builder.h"
#include "transport_router.h"

using namespace json;
class JsonReader
{
public:
	JsonReader(std::istream& inp) : doc_(std::move(Load(inp)))
	{}
	void LoadDB(catalogue::TransportCatalogue& cat);
	void PrintDB(const RequestHandler& handler, std::ostream& out) const;
	void GetRenderSettings(renderer::MapRenderer& map_rend);
	rout::TransportRouter GetRoutSettings(const catalogue::TransportCatalogue& cat);
private:	
	template<typename F>
	void ParseArrayMaps(const Array& array, F func) const
	{
		for (const auto& node : array)
		{
			func(node.AsDict());
		}
	}

	template<typename F>
	void ParseArrayStrings(const Array& array, F func) const
	{
		for (const auto& node : array)
		{
			func(node.AsString());
		}
	}

	template<typename F>
	void ParseMap(const Dict& dict, F func) const
	{
		for (const auto& [Key, Val] : dict)
		{
			func(Key, Val);
		}
	}

	svg::Color ParseColor(const Node& node) const;
	Document doc_;
};

