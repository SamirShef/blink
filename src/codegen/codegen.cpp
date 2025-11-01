#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/Support/Casting.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/GlobalValue.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/ADT/APFloat.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Function.h>
#include <llvm/ADT/Twine.h>
#include <llvm/ADT/APInt.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>

#include "../../include/exception/exception.hpp"
#include "../../include/codegen/codegen.hpp"
#include "../../include/parser/ast.hpp"
#include <iostream>

void CodeGenerator::generate() {
    llvm::Function* printf_func = module->getFunction("printf");
    if (!printf_func) {
        llvm::FunctionType* printf_type = llvm::FunctionType::get(builder.getInt32Ty(), llvm::PointerType::get(builder.getInt8Ty(), 0), true);
        printf_func = llvm::Function::Create(printf_type, llvm::Function::ExternalLinkage, "printf", *module);
    }
    functions.emplace("printf", printf_func);
    
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

llvm::Type* CodeGenerator::get_llvm_type(Type type, int line) {
    switch (type.type) {
        case TypeValue::I8:
        case TypeValue::U8: {
            llvm::Type* llvm_type = llvm::Type::getInt8Ty(context);
            if (type.is_pointer) {
                return llvm::PointerType::get(llvm_type, 0);
            }
            return llvm_type;
        }
        case TypeValue::I16:
        case TypeValue::U16: {
            llvm::Type* llvm_type = llvm::Type::getInt16Ty(context);
            if (type.is_pointer) {
                return llvm::PointerType::get(llvm_type, 0);
            }
            return llvm_type;
        }
        case TypeValue::I32:
        case TypeValue::U32: {
            llvm::Type* llvm_type = llvm::Type::getInt32Ty(context);
            if (type.is_pointer) {
                return llvm::PointerType::get(llvm_type, 0);
            }
            return llvm_type;
        }
        case TypeValue::I64:
        case TypeValue::U64: {
            llvm::Type* llvm_type = llvm::Type::getInt64Ty(context);
            if (type.is_pointer) {
                return llvm::PointerType::get(llvm_type, 0);
            }
            return llvm_type;
        }
        case TypeValue::F32: {
            llvm::Type* llvm_type = llvm::Type::getFloatTy(context);
            if (type.is_pointer) {
                return llvm::PointerType::get(llvm_type, 0);
            }
            return llvm_type;
        }
        case TypeValue::F64: {
            llvm::Type* llvm_type = llvm::Type::getDoubleTy(context);
            if (type.is_pointer) {
                return llvm::PointerType::get(llvm_type, 0);
            }
            return llvm_type;
        }
        case TypeValue::BOOL: {
            llvm::Type* llvm_type = llvm::Type::getInt1Ty(context);
            if (type.is_pointer) {
                return llvm::PointerType::get(llvm_type, 0);
            }
            return llvm_type;
        }
        case TypeValue::NOTHING:
            return llvm::Type::getVoidTy(context);
        default: {
            throw_error(file_name, file_name_in_error_printed, CODEGEN, "Unsupported type\n", line);
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
    else if (auto fcs = dynamic_cast<const FuncCallStmt*>(&stmt)) {
        generate_func_call_stmt(*fcs);
    }
    else if (auto vas = dynamic_cast<const VarAsgnStmt*>(&stmt)) {
        generate_var_asgn_stmt(*vas);
    }
    else if (auto is = dynamic_cast<const IfStmt*>(&stmt)) {
        generate_if_stmt(*is);
    }
    else if (auto fcs = dynamic_cast<const ForCycleStmt*>(&stmt)) {
        generate_for_cycle_stmt(*fcs);
    }
    else if (auto wcs = dynamic_cast<const WhileCycleStmt*>(&stmt)) {
        generate_while_cycle_stmt(*wcs);
    }
    else if (auto dwcs = dynamic_cast<const DoWhileCycleStmt*>(&stmt)) {
        generate_do_while_cycle_stmt(*dwcs);
    }
    else if (dynamic_cast<const BreakStmt*>(&stmt)) {
        generate_break_stmt();
    }
    else if (dynamic_cast<const ContinueStmt*>(&stmt)) {
        generate_continue_stmt();
    }
    else if (auto rs = dynamic_cast<const ReturnStmt*>(&stmt)) {
        generate_return_stmt(*rs);
    }
    else {
        throw_error(file_name, file_name_in_error_printed, CODEGEN, "Unsupported statement\n", stmt.line);
    }
}

void CodeGenerator::generate_var_decl_stmt(const VarDeclStmt& vds) {
    llvm::Type* var_type = get_llvm_type(vds.type, vds.line);
    llvm::Value* var_init_val = nullptr;
    if (vds.expr) {
        var_init_val = implicitly_cast(generate_expr(*vds.expr), var_type, vds.line);
    }
    else {
        var_init_val = llvm::Constant::getNullValue(var_type);
    }
    if (blocks_deep == 0) {
        llvm::GlobalVariable* glob_var = new llvm::GlobalVariable(*module, var_type, vds.type.is_const, llvm::GlobalValue::ExternalLinkage, llvm::dyn_cast<llvm::Constant>(var_init_val), vds.name);
        variables.top().emplace(vds.name, glob_var);
    }
    else {
        llvm::AllocaInst* local_var = builder.CreateAlloca(var_type, nullptr, vds.name);
        builder.CreateStore(var_init_val, local_var);
        variables.top().emplace(vds.name, local_var);
    }
}

void CodeGenerator::generate_func_decl_stmt(const FuncDeclStmt& fds) {
    llvm::Type* func_ret_type = get_llvm_type(fds.return_type, fds.line);
    std::vector<llvm::Type*> param_types;
    for (const Argument& arg : fds.args) {
        param_types.push_back(get_llvm_type(arg.type, fds.line));
    }
    llvm::FunctionType* func_type = llvm::FunctionType::get(func_ret_type, param_types, false);
    llvm::Function* func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, fds.name, *module);

    llvm::BasicBlock* entry = llvm::BasicBlock::Create(context, "entry", func);
    builder.SetInsertPoint(entry);
    blocks_deep++;
    variables.push({});
    functions.emplace(fds.name, func);

    size_t index = 0;
    for (llvm::Argument& arg : func->args()) {
        arg.setName(fds.args[index].name);
        llvm::AllocaInst* arg_alloca = builder.CreateAlloca(arg.getType(), nullptr, fds.args[index].name);
        builder.CreateStore(&arg, arg_alloca);
        variables.top().emplace(fds.args[index].name, arg_alloca);
        index++;
    }
    for (const StmtPtr& stmt : fds.block) {
        generate_stmt(*stmt);
    }

    blocks_deep--;
    variables.pop();
}

void CodeGenerator::generate_func_call_stmt(const FuncCallStmt& fcs) {
    if (functions.empty() || functions.find(fcs.name) == functions.end()) {
        throw_error(file_name, file_name_in_error_printed, CODEGEN, "Function '" + fcs.name + "' does not exist\n", fcs.line);
    }
    
    std::vector<llvm::Value*> args;
    for (auto& arg : fcs.args) {
        args.push_back(generate_expr(*arg));
    }
    builder.CreateCall(functions[fcs.name], args, fcs.name + ".call");
}

void CodeGenerator::generate_var_asgn_stmt(const VarAsgnStmt& vas) {
    llvm::Value* value = generate_expr(*vas.expr);
    llvm::Value* var_ptr = nullptr;
    bool have_var = false;
    std::stack<std::map<std::string, llvm::Value*>> vars = variables;
    while (!vars.empty()) {
        std::map<std::string, llvm::Value*>& c_scope = vars.top();
        auto varIt = c_scope.find(vas.name);
        if (varIt != c_scope.end()) {
            var_ptr = varIt->second;
            have_var = true;
            break;
        }
        vars.pop();
    }
    if (!have_var) {
        throw_error(file_name, file_name_in_error_printed, CODEGEN, "Variable '" + vas.name + "' does not exist\n", vas.line);
    }

    llvm::Type* var_type;
    if (auto global = llvm::dyn_cast<llvm::GlobalVariable>(var_ptr)) {
        var_type = global->getValueType();
    }
    else if (auto local = llvm::dyn_cast<llvm::AllocaInst>(var_ptr)) {
        var_type = local->getAllocatedType();
    }
    else if (auto value = llvm::dyn_cast<llvm::Value>(var_ptr)) {
        var_type = value->getType();
    }
    value = implicitly_cast(value, var_type, vas.line);

    builder.CreateStore(value, var_ptr);
}

void CodeGenerator::generate_if_stmt(const IfStmt& is) {
    llvm::Value* cond = generate_expr(*is.condition);
    llvm::Function* func = builder.GetInsertBlock()->getParent();
    
    llvm::BasicBlock* true_bb = llvm::BasicBlock::Create(context, "then", func);
    llvm::BasicBlock* false_bb = llvm::BasicBlock::Create(context, "else", func);
    llvm::BasicBlock* merge_bb = llvm::BasicBlock::Create(context, "merge", func);

    builder.CreateCondBr(cond, true_bb, false_bb ? false_bb : merge_bb);

    builder.SetInsertPoint(true_bb);
    variables.push({});
    for (const StmtPtr& stmt : is.true_block) {
        generate_stmt(*stmt);
    }
    variables.pop();

    if (builder.GetInsertBlock()->getTerminator() == nullptr) {
        builder.CreateBr(merge_bb);
    }
    builder.SetInsertPoint(false_bb);
    variables.push({});
    for (const StmtPtr& stmt : is.false_block) {
        generate_stmt(*stmt);
    }
    variables.pop();
    
    if (builder.GetInsertBlock()->getTerminator() == nullptr) {
        builder.CreateBr(merge_bb);
    }
    builder.SetInsertPoint(merge_bb);
}

void CodeGenerator::generate_for_cycle_stmt(const ForCycleStmt& fcs) {
    llvm::Function* function = builder.GetInsertBlock()->getParent();

    llvm::BasicBlock* indexator_bb = llvm::BasicBlock::Create(context, "for.indexator", function);
    llvm::BasicBlock* condition_bb = llvm::BasicBlock::Create(context, "for.condition", function);
    llvm::BasicBlock* iteration_bb = llvm::BasicBlock::Create(context, "for.iteration", function);
    llvm::BasicBlock* body_bb = llvm::BasicBlock::Create(context, "for.body", function);
    llvm::BasicBlock* exit_bb = llvm::BasicBlock::Create(context, "for.exit", function);

    builder.CreateBr(indexator_bb);
    builder.SetInsertPoint(indexator_bb);
    generate_stmt(*fcs.indexator);

    builder.CreateBr(condition_bb);
    builder.SetInsertPoint(condition_bb);
    llvm::Value* condition_value = generate_expr(*fcs.condition);

    builder.CreateCondBr(condition_value, body_bb, exit_bb);
    builder.SetInsertPoint(body_bb);
    loop_blocks.emplace(exit_bb, iteration_bb);
    blocks_deep++;
    variables.push({});
    for (const StmtPtr& stmt : fcs.block) {
        generate_stmt(*stmt);
    }
    variables.pop();
    blocks_deep--;
    loop_blocks.pop();

    builder.CreateBr(iteration_bb);
    builder.SetInsertPoint(iteration_bb);
    generate_stmt(*fcs.iteration);

    builder.CreateBr(condition_bb);
    builder.SetInsertPoint(exit_bb);
}

void CodeGenerator::generate_while_cycle_stmt(const WhileCycleStmt& wcs) {
    llvm::Function* function = builder.GetInsertBlock()->getParent();

    llvm::BasicBlock* condition_bb = llvm::BasicBlock::Create(context, "while.condition", function);
    llvm::BasicBlock* body_bb = llvm::BasicBlock::Create(context, "while.body", function);
    llvm::BasicBlock* exit_bb = llvm::BasicBlock::Create(context, "while.exit", function);

    builder.CreateBr(condition_bb);
    builder.SetInsertPoint(condition_bb);
    llvm::Value* condition_value = generate_expr(*wcs.condition);

    builder.CreateCondBr(condition_value, body_bb, exit_bb);
    builder.SetInsertPoint(body_bb);
    loop_blocks.emplace(exit_bb, condition_bb);
    blocks_deep++;
    variables.push({});
    for (const StmtPtr& stmt : wcs.block) {
        generate_stmt(*stmt);
    }
    variables.pop();
    blocks_deep--;
    loop_blocks.pop();

    builder.CreateBr(condition_bb);
    builder.SetInsertPoint(exit_bb);
}

void CodeGenerator::generate_do_while_cycle_stmt(const DoWhileCycleStmt& dwcs) {
    llvm::Function* function = builder.GetInsertBlock()->getParent();

    llvm::BasicBlock* condition_bb = llvm::BasicBlock::Create(context, "dowhile.condition", function);
    llvm::BasicBlock* body_bb = llvm::BasicBlock::Create(context, "dowhile.body", function);
    llvm::BasicBlock* exit_bb = llvm::BasicBlock::Create(context, "dowhile.exit", function);

    builder.CreateBr(body_bb);
    builder.SetInsertPoint(body_bb);
    loop_blocks.emplace(exit_bb, condition_bb);
    blocks_deep++;
    variables.push({});
    for (const StmtPtr& stmt : dwcs.block) {
        generate_stmt(*stmt);
    }
    variables.pop();
    blocks_deep--;
    loop_blocks.pop();

    builder.CreateBr(condition_bb);
    builder.SetInsertPoint(condition_bb);
    llvm::Value* condition_value = generate_expr(*dwcs.condition);
    builder.CreateCondBr(condition_value, body_bb, exit_bb);

    builder.SetInsertPoint(exit_bb);
}

void CodeGenerator::generate_break_stmt() {
    builder.CreateBr(loop_blocks.top().first);
}

void CodeGenerator::generate_continue_stmt() {
    builder.CreateBr(loop_blocks.top().second);
}

void CodeGenerator::generate_return_stmt(const ReturnStmt& rs) {
    if (rs.expr == nullptr) {
        builder.CreateRetVoid();
        return;
    }

    llvm::Value* value = generate_expr(*rs.expr);
    builder.CreateRet(value);
}

llvm::Value* CodeGenerator::generate_expr(const Expr& expr) {
    if (auto lit = dynamic_cast<const Literal*>(&expr)) {
        return generate_literal(*lit);
    }
    else if (auto be = dynamic_cast<const BinaryExpr*>(&expr)) {
        return generate_binary_expr(*be);
    }
    else if (auto ue = dynamic_cast<const UnaryExpr*>(&expr)) {
        return generate_unary_expr(*ue);
    }
    else if (auto ve = dynamic_cast<const VarExpr*>(&expr)) {
        return generate_var_expr(*ve);
    }
    else if (auto fce = dynamic_cast<const FuncCallExpr*>(&expr)) {
        return generate_func_call_expr(*fce);
    }
    else {
        throw_error(file_name, file_name_in_error_printed, CODEGEN, "Unsupported expression\n", expr.line);
    }
}

llvm::Value* CodeGenerator::generate_literal(const Literal& lit) {
    const auto& value = lit.value.value;

    switch (lit.type.type) {
        case TypeValue::I8:
            return llvm::ConstantInt::get(context, llvm::APInt(8, std::get<std::int8_t>(value), !lit.type.is_unsigned));
        case TypeValue::I16:
            return llvm::ConstantInt::get(context, llvm::APInt(16, std::get<std::int16_t>(value), !lit.type.is_unsigned));
        case TypeValue::I32:
            return llvm::ConstantInt::get(context, llvm::APInt(32, std::get<std::int32_t>(value), !lit.type.is_unsigned));
        case TypeValue::I64:
            return llvm::ConstantInt::get(context, llvm::APInt(64, std::get<std::int64_t>(value), !lit.type.is_unsigned));
        case TypeValue::F32:
            return llvm::ConstantFP::get(context, llvm::APFloat(std::get<std::float_t>(value)));
        case TypeValue::F64:
            return llvm::ConstantFP::get(context, llvm::APFloat(std::get<std::double_t>(value)));
        case TypeValue::U8:
            return llvm::ConstantInt::get(context, llvm::APInt(8, std::get<std::uint8_t>(value), !lit.type.is_unsigned));
        case TypeValue::U16:
            return llvm::ConstantInt::get(context, llvm::APInt(16, std::get<std::uint16_t>(value), !lit.type.is_unsigned));
        case TypeValue::U32:
            return llvm::ConstantInt::get(context, llvm::APInt(32, std::get<std::uint32_t>(value), !lit.type.is_unsigned));
        case TypeValue::U64:
            return llvm::ConstantInt::get(context, llvm::APInt(64, std::get<std::uint64_t>(value), !lit.type.is_unsigned));
        case TypeValue::BOOL:
            return llvm::ConstantInt::get(context, llvm::APInt(1, std::get<bool>(value), !lit.type.is_unsigned));
        case TypeValue::STRING: {
                return builder.CreateGlobalString(std::get<std::string>(value), "string_lit");
            }
        default:
            throw_error(file_name, file_name_in_error_printed, CODEGEN, "Unsupported literal\n", lit.line);
    }
}

llvm::Value* CodeGenerator::generate_binary_expr(const BinaryExpr& be) {
    llvm::Value* left = generate_expr(*be.left);
    llvm::Value* right = generate_expr(*be.right);

    auto get_common_type = [&](llvm::Value* left, llvm::Value* right) -> llvm::Type* {
        llvm::Type* left_type = left->getType();
        llvm::Type* right_type = right->getType();

        if (left_type->isDoubleTy() || right_type->isDoubleTy()) {
            return llvm::Type::getDoubleTy(context);
        }
        else if (left_type->isFloatTy() && right_type->isFloatTy()) {
            return llvm::Type::getFloatTy(context);
        }
        else if (left_type->isIntegerTy() && right_type->isIntegerTy()) {
            unsigned left_width = left_type->getIntegerBitWidth();
            unsigned right_width = right_type->getIntegerBitWidth();

            if (left_width == right_width || left_width > right_width) {
                return left_type;
            }
            else {
                return right_type;
            }
        }

        return nullptr;
    };
    
    llvm::Type* left_type = left->getType();
    llvm::Type* right_type = right->getType();
    llvm::Type* common_type = get_common_type(left, right);
    if (common_type == nullptr) {
        std::cerr << "codegen: There is no common type between ";
        left_type->print(llvm::outs());
        std::cerr << " and ";
        right->getType()->print(llvm::outs());
        std::cerr << '\n';
        exit(1);
    }
    if (left_type != common_type) {
        left = implicitly_cast(left, common_type, be.line);
        left_type = left->getType();
    }
    else if (right_type != common_type) {
        right = implicitly_cast(right, common_type, be.line);
        right_type = right->getType();
    }
    switch (be.op_type) {
        case TokenType::PLUS:
            if (left_type->isFloatingPointTy() || right_type->isFloatingPointTy()) {
                return builder.CreateFAdd(left, right, "addtmp");
            }
            else {
                return builder.CreateAdd(left, right, "addtmp");
            }
        case TokenType::MINUS:
            if (left_type->isFloatingPointTy() || right_type->isFloatingPointTy()) {
                return builder.CreateFSub(left, right, "subtmp");
            }
            else {
                return builder.CreateSub(left, right, "subtmp");
            }
        case TokenType::MULT:
            if (left_type->isFloatingPointTy() || right_type->isFloatingPointTy()) {
                return builder.CreateFMul(left, right, "multmp");
            }
            else {
                return builder.CreateMul(left, right, "multmp");
            }
        case TokenType::DIV:
            if (left_type->isFloatingPointTy() || right_type->isFloatingPointTy()) {
                return builder.CreateFDiv(left, right, "divtmp");
            }
            else {
                return builder.CreateSDiv(left, right, "divtmp");
            }
        case TokenType::MODULO:
            if (left_type->isFloatingPointTy() || right_type->isFloatingPointTy()) {
                return builder.CreateFRem(left, right, "remtmp");
            }
            else {
                return builder.CreateSRem(left, right, "remtmp");
            }
        case TokenType::GT:
            if (left_type->isFloatingPointTy() || right_type->isFloatingPointTy()) {
                return builder.CreateFCmpOGT(left, right, "gttmp");
            }
            else {
                return builder.CreateICmpSGT(left, right, "gttmp");
            }
        case TokenType::GT_EQ:
            if (left_type->isFloatingPointTy() || right_type->isFloatingPointTy()) {
                return builder.CreateFCmpOGE(left, right, "getmp");
            }
            else {
                return builder.CreateICmpSGE(left, right, "getmp");
            }
        case TokenType::LS:
            if (left_type->isFloatingPointTy() || right_type->isFloatingPointTy()) {
                return builder.CreateFCmpOLT(left, right, "lttmp");
            }
            else {
                return builder.CreateICmpSLT(left, right, "lttmp");
            }
        case TokenType::LS_EQ:
            if (left_type->isFloatingPointTy() || right_type->isFloatingPointTy()) {
                return builder.CreateFCmpOLE(left, right, "letmp");
            }
            else {
                return builder.CreateICmpSLE(left, right, "letmp");
            }
        case TokenType::EQ_EQ:
            if (left_type->isFloatingPointTy() || right_type->isFloatingPointTy()) {
                return builder.CreateFCmpOEQ(left, right, "eqtmp");
            }
            else {
                return builder.CreateICmpEQ(left, right, "eqtmp");
            }
        case TokenType::NOT_EQ:
            if (left_type->isFloatingPointTy() || right_type->isFloatingPointTy()) {
                return builder.CreateFCmpONE(left, right, "netmp");
            }
            else {
                return builder.CreateICmpNE(left, right, "netmp");
            }
        case TokenType::L_AND:
            return builder.CreateLogicalAnd(left, right, "landtmp");
        case TokenType::L_OR:
            return builder.CreateLogicalOr(left, right, "lortmp");
        default: {}
    }
}

llvm::Value* CodeGenerator::generate_unary_expr(const UnaryExpr& ue) {
    llvm::Value* value = generate_expr(*ue.expr);
    
    switch (ue.op_type) {
        case TokenType::MINUS:
            if (value->getType()->isFloatingPointTy()) {
                return builder.CreateFNeg(value, "negtmp");
            }
            else {
                return builder.CreateNeg(value, "negtmp");
            }
        case TokenType::L_NOT:
            if (value->getType()->isFloatingPointTy()) {
                return builder.CreateFCmpOEQ(value, builder.getInt32(0), "lnottmp");
            }
            else {
                return builder.CreateICmpEQ(value, builder.getInt32(0), "lnottmp");
            }
        default: {}
    }
}

llvm::Value* CodeGenerator::generate_var_expr(const VarExpr& ve) {
    std::stack<std::map<std::string, llvm::Value*>> vars = variables;
    while (!vars.empty()) {
        std::map<std::string, llvm::Value*>& c_scope = vars.top();
        auto var_it = c_scope.find(ve.name);
        if (var_it != c_scope.end()) {
            llvm::Type* type = nullptr;
            if (auto global = llvm::dyn_cast<llvm::GlobalVariable>(var_it->second)) {
                type = global->getValueType();
            }
            else if (auto local = llvm::dyn_cast<llvm::AllocaInst>(var_it->second)) {
                type = local->getAllocatedType();
            }
            else if (auto value = llvm::dyn_cast<llvm::Value>(var_it->second)) {
                type = value->getType();
            }
            return builder.CreateLoad(type, var_it->second, ve.name + ".load");
        }
        vars.pop();
    }

    throw_error(file_name, file_name_in_error_printed, CODEGEN, "Variable '" + ve.name + "' does not exist\n", ve.line);
}

llvm::Value* CodeGenerator::generate_func_call_expr(const FuncCallExpr& fce) {
    if (functions.empty() || functions.find(fce.name) == functions.end()) {
        throw_error(file_name, file_name_in_error_printed, CODEGEN, "Function '" + fce.name + "' does not exist\n", fce.line);
    }
    
    std::vector<llvm::Value*> args;
    for (auto& arg : fce.args) {
        args.push_back(generate_expr(*arg));
    }
    return builder.CreateCall(functions[fce.name], args, fce.name + ".call");
}

llvm::Value* CodeGenerator::implicitly_cast(llvm::Value* value, llvm::Type* expected_type, int line) {
    llvm::Type* value_type = value->getType();

    if (value_type == expected_type)  {
        return value;
    }
    else if (value_type->isIntegerTy() && expected_type->isIntegerTy()) {
        unsigned long value_width = value_type->getIntegerBitWidth();
        unsigned long expected_width = expected_type->getIntegerBitWidth();

        if (value_width == expected_width) {
            return value;
        }
        else if (value_width > expected_width) {
            return builder.CreateTrunc(value, expected_type, "trunctmp");
        }
        else {
            return builder.CreateSExt(value, expected_type, "sexttmp");
        }
    }
    else if (value_type->isFloatingPointTy() && expected_type->isFloatingPointTy()) {
        if (value_type->isFloatTy() && expected_type->isFloatTy() || value_type->isDoubleTy() && expected_type->isDoubleTy()) {
            return value;
        }
        else if (value_type->isFloatTy() && expected_type->isDoubleTy()) {
            return builder.CreateFPExt(value, expected_type, "fpexttmp");
        }
        else {
            return builder.CreateFPTrunc(value, expected_type, "fptrunctmp");
        }
    }
    else if (value_type->isIntegerTy() && expected_type->isFloatingPointTy()) {
        return builder.CreateSIToFP(value, expected_type, "sitofptmp");
    }

    std::cerr << "codegen: Unknown type to implicitly cast (";
    value_type->print(llvm::outs());
    std::cerr << " to ";
    expected_type->print(llvm::outs());
    std::cerr << ")\n";
    exit(1);
}