#include "map_renderer.h"
namespace renderer
{
	MapRenderer::~MapRenderer()
	{
		delete[] draw_sets_.color_palette;
	}
	
	void MapRenderer::PutRenderSettings(DrawSettings& rend_sets)
	{
		draw_sets_ = std::move(rend_sets);		
	}

	void MapRenderer::AddRouteToMap(svg::Document& map_, std::vector<svg::Point>& points, size_t curr_col) const
	{	
		using namespace svg;
		Polyline route;
		route.SetFillColor(NoneColor)
			.SetStrokeColor(draw_sets_.color_palette[curr_col])
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