#pragma once
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include <parser/ast.hpp>
#include <string>
#include <vector>

class CodeGenerator {
private:
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder;
    std::unique_ptr<llvm::Module> module;
    std::vector<StmtPtr> stmts;

public:
    CodeGenerator(std::string n, std::vector<StmtPtr> s) : context(), builder(context), module(std::make_unique<llvm::Module>(n, context)),
                                                           stmts(std::move(s)) {}

    void generate();

private:
    llvm::Type* get_llvm_type(Type type);

    void generate_stmt(const Stmt& stmt);
    void generate_var_decl_stmt(const VarDeclStmt& stmt);

    llvm::Value* generate_expr(const Expr& expr);
    llvm::Value* generate_literal(const Literal& literal);
};