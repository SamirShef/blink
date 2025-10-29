#include "../../include/semantic/semantic.hpp"

void SemanticAnalyzer::analyze() {
    for (const StmtPtr& stmt : stmts) {
        analyze_stmt(*stmt);
    }
}

void SemanticAnalyzer::analyze_stmt(const Stmt& stmt) {
    if (auto vds = dynamic_cast<const VarDeclStmt*>(&stmt)) {
        analyze_var_decl_stmt(*vds);
    }
}

void SemanticAnalyzer::analyze_var_decl_stmt(const VarDeclStmt& vds) {

}

Type SemanticAnalyzer::analyze_expr(const Expr& expr) {
    if (auto lit = dynamic_cast<const Literal*>(&expr)) {
        return analyze_literal(*lit);
    }
    else if (auto be = dynamic_cast<const BinaryExpr*>(&expr)) {
        return analyze_binary_expr(*be);
    }
    else if (auto ue = dynamic_cast<const UnaryExpr*>(&expr)) {
        return analyze_unary_expr(*ue);
    }
    else if (auto ve = dynamic_cast<const VarExpr*>(&expr)) {
        return analyze_var_expr(*ve);
    }
    else if (auto fce = dynamic_cast<const FuncCallExpr*>(&expr)) {
        return analyze_func_call_expr(*fce);
    }
}

Type SemanticAnalyzer::analyze_literal(const Literal& lit) {
    return lit.type;
}

Type SemanticAnalyzer::analyze_binary_expr(const BinaryExpr& be) {
    Type left_type = analyze_expr(*be.left);
    Type right_type = analyze_expr(*be.right);

    return get_common_type(left_type, right_type);
}

Type SemanticAnalyzer::analyze_unary_expr(const UnaryExpr& ue) {
    return analyze_expr(*ue.expr);
}

Type SemanticAnalyzer::analyze_var_expr(const VarExpr& ve) {

}

Type SemanticAnalyzer::analyze_func_call_expr(const FuncCallExpr& fce) {

}

Type SemanticAnalyzer::get_common_type(Type left_type, Type right_type) {
    if (left_type == right_type) {
        return left_type;
    }

    
}