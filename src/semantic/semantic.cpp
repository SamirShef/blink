#include "../../include/exception/exception.hpp"
#include "../../include/semantic/semantic.hpp"

void SemanticAnalyzer::analyze() {
    for (const StmtPtr& stmt : stmts) {
        analyze_stmt(*stmt);
    }
}

void SemanticAnalyzer::analyze_stmt(Stmt& stmt) {
    if (auto vds = dynamic_cast<VarDeclStmt*>(&stmt)) {
        analyze_var_decl_stmt(*vds);
    }
    else if (auto fds = dynamic_cast<FuncDeclStmt*>(&stmt)) {
        analyze_func_decl_stmt(*fds);
    }
    else if (auto fcs = dynamic_cast<FuncCallStmt*>(&stmt)) {
        analyze_func_call_stmt(*fcs);
    }
    else if (auto vas = dynamic_cast<VarAsgnStmt*>(&stmt)) {
        analyze_var_asgn_stmt(*vas);
    }
    else if (auto is = dynamic_cast<IfStmt*>(&stmt)) {
        analyze_if_stmt(*is);
    }
    else if (auto fcs = dynamic_cast<ForCycleStmt*>(&stmt)) {
        analyze_for_cycle_stmt(*fcs);
    }
    else if (auto wcs = dynamic_cast<WhileCycleStmt*>(&stmt)) {
        analyze_while_cycle_stmt(*wcs);
    }
    else if (auto dwcs = dynamic_cast<DoWhileCycleStmt*>(&stmt)) {
        analyze_do_while_cycle_stmt(*dwcs);
    }
    else if (auto bs = dynamic_cast<BreakStmt*>(&stmt)) {
        analyze_break_stmt(*bs);
    }
    else if (auto cs = dynamic_cast<ContinueStmt*>(&stmt)) {
        analyze_continue_stmt(*cs);
    }
    else if (auto rs = dynamic_cast<ReturnStmt*>(&stmt)) {
        analyze_return_stmt(*rs);
    }
    else {
        throw_error(file_name, file_name_in_error_printed, SEMANTIC, "Unsupported statement\n", stmt.line);
    }
}

void SemanticAnalyzer::analyze_var_decl_stmt(VarDeclStmt& vds) {
    auto vars = variables;
    while (!vars.empty()) {
        auto var_it = vars.top().find(vds.name);
        if (var_it != vars.top().end()) {
            throw_error(file_name, file_name_in_error_printed, SEMANTIC, "Variable '" + vds.name + "' already exist\n", vds.line);
        }
        vars.pop();
    }
    
    if (vds.expr != nullptr) {
        Type expr_type = analyze_expr(*vds.expr);
        get_common_type(vds.type, expr_type, vds.line);
    }

    variables.top().emplace(vds.name, vds.type);
}

void SemanticAnalyzer::analyze_func_decl_stmt(FuncDeclStmt& fds) {
    auto func_it = functions.find(fds.name);
    if (func_it != functions.end()) {
        std::string joined_args;
        if (!fds.args.empty()) {
            joined_args.reserve();
            joined_args.append(type_to_string(func_it->second.args[0].type));
            unsigned args_size = func_it->second.args.size();
            for (unsigned i = 1; i < args_size; i++) {
                joined_args.append(", ");
                joined_args.append(type_to_string(fds.args[i].type));
            }
        }
        throw_error(file_name, file_name_in_error_printed, SEMANTIC, "Function '" + type_to_string(func_it->second.return_type) + ' ' + func_it->first + '(' + joined_args + ")' already exist\n", fds.line);
    }

    std::vector<Argument> args_copy = fds.args;
    
    variables.push({});
    functions_types_stack.push(fds.return_type);
    functions.emplace(fds.name, FunctionInfo(fds.return_type, std::move(args_copy)));
    for (const Argument& arg : fds.args) {
        variables.top().emplace(arg.name, arg.type);
    }
    for (const StmtPtr& stmt : fds.block) {
        analyze_stmt(*stmt);
    }
    functions_types_stack.pop();
    variables.pop();
}

void SemanticAnalyzer::analyze_func_call_stmt(FuncCallStmt& fcs) {
    if (fcs.name == "printf") {
        return;
    }

    auto func_it = functions.find(fcs.name);
    if (func_it == functions.end()) {
        std::string joined_args;
        if (!fcs.args.empty()) {
            joined_args.reserve();
            joined_args.append(type_to_string(analyze_expr(*fcs.args[0])));
            unsigned args_size = fcs.args.size();
            for (unsigned i = 1; i < args_size; i++) {
                joined_args.append(", ");
                joined_args.append(type_to_string(analyze_expr(*fcs.args[i])));
            }
        }
        throw_error(file_name, file_name_in_error_printed, SEMANTIC, "Function '" + fcs.name + '(' + joined_args + ")' does not exist\n", fcs.line);
    }

    unsigned args_size = fcs.args.size();
    for (unsigned i = 0; i < args_size; i++) {
        get_common_type(analyze_expr(*fcs.args[i]), func_it->second.args[i].type, fcs.line);
    }
}

void SemanticAnalyzer::analyze_var_asgn_stmt(VarAsgnStmt& vas) {
    Type var_type = analyze_var_expr(VarExpr(vas.name, vas.line));
    analyze_expr(*vas.expr);
}

void SemanticAnalyzer::analyze_if_stmt(IfStmt& is) {
    if (is.condition == nullptr) {
        throw_error(file_name, file_name_in_error_printed, SEMANTIC, "Conditional expression must not be null\n", is.line);
    }

    for (const StmtPtr& stmt : is.true_block) {
        analyze_stmt(*stmt);
    }
    for (const StmtPtr& stmt : is.false_block) {
        analyze_stmt(*stmt);
    }
}

void SemanticAnalyzer::analyze_for_cycle_stmt(ForCycleStmt& fcs) {
    analyze_stmt(*fcs.indexator);
    analyze_expr(*fcs.condition);
    analyze_stmt(*fcs.iteration);
    loops_blocks_deep++;

    for (const StmtPtr& stmt : fcs.block) {
        analyze_stmt(*stmt);
    }

    loops_blocks_deep--;
}

void SemanticAnalyzer::analyze_while_cycle_stmt(WhileCycleStmt& wcs) {
    analyze_expr(*wcs.condition);
    loops_blocks_deep++;

    for (const StmtPtr& stmt : wcs.block) {
        analyze_stmt(*stmt);
    }

    loops_blocks_deep--;
}

void SemanticAnalyzer::analyze_do_while_cycle_stmt(DoWhileCycleStmt& dwcs) {
    analyze_expr(*dwcs.condition);
    loops_blocks_deep++;

    for (const StmtPtr& stmt : dwcs.block) {
        analyze_stmt(*stmt);
    }

    loops_blocks_deep--;
}

void SemanticAnalyzer::analyze_break_stmt(BreakStmt& bs) {
    if (loops_blocks_deep == 0) {
        throw_error(file_name, file_name_in_error_printed, SEMANTIC, "`break` statement must be must be inside the loop\n", bs.line);
    }
}

void SemanticAnalyzer::analyze_continue_stmt(ContinueStmt& cs) {
    if (loops_blocks_deep == 0) {
        throw_error(file_name, file_name_in_error_printed, SEMANTIC, "`continue` statement must be must be inside the loop\n", cs.line);
    }
}

void SemanticAnalyzer::analyze_return_stmt(ReturnStmt& rs) {
    get_common_type(analyze_expr(*rs.expr), functions_types_stack.top(), rs.line);
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
    else {
        throw_error(file_name, file_name_in_error_printed, SEMANTIC, "Unsupported expression\n", expr.line);
    }
}

Type SemanticAnalyzer::analyze_literal(const Literal& lit) {
    return lit.type;
}

Type SemanticAnalyzer::analyze_binary_expr(const BinaryExpr& be) {
    Type left_type = analyze_expr(*be.left);
    Type right_type = analyze_expr(*be.right);

    return get_common_type(left_type, right_type, be.line);
}

Type SemanticAnalyzer::analyze_unary_expr(const UnaryExpr& ue) {
    return analyze_expr(*ue.expr);
}

Type SemanticAnalyzer::analyze_var_expr(const VarExpr& ve) {
    auto vars = variables;
    while (!vars.empty()) {
        auto var_it = vars.top().find(ve.name);
        if (var_it != vars.top().end()) {
            return var_it->second;
        }
        vars.pop();
    }

    throw_error(file_name, file_name_in_error_printed, SEMANTIC, "Variable '" + ve.name + "' does not exist\n", ve.line);
}

Type SemanticAnalyzer::analyze_func_call_expr(const FuncCallExpr& fce) {
    auto func_it = functions.find(fce.name);
    if (func_it == functions.end()) {
        std::string joined_args;
        if (!fce.args.empty()) {
            joined_args.reserve();
            joined_args.append(type_to_string(analyze_expr(*fce.args[0])));
            unsigned args_size = func_it->second.args.size();
            for (unsigned i = 1; i < args_size; i++) {
                joined_args.append(", ");
                joined_args.append(type_to_string(analyze_expr(*fce.args[i])));
            }
        }
        throw_error(file_name, file_name_in_error_printed, SEMANTIC, "Function '" + fce.name + '(' + joined_args + ")' does not exist\n", fce.line);
    }

    unsigned args_size = fce.args.size();
    for (unsigned i = 0; i < args_size; i++) {
        get_common_type(analyze_expr(*fce.args[i]), func_it->second.args[i].type, fce.line);
    }
    
    return func_it->second.return_type;
}

Type SemanticAnalyzer::get_common_type(Type left_type, Type right_type, int line) {
    if (left_type == right_type) {
        return left_type;
    }

    if (left_type.type <= TypeValue::F64) {
        if (right_type.type <= TypeValue::F64) {
            return left_type.type > right_type.type ? left_type : right_type;
        }
        else {
            if (right_type.type >= TypeValue::STRING) {
                throw_error(file_name, file_name_in_error_printed, SEMANTIC, "There is no common type between " + type_to_string(left_type) + " and " + type_to_string(right_type) + '\n', line);
            }
            else {
                return (int)left_type.type > (int)right_type.type - 6 ? left_type : right_type;
            }
        }
    }
    else {
        if (right_type.type <= TypeValue::F64) {
            return (int)left_type.type - 6 >= (int)right_type.type ? left_type : right_type;
        }
        else {
            if (right_type.type >= TypeValue::STRING) {
                throw_error(file_name, file_name_in_error_printed, SEMANTIC, "There is no common type between " + type_to_string(left_type) + " and " + type_to_string(right_type) + '\n', line);
            }
            else {
                return left_type.type > right_type.type ? left_type : right_type;
            }
        }
    }
}

std::string SemanticAnalyzer::type_to_string(Type type) {
    std::string base;
    switch (type.type) {
        case TypeValue::BOOL:
        case TypeValue::I8:
        case TypeValue::I16:
        case TypeValue::I32:
        case TypeValue::I64:
        case TypeValue::F32:
        case TypeValue::F64:
        case TypeValue::U8:
        case TypeValue::U16:
        case TypeValue::U32:
        case TypeValue::U64:
        case TypeValue::STRING:
        case TypeValue::NOTHING:
            base = type.name;
            break;
        case TypeValue::CLASS:
            base = "class <" + type.name + '>';
            break;
        case TypeValue::ENUM:
            base = "enum <" + type.name + '>';
            break;
    }
    if (type.is_const) {
        base = "const " + base;
    }
    if (type.is_pointer) {
        base += "*";
    }
    return base;
}