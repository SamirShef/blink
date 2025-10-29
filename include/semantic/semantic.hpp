#include "../../include/parser/ast.hpp"
#include <vector>

class SemanticAnalyzer {
private:
    std::vector<StmtPtr> stmts;

public:
    SemanticAnalyzer(std::vector<StmtPtr> s) : stmts(std::move(s)) {}

    void analyze();

private:
    void analyze_stmt(const Stmt& stmt);
    void analyze_var_decl_stmt(const VarDeclStmt& vds);

    Type analyze_expr(const Expr& expr);
    Type analyze_literal(const Literal& lit);
    Type analyze_binary_expr(const BinaryExpr& be);
    Type analyze_unary_expr(const UnaryExpr& ue);
    Type analyze_var_expr(const VarExpr& ve);
    Type analyze_func_call_expr(const FuncCallExpr& fce);

    Type get_common_type(Type left_type, Type right_type);
};