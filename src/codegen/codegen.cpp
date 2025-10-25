#include <algorithm>
#include <iostream>
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/APInt.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalValue.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include <llvm/Support/Casting.h>
#include <llvm/Support/raw_ostream.h>
#include <memory>
#include <vector>
#include "../../include/parser/ast.hpp"
#include "../../include/codegen/codegen.hpp"

void CodeGenerator::generate() {
    for (StmtPtr& stmt : stmts) {
        generate_stmt(*stmt);
    }
}

std::unique_ptr<llvm::Module> CodeGenerator::get_module() {
    return std::move(module);
}

void CodeGenerator::print_ir() const {
    module->print(llvm::outs(), nullptr);
}

llvm::Type* CodeGenerator::get_llvm_type(Type type) {
    switch (type.type) {
        case TypeValue::CHAR: {
            llvm::Type* llvm_type = llvm::Type::getInt8Ty(context);
            if (type.is_pointer) {
                return llvm::PointerType::get(llvm_type, 0);
            }
            return llvm_type;
        }
        case TypeValue::SHORT: {
            llvm::Type* llvm_type = llvm::Type::getInt16Ty(context);
            if (type.is_pointer) {
                return llvm::PointerType::get(llvm_type, 0);
            }
            return llvm_type;
        }
        case TypeValue::INT: {
            llvm::Type* llvm_type = llvm::Type::getInt32Ty(context);
            if (type.is_pointer) {
                return llvm::PointerType::get(llvm_type, 0);
            }
            return llvm_type;
        }
        case TypeValue::LONG: {
            llvm::Type* llvm_type = llvm::Type::getInt64Ty(context);
            if (type.is_pointer) {
                return llvm::PointerType::get(llvm_type, 0);
            }
            return llvm_type;
        }
        case TypeValue::FLOAT: {
            llvm::Type* llvm_type = llvm::Type::getFloatTy(context);
            if (type.is_pointer) {
                return llvm::PointerType::get(llvm_type, 0);
            }
            return llvm_type;
        }
        case TypeValue::DOUBLE: {
            llvm::Type* llvm_type = llvm::Type::getDoubleTy(context);
            if (type.is_pointer) {
                return llvm::PointerType::get(llvm_type, 0);
            }
            return llvm_type;
        }
    }
}

void CodeGenerator::generate_stmt(const Stmt& stmt) {
    if (auto vds = dynamic_cast<const VarDeclStmt*>(&stmt)) {
        generate_var_decl_stmt(*vds);
    }
    else if (auto fds = dynamic_cast<const FuncDeclStmt*>(&stmt)) {
        generate_func_decl_stmt(*fds);
    }
}

void CodeGenerator::generate_var_decl_stmt(const VarDeclStmt& vds) {
    llvm::Type* var_type = get_llvm_type(vds.type);
    llvm::Value* var_init_val = nullptr;
    if (vds.expr) {
        var_init_val = generate_expr(*vds.expr);
    }
    else {
        var_init_val = llvm::Constant::getNullValue(var_type);
    }
    if (blocks_deep == 0) {
        llvm::GlobalVariable* glob_var = new llvm::GlobalVariable(*module, var_type, vds.type.is_const, llvm::GlobalValue::ExternalLinkage, llvm::dyn_cast<llvm::Constant>(var_init_val), vds.name);
    }
    else {
        llvm::AllocaInst* local_var = builder.CreateAlloca(var_type, nullptr, vds.name);
        builder.CreateStore(var_init_val, local_var);
    }
}

void CodeGenerator::generate_func_decl_stmt(const FuncDeclStmt& fds) {
    llvm::Type* func_ret_type = get_llvm_type(fds.type);
    std::vector<llvm::Type*> param_types;
    for (const Argument& arg : fds.args) {
        param_types.push_back(get_llvm_type(arg.type));
    }
    llvm::FunctionType* func_type = llvm::FunctionType::get(func_ret_type, param_types, false);
    llvm::Function* func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, fds.name, *module);

    llvm::BasicBlock* entry = llvm::BasicBlock::Create(context, "entry", func);
    builder.SetInsertPoint(entry);
    blocks_deep++;

    size_t index = 0;
    for (llvm::Argument& arg : func->args()) {
        arg.setName(fds.args[index].name);
        llvm::AllocaInst* alloca = builder.CreateAlloca(arg.getType(), nullptr, fds.args[index].name + ".addr");
        builder.CreateStore(&arg, alloca);
    }
    for (const StmtPtr& stmt : fds.block) {
        generate_stmt(*stmt);
    }

    builder.CreateRet(llvm::Constant::getIntegerValue(llvm::Type::getInt32Ty(context), llvm::APInt(32, 0)));
    blocks_deep--;
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