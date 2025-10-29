#pragma once
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include "../parser/ast.hpp"
#include <map>
#include <stack>
#include <string>
#include <utility>
#include <vector>

class CodeGenerator {
private:
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder;
    std::unique_ptr<llvm::Module> module;
    std::vector<StmtPtr> stmts;
    unsigned blocks_deep;
    std::stack<std::map<std::string, llvm::Value*>> variables;
    std::map<std::string, llvm::Function*> functions;
    std::stack<std::pair<llvm::BasicBlock*, llvm::BasicBlock*>> loop_blocks;    // first for `break`, second for `continue`

public:
    CodeGenerator(std::string n, std::vector<StmtPtr> s) : context(), builder(context), module(std::make_unique<llvm::Module>(n, context)),
                                                           stmts(std::move(s)), blocks_deep(0) {
        variables.push({});
    }

    void generate();
    std::unique_ptr<llvm::Module> get_module();
    void print_ir() const;

private:
    llvm::Type* get_llvm_type(Type type);

    void generate_stmt(const Stmt& stmt);
    void generate_var_decl_stmt(const VarDeclStmt& vds);
    void generate_func_decl_stmt(const FuncDeclStmt& fds);
    void generate_func_call_stmt(const FuncCallStmt& fcs);
    void generate_var_asgn_stmt(const VarAsgnStmt& vas);
    void generate_if_stmt(const IfStmt& is);
    void generate_for_cycle_stmt(const ForCycleStmt& fcs);
    void generate_while_cycle_stmt(const WhileCycleStmt& wcs);
    void generate_do_while_cycle_stmt(const DoWhileCycleStmt& wcs);
    void generate_break_stmt();
    void generate_continue_stmt();
    void generate_return_stmt(const ReturnStmt& rs);

    llvm::Value* generate_expr(const Expr& expr);
    llvm::Value* generate_literal(const Literal& lit);
    llvm::Value* generate_binary_expr(const BinaryExpr& be);
    llvm::Value* generate_unary_expr(const UnaryExpr& ue);
    llvm::Value* generate_var_expr(const VarExpr& ve);
    llvm::Value* generate_func_call_expr(const FuncCallExpr& fce);
    llvm::Value* implicitly_cast(llvm::Value* value, llvm::Type* expected_type);
};