#include "Engine.h"

static object_t NullObject() {
	object_t ret;
	ret.type = OT_NULL;
	ret.value._string = "null";
	ret.value._int = 0;
	return ret;
}

object_t Engine::Execute(ASTNode* node) {
	assert(node != nullptr);
	switch (node->Type()) {
		case AST_INT_LITERAL:
			return Execute((ASTIntLiteral*)node);
			break;
		case AST_ADD:
			return Execute((ASTAdd*)node);
			break;
		case AST_SUBTRACT:
			return Execute((ASTSubtract*)node);
			break;
		case AST_IDENTIFIER:
			return Execute((ASTIdentifier*)node);
			break;
		case AST_ASSIGN:
			return Execute((ASTAssign*)node);
			break;
		case AST_BLOCK:
			return Execute((ASTBlock*)node);
			break;
		case AST_CALL:
			return Execute((ASTCall*)node);
			break;
		case AST_INCREMENT:
			return Execute((ASTIncrement*)node);
			break;
		case AST_DECREMENT:
			return Execute((ASTDecrement*)node);
			break;
		case AST_IF:
			return Execute((ASTIf*)node);
			break;
		case AST_WHILE:
			return Execute((ASTWhile*)node);
			break;
		case AST_FUNC_DEF:
			break;
		case AST_BREAK:
			return Execute((ASTBreak*)node);
			break;
		case AST_RETURN:
			return Execute((ASTReturn*)node);
		case AST_NOT:
			return Execute((ASTNot*)node);
		default:
			assert(false);
			break;
	}

	return NullObject();
}

object_t Engine::Execute(ASTNot* node) {
	object_t result = Execute(node->Expression().get());
	result.value._int = !result.value._int;
	return result;
}

object_t Engine::Execute(ASTReturn* node) {
	object_t result = Execute(node->Expression().get());
	Set(F_RETURN);
	return result;
}

object_t Engine::Execute(ASTBreak* node) {
	Set(F_BREAK);
	return NullObject();
}

object_t Engine::Execute(ASTIf* node) {
	object_t expr = Execute(node->Expression().get());
	assert(expr.type == OT_INTEGER);
	if (expr.value._int) {
		return Execute(node->Statement().get());
	}
	return NullObject();
}

object_t Engine::Execute(ASTWhile* node) {
	object_t result = NullObject();
	while (!Test(F_BREAK) && !Test(F_RETURN)) {
		object_t expr = Execute(node->Expression().get());
		assert(expr.type == OT_INTEGER);
		if (expr.value._int == 0)
			break;
		result = Execute(node->Statement().get());
	}

	Clear(F_BREAK);
	return result;
}

object_t Engine::Execute(ASTIncrement* node) {
	ASTNodeRef child = node->Child(0);
	assert(child != nullptr);

	if (child->Type() != AST_IDENTIFIER) {
		object_t r = Execute(child.get());
		r.value._int++;
		return r;
	}
	
	ASTIdentifier* ident = (ASTIdentifier*)child.get();
	object_t* ref = _VariableLookup(ident->Name());
	assert(ref != nullptr);
	ref->value._int++;
	return *ref;
}

object_t Engine::Execute(ASTDecrement* node) {
	ASTNodeRef child = node->Child(0);
	assert(child != nullptr);

	if (child->Type() != AST_IDENTIFIER) {
		object_t r = Execute(child.get());
		r.value._int--;
		return r;
	}

	ASTIdentifier* ident = (ASTIdentifier*)child.get();
	object_t* ref = _VariableLookup(ident->Name());
	assert(ref != nullptr);
	ref->value._int--;
	return *ref;
}

object_t Engine::Execute(ASTAssign* node) {
	assert(node->NumChildren() == 2);
	Ref<ASTIdentifier> ident = node->LHS();
	object_t value = Execute(node->RHS().get());
	return *_VariableAssign(ident->Name(), value);
}

object_t Engine::Execute(ASTIntLiteral* node) {
	object_t ret;
	ret.type = OT_INTEGER;
	ret.value._int = node->Value();
	return ret;
}

object_t Engine::Execute(ASTSubtract* node) {
	assert(node->NumChildren() == 2);
	object_t a = Execute(node->Child(0).get());
	object_t b = Execute(node->Child(1).get());

	if (a.type == OT_INTEGER && b.type == OT_INTEGER) {
		object_t r;
		r.type = OT_INTEGER;
		r.value._int = a.value._int - b.value._int;
		return r;
	}

	return NullObject();
}

object_t Engine::Execute(ASTAdd* node) {

	assert(node->NumChildren() == 2);
	object_t a = Execute(node->Child(0).get());
	object_t b = Execute(node->Child(1).get());

	if (a.type == OT_INTEGER && b.type == OT_INTEGER) {
		object_t r;
		r.type = OT_INTEGER;
		r.value._int = a.value._int + b.value._int;
		return r;
	}

	return NullObject();
}

object_t Engine::Execute(ASTIdentifier* node) {

	return *_VariableLookup(node->Name());
}

object_t Engine::Execute(ASTCall* node) {

	vector<object_t> args;
	for (size_t i = 0; i < node->NumArguments(); ++i) {
		args.push_back(Execute(node->Argument(i).get()));
	}
	object_t result = _Invoke(node->Identifier()->Name(), args);
	Clear(F_RETURN);
	return result;
}

object_t Engine::Execute(ASTBlock* node) {
	if (!Executing()) return NullObject();

	_PushScope();
	object_t result = NullObject();
	for (size_t i = 0; i < node->NumChildren(); ++i) {
		if (!Executing())
			break;
		result = Execute(node->Child(i).get());
	}
	_PopScope();
	return result;
}

void Engine::Execute(ASTProgram* program) {
	_PopulateFunctions(program);
	_PushSpace();
	_PushScope();
	for (size_t i = 0; i < program->NumChildren(); ++i) {
		if (!Executing())
			break;
		Execute(program->Child(i).get());
	}
	_PopScope();
	_PopSpace();
}
