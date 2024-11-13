
#include "svg.h"

namespace svg {

    using namespace std::literals;

    std::ostream& operator<<(std::ostream& out, StrokeLineCap to_out)
    {       
        switch (to_out)
        {
        case StrokeLineCap::BUTT:
            out << "butt";
            break;
        case StrokeLineCap::ROUND:
            out << "round";
            break;
        case StrokeLineCap::SQUARE:
            out << "square";
            break;
        default:
            break;
        }
        return out;
    }

    std::ostream& operator<<(std::ostream& out, StrokeLineJoin to_out)
    {
        switch (to_out)
        {
        case StrokeLineJoin::ARCS:
            out << "arcs";
            break;
        case StrokeLineJoin::BEVEL:
            out << "bevel";
            break;
        case StrokeLineJoin::MITER:
            out << "miter";
            break;
        case StrokeLineJoin::MITER_CLIP:
            out << "miter-clip";
            break;
        case StrokeLineJoin::ROUND:
            out << "round";
            break;
        default:
            break;
        }
        return out;
    }

    std::ostream& operator<<(std::ostream& out, Color to_out)
    {
        struct Renderer
        {
            std::ostream& out_;
            void operator()(std::monostate) const
            {
                out_ << "none";
            }
            void operator()(std::string col) const
            {
                out_ << col;
            }
            void operator()(Rgb col) const
            {
                out_ << "rgb(" << (int)col.red << "," << (int)col.green << "," << (int)col.blue << ")";
            }
            void operator()(Rgba col) const
            {
                out_ << "rgba(" << (int)col.red << "," << (int)col.green << "," << (int)col.blue
                    << "," << col.opacity << ")";
            }
        };
        std::visit(Renderer{ out }, to_out);
        return out;
    }

    void Object::Render(const RenderContext& context) const
    {
        context.RenderIndent();
        // Делегируем вывод тега своим подклассам
        RenderObject(context);
        context.out << std::endl;
    }

    Circle& Circle::SetCenter(Point center) {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius) {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\" "sv;
        RenderAttrs(context.out);
        out << "/>"sv;
    }

    void Polyline::RenderObject(const RenderContext& context) const
    {
        auto& out = context.out;
        out << "<polyline points=\""sv;
        for (size_t i = 0; i < line_points_.size(); i++)
        {
            out << line_points_[i].x << ',' << line_points_[i].y;
            if (i < line_points_.size() - 1)
            {
                out << ' ';
            }
        }
        out << "\""sv;
        RenderAttrs(context.out);
        out << "/>"sv;
    }

    Polyline& Polyline::AddPoint(Point point)
    {
        line_points_.push_back(point);
        return *this;
    }

    void Text::RenderObject(const RenderContext& context) const
    {
        auto& out = context.out;
        out << "<text x=\""sv;
        out << pos_.x << "\" y=\""sv << pos_.y;
        out << "\" dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y;
        out << "\" font-size=\""sv << font_size_ << "\""sv;
        if (!font_family_.empty())
        {
            out << " font-family=\""sv << font_family_ << "\""sv;
        }
        if (!font_weight_.empty())
        {
            out << " font-weight=\""sv << font_weight_ << "\""sv;
        }
        RenderAttrs(context.out);
        out << ">"sv;
        std::string txt = data_;
        ReplaceSymbols(txt);
        out << txt << "</text>"sv;
    }

    // Задаёт координаты опорной точки (атрибуты x и y)    
    Text& Text::SetPosition(Point pos)
    {
        pos_ = pos;
        return *this;
    }

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& Text::SetOffset(Point offset)
    {
        offset_ = offset;
        return *this;
    }

    // Задаёт размеры шрифта (атрибут font-size)
    Text& Text::SetFontSize(uint32_t size)
    {
        font_size_ = size;
        return *this;
    }

    // Задаёт название шрифта (атрибут font-family)
    Text& Text::SetFontFamily(std::string font_family)
    {
        font_family_ = font_family;
        return *this;
    }

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& Text::SetFontWeight(std::string font_weight)
    {
        font_weight_ = font_weight;
        return *this;
    }

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& Text::SetData(std::string data)
    {
        data_ = data;
        return *this;
    }

    void Text::ReplaceSymbols(std::string& txt) const
    {
        for (size_t i = 0; i < txt.size(); i++)
        {
            switch (txt[i])
            {
            case '\"':
                txt.erase(i, 1);
                txt.insert(i, "&quot;"sv);
                break;
            case '\'':
                txt.erase(i, 1);
                txt.insert(i, "&apos;"sv);
                break;
            case '<':
                txt.erase(i, 1);
                txt.insert(i, "&lt;"sv);
                break;
            case '>':
                txt.erase(i, 1);
                txt.insert(i, "&gt;"sv);
                break;
            case '&':
                txt.erase(i, 1);
                txt.insert(i, "&amp;"sv);
                break;
            default:
                break;
            }
        }
    }
    
    void Document::AddPtr(std::unique_ptr<Object>&& obj)
    {
        figures_.push_back(std::move(obj));
    }

    void Document::Render(std::ostream& out) const
    {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
        RenderContext cont = RenderContext(out, 1).Indented();
        for (auto& fig : figures_)
        {
            cont.RenderIndent();
            fig->Render(cont);
        }
        out << "</svg>"sv << std::endl;
    }
}  // namespace svg
