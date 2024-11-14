#include "json_reader.h"
#include <sstream>

void JsonReader::LoadDB(catalogue::TransportCatalogue& cat)
{
	auto add_stop = [&cat](const Dict& stop)
	{
		if (stop.at("type").AsString() == "Stop")
		{
			cat.AddStop(stop.at("name").AsString(),
				{ stop.at("latitude").AsDouble(),stop.at("longitude").AsDouble(), });
		}
	};

	auto add_stop_dists = [&](const Dict& stop)
	{
		if (stop.at("type").AsString() == "Stop")
		{
			auto stop_from = stop.at("name").AsString();
			ParseMap(stop.at("road_distances").AsMap(),
				[&cat,stop_from, *this](std::string stop_to, const Node& dist)
			{
					cat.AddStopsDist(stop_from,
					stop_to, dist.AsInt());
			});
		}
	};

	auto add_bus = [&](const Dict& bus)
	{
		if (bus.at("type").AsString() == "Bus")
		{			
			std::vector<std::string> stop_names;
			ParseArrayStrings(bus.at("stops").AsArray(),
				[&](const auto stop)
				{					
					stop_names.push_back(stop);
				});			
			cat.AddRoute(bus.at("name").AsString(), stop_names, bus.at("is_roundtrip").AsBool());
		}
	};

	const Array& req = doc_.GetRoot().AsMap().at("base_requests").AsArray();
	ParseArrayMaps(req, add_stop);
	ParseArrayMaps(req, add_stop_dists);
	ParseArrayMaps(req, add_bus);
}

void JsonReader::PrintDB(const RequestHandler& handler, std::ostream& out) const
{	
	Array ans;
	std::string err_mes = "not found";
	auto add_bus = [&](const Dict& bus, int id)
	{
		auto stat = handler.GetStat(bus.at("name").AsString());
		
		if (stat)
		{
			ans.push_back({ {
				{"request_id",id},
				{"stop_count",stat->stops},
				{"unique_stop_count",stat->unique_stops},
				{"route_length",stat->dist},
				{"curvature",stat->curv} } });
		}
		else
		{
			ans.push_back({ {
				{"request_id",id},
				{"error_message",err_mes} } });
		}	
	};

	auto add_stop = [&](const Dict& stop, int id)
	{
		auto buses = handler.GetBusesWithStop(stop.at("name").AsString());

		if (buses)
		{
			Array route;
			for (auto bus : **buses)
			{
				route.push_back(std::string(bus));
			}
			ans.push_back({ {
				{"request_id",id},
				{"buses",route} } });
		}
		else
		{			
			ans.push_back({ {
				{"request_id",id},
				{"error_message",err_mes} } });
		}
	};

	auto add_ans_el = [&](const Dict& req_el)
	{
		int req_id = req_el.at("id").AsInt();				
		if (req_el.at("type").AsString() == "Map")
		{
			svg::Document map = handler.RenderMap();
			std::ostringstream out;
			map.Render(out);
			ans.push_back({ {
				{"request_id",req_id},
				{"map",out.str()} } });
		}
		if (req_el.at("type").AsString() == "Bus")
		{
			add_bus(req_el, req_id);			
		}
		if (req_el.at("type").AsString() == "Stop")
		{
			add_stop(req_el, req_id);			
		}
	};
	
	ParseArrayMaps(doc_.GetRoot().AsMap().at("stat_requests").AsArray(), add_ans_el);
	Print(Document{ ans }, out);
}

void JsonReader::GetRenderSettings(renderer::MapRenderer& map_rend)
{
	using namespace renderer;	
	renderer::DrawSettings rend_sets;

	auto set_renders = [&](std::string Key, const Node& Val)
	{
		if (Key == "width")
		{
			rend_sets.width = Val.AsDouble();
		}
		if (Key == "height")
		{
			rend_sets.height = Val.AsDouble();
		}
		if (Key == "padding")
		{
			rend_sets.padding = Val.AsDouble();
		}
		if (Key == "line_width")
		{
			rend_sets.line_width = Val.AsDouble();
		}
		if (Key == "stop_radius")
		{
			rend_sets.stop_radius = Val.AsDouble();
		}
		if (Key == "bus_label_font_size")
		{
			rend_sets.bus_label_font_size = Val.AsInt();
		}
		if (Key == "bus_label_offset")
		{			
			rend_sets.bus_label_offset.dx = Val.AsArray()[0].AsDouble();
			rend_sets.bus_label_offset.dy = Val.AsArray()[1].AsDouble();
		}
		if (Key == "stop_label_font_size")
		{
			rend_sets.stop_label_font_size = Val.AsInt();
		}
		if (Key == "stop_label_offset")
		{			
			rend_sets.stop_label_offset.dx = Val.AsArray()[0].AsDouble();
			rend_sets.stop_label_offset.dy = Val.AsArray()[1].AsDouble();
		}
		if (Key == "underlayer_width")
		{
			rend_sets.underlayer_width = Val.AsDouble();
		}
		if (Key == "underlayer_color")
		{
			rend_sets.underlayer_color = ParseColor(Val);
		}
		if (Key == "color_palette")
		{
			const Array& cols = Val.AsArray();
			svg::Color* parsed_cols = new svg::Color[cols.size()];			
			for (size_t i = 0; i < cols.size(); i++)
			{
				parsed_cols[i] = ParseColor(cols[i]);
			}
			rend_sets.color_palette = parsed_cols;
			rend_sets.num_cols = cols.size();
		}
	};

	ParseMap(doc_.GetRoot().AsMap().at("render_settings").AsMap(), set_renders);
	map_rend.PutRenderSettings(rend_sets);
}

svg::Color JsonReader::ParseColor(const Node& node) const
{

	if (node.IsString())
	{
		return node.AsString();
	}
	else
	{
		const Array& color = node.AsArray();
		if (color.size() == 3)
		{		
			svg::Rgb col;			
			col.red = color[0].AsInt();
			col.green = color[1].AsInt();
			col.blue = color[2].AsInt();
			return col;
		}
		else
		{			
			svg::Rgba col;			
			col.red = color[0].AsInt();
			col.green = color[1].AsInt();
			col.blue = color[2].AsInt();
			col.opacity = color[3].AsDouble();
			return col;			
		}
	}
}
