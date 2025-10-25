#pragma once
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include "../parser/ast.hpp"
#include <map>
#include <string>
#include <vector>

class CodeGenerator {
private:
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder;
    std::unique_ptr<llvm::Module> module;
    std::vector<StmtPtr> stmts;
    unsigned blocks_deep;
    std::map<std::string, llvm::Type*> variables;

public:
    CodeGenerator(std::string n, std::vector<StmtPtr> s) : context(), builder(context), module(std::make_unique<llvm::Module>(n, context)),
                                                           stmts(std::move(s)), blocks_deep(0) {}

    void generate();
    std::unique_ptr<llvm::Module> get_module();
    void print_ir() const;

private:
    llvm::Type* get_llvm_type(Type type);

    void generate_stmt(const Stmt& stmt);
    void generate_var_decl_stmt(const VarDeclStmt& vds);
    void generate_func_decl_stmt(const FuncDeclStmt& fds);

    llvm::Value* generate_expr(const Expr& expr);
    llvm::Value* generate_literal(const Literal& literal);
};