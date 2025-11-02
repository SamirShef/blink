#include "../../include/parser/ast.hpp"
#include <stack>
#include <map>

class SemanticAnalyzer {
private:
    std::vector<StmtPtr>& stmts;
    std::stack<std::map<std::string, Type>> variables;
    unsigned blocks_deep;
    unsigned loops_blocks_deep;
    
    struct FunctionInfo {
        Type return_type;
        std::vector<Argument> args;

        FunctionInfo(Type rt, std::vector<Argument> a) : return_type(rt), args(std::move(a)) {}
    };
    std::map<std::string, FunctionInfo> functions;
    std::stack<Type> functions_types_stack;

public:
    SemanticAnalyzer(std::vector<StmtPtr>& s) : stmts(s), blocks_deep(0) {
        variables.push({});
    }

    void analyze();

private:
    void analyze_stmt(Stmt& stmt);
    void analyze_var_decl_stmt(VarDeclStmt& vds);
    void analyze_func_decl_stmt(FuncDeclStmt& fds);
    void analyze_func_call_stmt(FuncCallStmt& fcs);
    void analyze_var_asgn_stmt(VarAsgnStmt& vas);
    void analyze_if_stmt(IfStmt& is);
    void analyze_for_cycle_stmt(ForCycleStmt& fcs);
    void analyze_while_cycle_stmt(WhileCycleStmt& wcs);
    void analyze_do_while_cycle_stmt(DoWhileCycleStmt& wcs);
    void analyze_break_stmt(BreakStmt& bs);
    void analyze_continue_stmt(ContinueStmt& cs);
    void analyze_return_stmt(ReturnStmt& rs);

    Type analyze_expr(const Expr& expr);
    Type analyze_literal(const Literal& lit);
    Type analyze_binary_expr(const BinaryExpr& be);
    Type analyze_unary_expr(const UnaryExpr& ue);
    Type analyze_var_expr(const VarExpr& ve);
    Type analyze_func_call_expr(const FuncCallExpr& fce);

    Type get_common_type(Type left_type, Type right_type, Token first_token);
    std::string type_to_string(Type type);
};