#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <variant>
#include <iomanip>

namespace svg {
    struct Rgb
    {
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
    };

    struct Rgba
    {
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
        double opacity=1.0;
    };

    using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;
    inline  Color NoneColor = {};

    enum class StrokeLineCap {
        BUTT,
        ROUND,
        SQUARE,
    };

    enum class StrokeLineJoin {
        ARCS,
        BEVEL,
        MITER,
        MITER_CLIP,
        ROUND,
    };

    struct Point {
        Point() = default;
        Point(double x, double y)
            : x(x)
            , y(y) {
        }
        double x = 0;
        double y = 0;
    };

    std::ostream& operator<<(std::ostream& out, StrokeLineCap to_out);
    std::ostream& operator<<(std::ostream& out, StrokeLineJoin to_out);
    std::ostream& operator<<(std::ostream& out, Color to_out);
    /*
     * Вспомогательная структура, хранящая контекст для вывода SVG-документа с отступами.
     * Хранит ссылку на поток вывода, текущее значение и шаг отступа при выводе элемента
     */
    struct RenderContext {
        RenderContext(std::ostream& out)
            : out(out) {
        }

        RenderContext(std::ostream& out, int indent_step, int indent = 0)
            : out(out)
            , indent_step(indent_step)
            , indent(indent) {
        }

        RenderContext Indented() const {
            return { out, indent_step, indent + indent_step };
        }

        void RenderIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        std::ostream& out;
        int indent_step = 0;
        int indent = 0;
    };
    
    template <typename Owner>
    class PathProps {
    public:
        Owner& SetFillColor(Color fill_color)
        {
            fill_col_ = fill_color;
            return GetOwner();
        }

        Owner& SetStrokeColor(Color stroke_color)
        {
            stroke_col_ = stroke_color;
            return GetOwner();
        }

        Owner& SetStrokeWidth(double width)
        {
            stroke_width_ = width;
            return GetOwner();
        }

        Owner& SetStrokeLineCap(StrokeLineCap arg)
        {
            stroke_line_cap_ = arg;
            return GetOwner();
        }

        Owner& SetStrokeLineJoin(StrokeLineJoin arg)
        {
            stroke_line_join_ = arg;
            return GetOwner();
        }

    protected:
        ~PathProps() = default;

        void RenderAttrs(std::ostream& out) const {
            using namespace std::literals;

            if (fill_col_) {
                out << " fill=\""sv << *fill_col_ << "\""sv;
            }

            if (stroke_col_) {
                out << " stroke=\""sv << *stroke_col_ << "\""sv;
            }

            if (stroke_width_) {
                out << " stroke-width=\""sv << stroke_width_ << "\""sv;
            }

            if (stroke_line_cap_) {
                out << " stroke-linecap=\""sv << *stroke_line_cap_ << "\""sv;
            }

            if (stroke_line_join_) {
                out << " stroke-linejoin=\""sv << *stroke_line_join_ << "\""sv;
            }
        }
    private:
        std::optional <Color> fill_col_;
        std::optional <Color> stroke_col_;
        double stroke_width_ = 0;
        std::optional<StrokeLineCap> stroke_line_cap_;
        std::optional<StrokeLineJoin> stroke_line_join_;

        Owner& GetOwner()
        {
            return (Owner&)*this;
        }
    };

    class Object {
    public:
        void Render(const RenderContext& context) const;
        virtual ~Object() = default;

    private:
        virtual void RenderObject(const RenderContext& context) const = 0;
    };

    class ObjectContainer
    {
    public:
        template<typename T>
        void Add(T obj)
        {
            AddPtr(std::make_unique<T>(std::move(obj)));
        }

        virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;

    protected:
        ~ObjectContainer() = default;
    };

    class Drawable
    {
    public:
        virtual void Draw(ObjectContainer& container) const = 0;
        virtual ~Drawable() = default;
    };
    /*
     * Класс Circle моделирует элемент <circle> для отображения круга
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/circle
     */
    class Circle final : public Object, public PathProps<Circle> {
    public:
        Circle& SetCenter(Point center);
        Circle& SetRadius(double radius);

    private:
        void RenderObject(const RenderContext& context) const override;

        Point center_;
        double radius_ = 1.0;
    };
    /*
     * Класс Polyline моделирует элемент <polyline> для отображения ломаных линий
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/polyline
     */
    class Polyline final : public Object, public PathProps<Polyline> {
    public:
        // Добавляет очередную вершину к ломаной линии
        Polyline& AddPoint(Point point);

    private:
        void RenderObject(const RenderContext& context) const;
        std::vector<Point> line_points_;
    };
    /*
     * Класс Text моделирует элемент <text> для отображения текста
     * https://developer.mozilla.org/en-US/docs/Web/SVG/Element/text
     */
    class Text final : public Object, public PathProps<Text> {
    public:
        // Задаёт координаты опорной точки (атрибуты x и y)
        Text& SetPosition(Point pos);
        // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
        Text& SetOffset(Point offset);
        // Задаёт размеры шрифта (атрибут font-size)
        Text& SetFontSize(uint32_t size);
        // Задаёт название шрифта (атрибут font-family)
        Text& SetFontFamily(std::string font_family);
        // Задаёт толщину шрифта (атрибут font-weight)
        Text& SetFontWeight(std::string font_weight);
        // Задаёт текстовое содержимое объекта (отображается внутри тега text)
        Text& SetData(std::string data);

    private:
        void RenderObject(const RenderContext& context) const;
        void ReplaceSymbols(std::string& txt) const;

        Point pos_, offset_;
        uint32_t font_size_ = 1;
        std::string font_family_ = "";
        std::string font_weight_ = "";
        std::string data_ = "";
    };

    class Document : public ObjectContainer {
    public:
        void AddPtr(std::unique_ptr<Object>&& obj) override;
        // Выводит в ostream svg-представление документа
        void Render(std::ostream& out) const;

    private:
        std::vector<std::unique_ptr<Object>> figures_;
    };
}  // namespace svg

