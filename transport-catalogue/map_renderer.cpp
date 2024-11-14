#include "map_renderer.h"
#include <unordered_map>
namespace renderer
{
	MapRenderer::~MapRenderer()
	{
		delete[] draw_sets_.color_palette;
	}
	
	svg::Document MapRenderer::RenderMap(const catalogue::BUSES& all_buses, const catalogue::STOPS& all_stops) const
	{
		using namespace catalogue;
		svg::Document map;
		std::vector<geo::Coordinates> geo_coords;
		for (auto stop : all_stops)
		{
			geo_coords.push_back(stop->coord);
		}

		SphereProjector proj{
			geo_coords.begin(), geo_coords.end(),
			draw_sets_.width, draw_sets_.height, draw_sets_.padding
		};
		std::unordered_map<const Stop_*, svg::Point> stop_projs;
		for (auto stop : all_stops)
		{
			stop_projs.insert({ stop,proj(stop->coord) });
		}

		size_t curr_col = 0;
		for (auto bus : all_buses)
		{
			std::vector<svg::Point> points;
			STOPS stops = MakeFullRoute(bus);
			for (auto stop : stops)
			{
				points.push_back(stop_projs.at(stop));
			}
			AddRouteToMap(map, points, curr_col);
			curr_col = (curr_col + 1) % draw_sets_.num_cols;
		}

		curr_col = 0;
		for (auto bus : all_buses)
		{
			const STOPS& stops = bus->route;
			AddRouteNameToMap(map, bus->num, curr_col,
				stop_projs.at(stops.front()));
			if (!bus->is_round && stops.front() != stops.back())
			{
				AddRouteNameToMap(map, bus->num, curr_col,
					stop_projs.at(stops.back()));
			}
			curr_col = (curr_col + 1) % draw_sets_.num_cols;
		}

		for (auto stop : all_stops)
		{
			AddStopToMap(map, stop_projs.at(stop));
		}

		for (auto stop : all_stops)
		{
			AddStopNameToMap(map, stop->name, stop_projs.at(stop));
		}
		return map;
	}

	void MapRenderer::PutRenderSettings(DrawSettings& rend_sets)
	{
		draw_sets_ = std::move(rend_sets);		
	}

	void MapRenderer::AddRouteToMap(svg::Document& map_, std::vector<svg::Point>& points, size_t color) const
	{	
		using namespace svg;
		Polyline route;
		route.SetFillColor(NoneColor)
			.SetStrokeColor(draw_sets_.color_palette[color])
			.SetStrokeWidth(draw_sets_.line_width)
			.SetStrokeLineCap(StrokeLineCap::ROUND)
			.SetStrokeLineJoin(StrokeLineJoin::ROUND);
		for (svg::Point point : points)
		{
			route.AddPoint(point);
		}
		map_.Add(route);		
	}

	void MapRenderer::AddRouteNameToMap(svg::Document& map_, std::string name, size_t color, svg::Point coor) const
	{
		using namespace svg;
		Text base;
		base.SetPosition(coor)
			.SetOffset({ draw_sets_.bus_label_offset.dx,draw_sets_.bus_label_offset.dy })
			.SetFontSize(draw_sets_.bus_label_font_size)
			.SetFontFamily("Verdana")
			.SetFontWeight("bold")
			.SetData(name);
		Text label = base;
		base.SetFillColor(draw_sets_.underlayer_color)
			.SetStrokeColor(draw_sets_.underlayer_color)
			.SetStrokeWidth(draw_sets_.underlayer_width)
			.SetStrokeLineCap(StrokeLineCap::ROUND)
			.SetStrokeLineJoin(StrokeLineJoin::ROUND);
		label.SetFillColor(draw_sets_.color_palette[color]);
		map_.Add(base);
		map_.Add(label);
	}

	void MapRenderer::AddStopToMap(svg::Document& map_, svg::Point coor) const
	{
		using namespace svg;
		Circle stop;
		stop.SetCenter(coor)
			.SetRadius(draw_sets_.stop_radius)
			.SetFillColor("white");
		map_.Add(stop);
	}

	void MapRenderer::AddStopNameToMap(svg::Document& map_, std::string name, svg::Point coor) const
	{
		using namespace svg;
		Text base;
		base.SetPosition(coor)
			.SetOffset({ draw_sets_.stop_label_offset.dx,draw_sets_.stop_label_offset.dy })
			.SetFontSize(draw_sets_.stop_label_font_size)
			.SetFontFamily("Verdana")			
			.SetData(name);
		Text label = base;
		base.SetFillColor(draw_sets_.underlayer_color)
			.SetStrokeColor(draw_sets_.underlayer_color)
			.SetStrokeWidth(draw_sets_.underlayer_width)
			.SetStrokeLineCap(StrokeLineCap::ROUND)
			.SetStrokeLineJoin(StrokeLineJoin::ROUND);
		label.SetFillColor("black");
		map_.Add(base);
		map_.Add(label);
	}
}