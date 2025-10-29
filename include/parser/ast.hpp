#pragma once
#include "../lexer/token.hpp"
#include <cmath>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

enum class TypeValue {
    I8, I16, I32, I64, F32, F64, U8, U16, U32, U64, BOOL, STRING, NOTHING, CLASS, ENUM
};

struct Type {
    TypeValue type;
    std::string name;
    bool is_const;
    bool is_unsigned;
    bool is_pointer;

    Type(TypeValue t, std::string n, bool is_c = false, bool is_u = false, bool is_p = false)
       : type(t), name(n), is_const(is_c), is_unsigned(is_u), is_pointer(is_p) {}
    
    bool operator ==(Type& other) const {
        return this->type == other.type && this->name == other.name && this->is_const == other.is_const
            && this->is_unsigned == other.is_unsigned && this->is_pointer == other.is_pointer;
    }

    bool operator !=(Type& other) const {
        return !(*this == other);
    }
};

struct Value {
    std::variant<std::int8_t, std::int16_t, std::int32_t, std::int64_t, std::float_t, std::double_t, std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t,
                 bool, std::string> value;

    Value(std::int8_t v)        : value(v) {}
    Value(std::int16_t v)       : value(v) {}
    Value(std::int32_t v)       : value(v) {}
    Value(std::int64_t v)       : value(v) {}
    Value(std::float_t v)       : value(v) {}
    Value(std::double_t v)      : value(v) {}
    Value(std::uint8_t v)       : value(v) {}
    Value(std::uint16_t v)      : value(v) {}
    Value(std::uint32_t v)      : value(v) {}
    Value(std::uint64_t v)      : value(v) {}
    Value(bool v)               : value(v) {}
    Value(std::string v)        : value(v) {}
};

class Expr {
public:
    virtual ~Expr() = default;
};

class Stmt {
public:
    virtual ~Stmt() = default;
};

using ExprPtr = std::unique_ptr<Expr>;
using StmtPtr = std::unique_ptr<Stmt>;

class Literal : public Expr {
public:
    Value value;
    Type type;

    Literal(Value v, Type t) : value(v), type(t) {}
    virtual ~Literal() = default;
};

class I8Literal : public Literal {
public:
    I8Literal(std::int8_t v) : Literal(Value(v), Type(TypeValue::I8, "i8")) {}
    ~I8Literal() override = default;
};

class I16Literal : public Literal {
public:
    I16Literal(std::int16_t v) : Literal(Value(v), Type(TypeValue::I16, "i16")) {}
    ~I16Literal() override = default;
};

class I32Literal : public Literal {
public:
    I32Literal(std::int32_t v) : Literal(Value(v), Type(TypeValue::I32, "i32")) {}
    ~I32Literal() override = default;
};

class I64Literal : public Literal {
public:
    I64Literal(std::int64_t v) : Literal(Value(v), Type(TypeValue::I64, "i64")) {}
    ~I64Literal() override = default;
};

class F32Literal : public Literal {
public:
    F32Literal(std::float_t v) : Literal(Value(v), Type(TypeValue::F32, "f32")) {}
    ~F32Literal() override = default;
};

class F64Literal : public Literal {
public:
    F64Literal(std::double_t v) : Literal(Value(v), Type(TypeValue::F64, "f64")) {}
    ~F64Literal() override = default;
};

class U8Literal : public Literal {
public:
    U8Literal(std::uint8_t v) : Literal(Value(v), Type(TypeValue::U8, "u8")) {}
    ~U8Literal() override = default;
};

class U16Literal : public Literal {
public:
    U16Literal(std::uint16_t v) : Literal(Value(v), Type(TypeValue::U16, "u16")) {}
    ~U16Literal() override = default;
};

class U32Literal : public Literal {
public:
    U32Literal(std::uint32_t v) : Literal(Value(v), Type(TypeValue::U32, "u32")) {}
    ~U32Literal() override = default;
};

class U64Literal : public Literal {
public:
    U64Literal(std::uint64_t v) : Literal(Value(v), Type(TypeValue::U64, "u64")) {}
    ~U64Literal() override = default;
};

class BoolLiteral : public Literal {
public:
    BoolLiteral(bool v) : Literal(Value(v), Type(TypeValue::BOOL, "bool")) {}
    ~BoolLiteral() override = default;
};

class StringLiteral : public Literal {
public:
    StringLiteral(std::string v) : Literal(Value(v), Type(TypeValue::STRING, "string")) {}
    ~StringLiteral() override = default;
};

class BinaryExpr : public Expr {
public:
    TokenType op_type;
    ExprPtr left;
    ExprPtr right;

    BinaryExpr(TokenType o, ExprPtr l, ExprPtr r) : op_type(o), left(std::move(l)), right(std::move(r)) {}
    ~BinaryExpr() override = default;
};

class UnaryExpr : public Expr {
public:
    TokenType op_type;
    ExprPtr expr;

    UnaryExpr(TokenType o, ExprPtr e) : op_type(o), expr(std::move(e)) {}
    ~UnaryExpr() override = default;
};

class VarExpr : public Expr {
public:
    std::string name;

    VarExpr(std::string n) : name(n) {}
    ~VarExpr() override = default;
};

class FuncCallExpr : public Expr {
public:
    std::string name;
    std::vector<ExprPtr> args;

    FuncCallExpr(std::string n, std::vector<ExprPtr> a) : name(n), args(std::move(a)) {}
};

class VarDeclStmt : public Stmt {
public:
    Type type;
    std::string name;
    ExprPtr expr;

    VarDeclStmt(Type t, std::string n, ExprPtr e) : type(t), name(n), expr(std::move(e)) {}
    ~VarDeclStmt() override = default;
};

struct Argument {
    Type type;
    std::string name;
    ExprPtr expr;

    Argument(Type t, std::string n, ExprPtr e) : type(t), name(n), expr(std::move(e)) {}
};

class FuncDeclStmt : public Stmt {
public:
    Type type;
    std::string name;
    std::vector<Argument> args;
    std::vector<StmtPtr> block;

    FuncDeclStmt(Type t, std::string n, std::vector<Argument> a, std::vector<StmtPtr> b) : type(t), name(n), args(std::move(a)), block(std::move(b)) {}
    ~FuncDeclStmt() override = default;
};

class FuncCallStmt : public Stmt {
public:
    std::string name;
    std::vector<ExprPtr> args;

    FuncCallStmt(std::string n, std::vector<ExprPtr> a) : name(n), args(std::move(a)) {}
    ~FuncCallStmt() override = default;
};

class VarAsgnStmt : public Stmt {
public:
    std::string name;
    ExprPtr expr;

    VarAsgnStmt(std::string n, ExprPtr e) : name(n), expr(std::move(e)) {}
    ~VarAsgnStmt() override = default;
};

class IfStmt : public Stmt {
public:
    ExprPtr condition;
    std::vector<StmtPtr> true_block;
    std::vector<StmtPtr> false_block;

    IfStmt(ExprPtr c, std::vector<StmtPtr> t, std::vector<StmtPtr> f) : condition(std::move(c)), true_block(std::move(t)), false_block(std::move(f)) {}
    ~IfStmt() override = default;
};

class ForCycleStmt : public Stmt {
public:
    StmtPtr indexator;
    ExprPtr condition;
    StmtPtr iteration;
    std::vector<StmtPtr> block;

    ForCycleStmt(StmtPtr ir, ExprPtr c, StmtPtr it, std::vector<StmtPtr> b) : indexator(std::move(ir)), condition(std::move(c)), iteration(std::move(it)),
                                                                              block(std::move(b)) {}
    ~ForCycleStmt() override = default;
};

class BreakStmt : public Stmt {
public:
    BreakStmt() = default;
    ~BreakStmt() override = default;
};

class ContinueStmt : public Stmt {
public:
    ContinueStmt() = default;
    ~ContinueStmt() override = default;
};

class ReturnStmt : public Stmt {
public:
    ExprPtr expr;

    ReturnStmt(ExprPtr e) : expr(std::move(e)) {}
    ~ReturnStmt() override = default;
};