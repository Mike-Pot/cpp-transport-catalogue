#include "json.h"

namespace json {

    namespace loader {

        Node LoadNode(std::istream& input);
        void SkipSpaceSymbs(std::istream& input)
        {
            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            while (it != end && (*it == ' ' || *it == '\n' || *it == '\t'))
            {
                it++;
            }
        }

        Node LoadArray(std::istream& input) {
            Array result;
            char c; 

            while (true)
            {
                if (!(input >> c))
                {
                    throw ParsingError("Error parsing array");
                }
                if (c == ']')
                {
                    break;
                }
                if (c == ',')
                {
                    SkipSpaceSymbs(input);
                }
                else
                {
                    input.putback(c);
                    result.push_back(LoadNode(input));
                }
            }
            return Node(std::move(result));
        }

        using Number = std::variant<int, double>;

        Number LoadNumber(std::istream& input) {
            using namespace std::literals;

            std::string parsed_num;

            // Считывает в parsed_num очередной символ из input
            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
            };

            // Считывает одну или более цифр в parsed_num из input
            auto read_digits = [&input, read_char] {
                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek())) {
                    read_char();
                }
            };

            if (input.peek() == '-') {
                read_char();
            }
            // Парсим целую часть числа
            if (input.peek() == '0') {
                read_char();
                // После 0 в JSON не могут идти другие цифры
            }
            else {
                read_digits();
            }

            bool is_int = true;
            // Парсим дробную часть числа
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            // Парсим экспоненциальную часть числа
            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }

            try {
                if (is_int) {
                    // Сначала пробуем преобразовать строку в int
                    try {
                        return std::stoi(parsed_num);
                    }
                    catch (...) {
                        // В случае неудачи, например, при переполнении,
                        // код ниже попробует преобразовать строку в double
                    }
                }
                return std::stod(parsed_num);
            }
            catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        std::string LoadString(std::istream& input) {
            using namespace std::literals;

            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end) {
                    // Поток закончился до того, как встретили закрывающую кавычку?
                    throw ParsingError("String parsing error");
                }
                const char ch = *it;
                if (ch == '"') {
                    // Встретили закрывающую кавычку
                    ++it;
                    break;
                }
                else if (ch == '\\') {
                    // Встретили начало escape-последовательности
                    ++it;
                    if (it == end) {
                        // Поток завершился сразу после символа обратной косой черты
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
                    switch (escaped_char) {
                    case 'n':
                        s.push_back('\n');
                        break;
                    case 't':
                        s.push_back('\t');
                        break;
                    case 'r':
                        s.push_back('\r');
                        break;
                    case '"':
                        s.push_back('"');
                        break;
                    case '\\':
                        s.push_back('\\');
                        break;
                    default:
                        // Встретили неизвестную escape-последовательность
                        throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                }
                else if (ch == '\n' || ch == '\r') {
                    // Строковый литерал внутри- JSON не может прерываться символами \r или \n
                    throw ParsingError("Unexpected end of line"s);
                }
                else {
                    // Просто считываем очередной символ и помещаем его в результирующую строку
                    s.push_back(ch);
                }
                ++it;
            }

            return s;
        }

        bool LoadTrue(std::istream& input)
        {
            std::string s;

            for (int i = 0; i < 3; i++)
            {
                s += input.get();
            }
            char next = input.peek();
            if ((s != "rue") ||
                (next != EOF && next != ' ' && next != ',' && next != ']' &&
                    next != '}' && next != '\n' && next != '\t'))
            {
                throw ParsingError("Parsing error, true possible");
            }
            return true;
        }

        bool LoadFalse(std::istream& input)
        {
            std::string s;

            for (int i = 0; i < 4; i++)
            {
                s += input.get();
            }
            char next = input.peek();
            if ((s != "alse") ||
                (next != EOF && next != ' ' && next != ',' && next != ']' &&
                    next != '}' && next != '\n' && next != '\t'))
            {
                throw ParsingError("Invalid input, false possible");
            }
            return false;
        }

        nullptr_t LoadNull(std::istream& input)
        {
            std::string s;

            for (int i = 0; i < 3; i++)
            {
                s += input.get();
            }
            char next = input.peek();
            if ((s != "ull") ||
                (next != EOF && next != ' ' && next != ',' && next != ']' &&
                    next != '}' && next != '\n' && next != '\t'))
            {
                throw ParsingError("Parsing error, null possible");
            }
            return {};
        }

        Node LoadDict(std::istream& input)
        {
            Dict result;
            char c; 

            while (true)
            {
                if (!(input >> c))
                {
                    throw ParsingError("Error parsing dictionary");
                }
                if (c == '}')
                {
                    break;
                }
                if (c == ',') {
                    input >> c;
                    SkipSpaceSymbs(input);
                }
                std::string key = LoadString(input);
                input >> c;
                SkipSpaceSymbs(input);
                result.insert({ std::move(key), LoadNode(input) });
            }
            return Node(std::move(result));
        }

        Node LoadNode(std::istream& input)
        {            
            char c;
            input >> c;

            if (c == '[') {
                SkipSpaceSymbs(input);
                return LoadArray(input);
            }
            else if (c == '{') {
                SkipSpaceSymbs(input);
                return LoadDict(input);
            }
            else if (c == '"') {
                return Node(LoadString(input));
            }
            else if (c == 't') {
                return Node(LoadTrue(input));
            }
            else if (c == 'f') {
                return Node(LoadFalse(input));
            }
            else if (c == 'n') {
                return Node(LoadNull(input));
            }
            else {
                input.putback(c);
                Number n = LoadNumber(input);
                if (std::holds_alternative<int>(n))
                {
                    return Node(get<int>(n));
                }
                else
                {
                    return Node(get<double>(n));
                }
            }
        }
    }  // namespace Loader

    const Array& Node::AsArray() const {
        return GetValRef<Array>();
    }

    const Dict& Node::AsMap() const {
        return GetValRef<Dict>();
    }

    int Node::AsInt() const {
        return GetVal<int>();
    }

    const std::string& Node::AsString() const {
        return GetValRef<std::string>();
    }
    bool Node::AsBool() const {
        return GetVal<bool>();
    }

    double Node::AsDouble() const {
        if (IsInt())
        {
            return GetVal<int>();
        }
        else
        {
            return GetVal<double>();
        }
    }

    bool Node::IsArray() const {
        return std::holds_alternative<Array>(val_);
    }

    bool Node::IsMap() const {
        return std::holds_alternative<Dict>(val_);
    }

    bool Node::IsInt() const {
        return std::holds_alternative<int>(val_);
    }

    bool Node::IsString() const {
        return std::holds_alternative<std::string>(val_);
    }

    bool Node::IsPureDouble() const {
        return std::holds_alternative<double>(val_);
    }

    bool Node::IsDouble() const {
        return (IsInt() || IsPureDouble());
    }

    bool Node::IsBool() const {
        return std::holds_alternative<bool>(val_);
    }

    bool Node::IsNull() const {
        return std::holds_alternative<std::nullptr_t>(val_);
    }

    void Node::PrintVal(std::ostream& out, int val) const
    {
        out << val;
    }

    void Node::PrintVal(std::ostream& out, double val) const
    {
        out << val;
    }

    void Node::PrintVal(std::ostream& out, bool val) const
    {
        if (val)
        {
            out << "true";
        }
        else
        {
            out << "false";
        }
    }

    void Node::PrintVal(std::ostream& out, const std::string& val) const
    {
        std::string s = "\"";
        for (size_t i = 0; i < val.size(); i++)
        {
            switch (val[i])
            {
            case '\\':
                s += "\\\\";
                break;
            case '\n':
                s += "\\n";
                break;
            case '\t':
                s += "\\t";
                break;
            case '\r':
                s += "\\r";
                break;
            case '"':
                s += "\\\"";
                break;
            default:
                s += val[i];
                break;
            }
        }
        s += "\"";
        out << s;
    }

    void Node::PrintVal(std::ostream& out, const Dict& val) const
    {
        if (val.empty())
        {
            out << "{}";
            return;
        }
        out << "{";
        auto last = --val.end();
        for (auto it = val.begin(); it != last; it++)
        {
            out << "\"" << it->first << "\": ";
            it->second.PrintNode(out);
            out << ", ";
        }
        out << "\"" << last->first << "\": ";
        last->second.PrintNode(out);
        out << "}";
    }

    void Node::PrintVal(std::ostream& out, const Array& val) const
    {
        if (val.empty())
        {
            out << "[]";
            return;
        }
        out << "[";
        for (auto it = val.begin(); it != val.end() - 1; it++)
        {
            it->PrintNode(out);
            out << ", ";
        }
        (val.end() - 1)->PrintNode(out);
        out << "]";
    }

    void Node::PrintVal(std::ostream& out, [[maybe_unused]] nullptr_t val) const
    {
        out << "null";
    }

    void Node::PrintNode(std::ostream& out) const
    {
        visit([&out, *this](auto& val) { PrintVal(out, val); }, val_);
    }

    Document::Document(Node root)
        : root_(std::move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    Document Load(std::istream& input) {
        return Document{ loader::LoadNode(input) };
    }

    void Print(const Document& doc, std::ostream& output) {
        doc.GetRoot().PrintNode(output);        
    }

}  // namespace json