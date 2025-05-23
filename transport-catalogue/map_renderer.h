#pragma once
#include "svg.h"
#include "geo.h"
#include "domain.h"
#include <algorithm>

namespace renderer
{     
    struct DrawSettings
    {
        double width = 0;
        double height = 0;
        double padding = 0;
        size_t num_cols = 0;
        double line_width = 0;
        double stop_radius = 0;
        int bus_label_font_size = 0;
        struct {
            double dx = 0;
            double dy = 0;
        } bus_label_offset;
        int stop_label_font_size = 0;
        struct {
            double dx = 0;
            double dy = 0;
        } stop_label_offset;
        svg::Color underlayer_color;
        double underlayer_width = 0;
        svg::Color* color_palette = nullptr;        
    };

    inline const double EPSILON = 1e-6;
    inline bool IsZero(double value) {
        return std::abs(value) < EPSILON;
    }

    class SphereProjector {
    public:
        // points_begin � points_end ������ ������ � ����� ��������� ��������� geo::Coordinates
        template <typename PointInputIt>
        SphereProjector(PointInputIt points_begin, PointInputIt points_end,
            double max_width, double max_height, double padding)
            : padding_(padding) //
        {
            // ���� ����� ����������� ����� �� ������, ��������� ������
            if (points_begin == points_end) {
                return;
            }

            // ������� ����� � ����������� � ������������ ��������
            const auto [left_it, right_it] = std::minmax_element(
                points_begin, points_end,
                [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
            min_lon_ = left_it->lng;
            const double max_lon = right_it->lng;

            // ������� ����� � ����������� � ������������ �������
            const auto [bottom_it, top_it] = std::minmax_element(
                points_begin, points_end,
                [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
            const double min_lat = bottom_it->lat;
            max_lat_ = top_it->lat;

            // ��������� ����������� ��������������� ����� ���������� x
            std::optional<double> width_zoom;
            if (!IsZero(max_lon - min_lon_)) {
                width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
            }

            // ��������� ����������� ��������������� ����� ���������� y
            std::optional<double> height_zoom;
            if (!IsZero(max_lat_ - min_lat)) {
                height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
            }

            if (width_zoom && height_zoom) {
                // ������������ ��������������� �� ������ � ������ ���������,
                // ���� ����������� �� ���
                zoom_coeff_ = std::min(*width_zoom, *height_zoom);
            }
            else if (width_zoom) {
                // ����������� ��������������� �� ������ ���������, ���������� ���
                zoom_coeff_ = *width_zoom;
            }
            else if (height_zoom) {
                // ����������� ��������������� �� ������ ���������, ���������� ���
                zoom_coeff_ = *height_zoom;
            }           
        }

        // ���������� ������ � ������� � ���������� ������ SVG-�����������
        svg::Point operator()(geo::Coordinates coords) const {
            return {
                (coords.lng - min_lon_) * zoom_coeff_ + padding_,
                (max_lat_ - coords.lat) * zoom_coeff_ + padding_
            };
        }

    private:
        double padding_;
        double min_lon_ = 0;
        double max_lat_ = 0;
        double zoom_coeff_ = 0;
    };
 
    class MapRenderer
    {        
    public:
        MapRenderer() = default;
        svg::Document RenderMap(const catalogue::BUSES& all_buses, const catalogue::STOPS& all_stops) const;
        void PutRenderSettings(DrawSettings& rend_sets);        
        ~MapRenderer();
    private:
        void AddRouteToMap(svg::Document& map_, std::vector<svg::Point>& points, size_t curr_col) const;
        void AddRouteNameToMap(svg::Document& map_, std::string name, size_t color, svg::Point coor) const;
        void AddStopToMap(svg::Document& map_, svg::Point coor) const;
        void AddStopNameToMap(svg::Document& map_, std::string name, svg::Point coor) const;
        DrawSettings draw_sets_;      
    };    
}
