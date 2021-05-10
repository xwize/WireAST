#include "Engine.h"

Engine::Engine() {
	callbacks = new CallbackRegistry();

	globalVariableSpace = nullptr;
	currentVariableSpace = nullptr;
	flags = F_NONE;
}

Engine::~Engine() {
}

void Engine::DefineCallback(const string& name, size_t numParams, callbackFunction_t func) {
	callback_t cb;
	cb.name = name;
	cb.callback = func;
	cb.parameters = numParams;
	DefineCallback(cb);
}

void Engine::DefineCallback(const callback_t& callback) {
	callbacks->Put(callback.name, callback);
}

void Engine::_PushScope() {
	assert(currentVariableSpace != nullptr);
	currentVariableSpace->PushScope();
}

void Engine::_PopScope() {
	assert(currentVariableSpace != nullptr);
	currentVariableSpace->PopScope();
}

void Engine::_PushSpace() {
	variableSpaces.push_back( new VariableSpace() );
	currentVariableSpace = variableSpaces.back().get();
	if (globalVariableSpace == nullptr) {
		assert(variableSpaces.size() == 1);
		globalVariableSpace = variableSpaces[0].get();
	}
}

void Engine::_PopSpace() {
	variableSpaces.pop_back();
	if (variableSpaces.size() == 0) {
		assert(globalVariableSpace != nullptr);
		globalVariableSpace = nullptr;
	} else {
		currentVariableSpace = variableSpaces.back().get();
	}
}

object_t* Engine::_VariableLookup(const string& name) {
	assert(currentVariableSpace != nullptr);
	object_t* ptr = nullptr;
	ptr = currentVariableSpace->Lookup(name);
	if (ptr != nullptr)
		return ptr;

	ptr = globalVariableSpace->Lookup(name);
	if (ptr != nullptr)
		return ptr;

	ptr = currentVariableSpace->Define(name);
	assert(ptr != nullptr);
	return ptr;
}

object_t* Engine::_VariableAssign(const string& name, const object_t& object) {
	object_t* x = _VariableLookup(name);
	assert(x != nullptr);
	*x = object;
	return x;
}

object_t Engine::_Invoke(const string& name, const vector<object_t>& args) {
	ASTFuncDef** funcPtr = functions.Get(name);
	if (funcPtr == nullptr)
		return _InvokeCallback(name, args);

	ASTFuncDef* func = *funcPtr;
	_PushSpace();
	_PushScope();
	assert(func->NumParameters() == args.size());
	for (size_t i = 0; i < args.size(); ++i) {
		_VariableAssign(func->Parameter(i)->Name(), args[i]);
	}
	object_t result = Execute(func->Block().get());
	_PopScope();
	_PopSpace();
	return result;
}

object_t Engine::_InvokeCallback(const string& name, const vector<object_t>& args) {
	callback_t* x = callbacks->Get(name);
	assert(x != nullptr);
	assert(x->parameters == args.size());

	object_t ret;
	callbackFailure_t failure;
	if (!x->callback(args, &ret, &failure)) {
		assert(false);
	}
	return ret;
}

void Engine::_PopulateFunctions(ASTProgram* program) {
	for (size_t i = 0; i < program->NumChildren(); ++i) {
		if (program->Child(i)->Type() != AST_FUNC_DEF)
			continue;
		ASTFuncDef* func = (ASTFuncDef*)program->Child(i).get();
		functions.Put(func->Name(),func);
	}
}

bool Engine::Executing() const {
	return !(Test(F_HLT) || Test(F_BREAK) || Test(F_RETURN) || Test(F_EXCEPTION));
}

bool Engine::Test(flag_t flag) const {
	return (flags & flag) > 0 ? true : false;
}

void Engine::Set(flag_t flag) {
	uint16_t f = ((uint16_t)flag);
	flags = (flag_t)((uint16_t)flags | f);
}

void Engine::Clear(flag_t flag) {
	uint16_t f = ((uint16_t)flag) ^ 0xFFFF;
	flags = (flag_t)((uint16_t)flags & f);
}

VariableSpace::VariableSpace() {
	currentRegistry = nullptr;
}

object_t* VariableSpace::Lookup(const string& name) {
	assert(currentRegistry != nullptr);
	// Check current scope first
	object_t* objPtr = nullptr;
	// Go up the tree to the root
	int i = static_cast<int>(registries.size() - 1);
	for ( ; i >= 0; i--) {
		object_t* foundPtr = registries[i]->Get(name);
		if (foundPtr != nullptr) {
			objPtr = foundPtr;
			break;
		}
	}
	return objPtr;
}

object_t* VariableSpace::Define(const string& name) {
	object_t empty;
	empty.type = OT_NULL;
	currentRegistry->Put(name, empty);
	return currentRegistry->Get(name);
}

object_t* VariableSpace::Assign(const string& name, const object_t & value) {
	object_t* x = Lookup(name);
	assert(x != nullptr);
	*x = value;
	return x;
}

void VariableSpace::PushScope() {
	registries.push_back(new VariableRegistry());
	currentRegistry = registries.back().get();
}

void VariableSpace::PopScope() {
	registries.pop_back();
	if (registries.size() == 0) {
		assert(currentRegistry != nullptr);
		currentRegistry = nullptr;
	} else {
		currentRegistry = registries.back().get();
	}
}
