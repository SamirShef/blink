#include <string>
#include <variant>

enum class TypeValue {
    BUILTIN, STRUCT, ENUM
};

enum class TypeSpecifier {
    NONE, LONG, SHORT
};

struct Type {
    TypeValue type;
    std::string name;
    TypeSpecifier specifier;
    bool is_const;
    bool is_unsigned;
    bool is_pointer;

    Type(TypeValue t, std::string n, TypeSpecifier s = TypeSpecifier::NONE, bool is_c = false, bool is_u = false, bool is_p = false)
       : type(t), name(n), specifier(s), is_const(is_c), is_unsigned(is_u), is_pointer(is_p) {}
    
    bool operator ==(Type& other) const {
        return this->type == other.type && this->name == other.name && this->specifier == other.specifier && this->is_const == other.is_const
            && this->is_unsigned == other.is_unsigned && this->is_pointer == other.is_pointer;
    }

    bool operator !=(Type& other) const {
        return !(*this == other);
    }
};

struct Value {
    std::variant<char, short, int, long, float, double> value;

    Value(char v)       : value(v) {}
    Value(short v)      : value(v) {}
    Value(int v)        : value(v) {}
    Value(long v)       : value(v) {}
    Value(float v)      : value(v) {}
    Value(double v)     : value(v) {}
};

class Expr {
public:
    virtual ~Expr() = default;
};

class Stmt {
public:
    virtual ~Stmt() = default;
};

class Literal : public Expr {
public:
    Value value;
    Type type;

    Literal(Value v, Type t) : value(v), type(t) {}
    virtual ~Literal() = default;
};

class CharLiteral : public Literal {
public:
    CharLiteral(char v) : Literal(Value(v), Type(TypeValue::BUILTIN, "char")) {}
    ~CharLiteral() override = default;
};

class ShortLiteral : public Literal {
public:
    ShortLiteral(short v) : Literal(Value(v), Type(TypeValue::BUILTIN, "short")) {}
    ~ShortLiteral() override = default;
};

class IntLiteral : public Literal {
public:
    IntLiteral(int v) : Literal(Value(v), Type(TypeValue::BUILTIN, "int")) {}
    ~IntLiteral() override = default;
};

class LongLiteral : public Literal {
public:
    LongLiteral(long v) : Literal(Value(v), Type(TypeValue::BUILTIN, "long")) {}
    ~LongLiteral() override = default;
};

class FloatLiteral : public Literal {
public:
    FloatLiteral(float v) : Literal(Value(v), Type(TypeValue::BUILTIN, "float")) {}
    ~FloatLiteral() override = default;
};

class DoubleLiteral : public Literal {
public:
    DoubleLiteral(double v) : Literal(Value(v), Type(TypeValue::BUILTIN, "double")) {}
    ~DoubleLiteral() override = default;
};