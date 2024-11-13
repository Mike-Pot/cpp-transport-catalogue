#pragma once

#include <variant>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <stdexcept>




namespace json {

    class Node;
    // —охраните объ€влени€ Dict и Array без изменени€
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;


    // Ёта ошибка должна выбрасыватьс€ при ошибках парсинга JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node {
    public:
        using Value = std::variant<std::nullptr_t, Array, Dict, int, std::string, double, bool>;
       
        Node() = default;

        Node(std::nullptr_t)
            : val_(nullptr) {
        }

        Node(std::string val)
            : val_(std::move(val)) {
        }
        Node(Array val)
            : val_(std::move(val)) {
        }

        Node(int val)
            : val_(val) {
        }
        Node(double val)
            : val_(val) {
        }
        Node(bool val)
            : val_(val) {
        }

        Node(Dict val)
            : val_(std::move(val)) {
        }

        bool operator==(const Node& rhs) const
        {
            return val_ == rhs.val_;
        }

        bool operator!=(const Node& rhs) const
        {
            return val_ != rhs.val_;
        }
       
        const Array& AsArray() const;
        const Dict& AsMap() const;
        int AsInt() const;
        const std::string& AsString() const;
        bool AsBool() const;
        double AsDouble() const;

        bool IsArray() const;
        bool IsMap() const;
        bool IsInt() const;
        bool IsString() const;
        bool IsDouble() const;
        bool IsPureDouble() const;
        bool IsBool() const;
        bool IsNull() const;

        void PrintNode(std::ostream& out) const;

    private:       
        template<typename T>
        T GetVal() const
        {            
            if (std::holds_alternative<T>(val_))
            {
                return std::get<T>(val_);                
            }
            else
            {
                throw std::logic_error("Wrong value type");
            }            
        }

        template<typename T>
        const T& GetValRef() const
        {
            if (std::holds_alternative<T>(val_))
            {
                return std::get<T>(val_);                
            }
            else
            {
                throw std::logic_error("Wrong value type");
            }            
        }

        void PrintVal(std::ostream& out, int val) const;
        void PrintVal(std::ostream& out, double val) const;
        void PrintVal(std::ostream& out, [[maybe_unused]] std::nullptr_t val) const;
        void PrintVal(std::ostream& out, bool val) const;
        void PrintVal(std::ostream& out, const Array& val) const;
        void PrintVal(std::ostream& out, const Dict& val) const;
        void PrintVal(std::ostream& out, const std::string& val) const;

        Value val_;
    };

    class Document {
    public:
        explicit Document(Node root);
        const Node& GetRoot() const;

        bool operator==(const Document& rhs) const
        {
            return root_ == rhs.root_;
        }

        bool operator!=(const Document& rhs) const
        {
            return root_ != rhs.root_;
        }

    private:
        Node root_;
    };

    Document Load(std::istream& input);
    void Print(const Document& doc, std::ostream& output);

}  // namespace json
