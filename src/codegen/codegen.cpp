#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/APInt.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/GlobalValue.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include <llvm/Support/Casting.h>
#include <parser/ast.hpp>
#include <codegen/codegen.hpp>

void CodeGenerator::generate() {
    for (StmtPtr& stmt : stmts) {
        generate_stmt(*stmt);
    }
}

llvm::Type* CodeGenerator::get_llvm_type(Type type) {
    switch (type.type) {
        case TypeValue::CHAR:
            return llvm::Type::getInt8Ty(context);
        case TypeValue::SHORT:
            return llvm::Type::getInt16Ty(context);
        case TypeValue::INT:
            return llvm::Type::getInt32Ty(context);
        case TypeValue::LONG:
            return llvm::Type::getInt64Ty(context);
        case TypeValue::FLOAT:
            return llvm::Type::getFloatTy(context);
        case TypeValue::DOUBLE:
            return llvm::Type::getDoubleTy(context);
    }
}

void CodeGenerator::generate_stmt(const Stmt& stmt) {
    if (auto vds = dynamic_cast<const VarDeclStmt*>(&stmt)) {
        generate_var_decl_stmt(*vds);
    }
}

void CodeGenerator::generate_var_decl_stmt(const VarDeclStmt& stmt) {
    llvm::Type* var_type = get_llvm_type(stmt.type);
    llvm::Value* var_init_val = nullptr;
    if (stmt.expr) {
        var_init_val = generate_expr(*stmt.expr);
    }
    else {
        var_init_val = llvm::Constant::getNullValue(var_type);
    }
    llvm::GlobalVariable glob_var = llvm::GlobalVariable(var_type, stmt.type.is_const, llvm::GlobalValue::ExternalLinkage, llvm::dyn_cast<llvm::Constant>(var_init_val), stmt.name);
}

llvm::Value* CodeGenerator::generate_expr(const Expr& expr) {
    if (auto lit = dynamic_cast<const Literal*>(&expr)) {
        return generate_literal(*lit);
    }
}

llvm::Value* CodeGenerator::generate_literal(const Literal& literal) {
    const auto& value = literal.value.value;

    switch (literal.type.type) {
        case TypeValue::CHAR:
            return llvm::ConstantInt::get(context, llvm::APInt(8, std::get<char>(value), !literal.type.is_unsigned));
        case TypeValue::SHORT:
            return llvm::ConstantInt::get(context, llvm::APInt(16, std::get<short>(value), !literal.type.is_unsigned));
        case TypeValue::INT:
            return llvm::ConstantInt::get(context, llvm::APInt(32, std::get<int>(value), !literal.type.is_unsigned));
        case TypeValue::LONG:
            return llvm::ConstantInt::get(context, llvm::APInt(64, std::get<long>(value), !literal.type.is_unsigned));
        case TypeValue::FLOAT:
            return llvm::ConstantFP::get(context, llvm::APFloat(std::get<float>(value)));
        case TypeValue::DOUBLE:
            return llvm::ConstantFP::get(context, llvm::APFloat(std::get<double>(value)));
    }
}