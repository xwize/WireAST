#include "Symbol.h"

Symbol::Symbol(string nm, symbolType_t tp) :
	name(nm), type(tp) {
}

Symbol::~Symbol() {
}

string Symbol::Name() const {
	return name;
}

symbolType_t Symbol::Type() const {
	return type;
}

FunctionSymbol::FunctionSymbol(string name) :
	Symbol(name,ST_FUNCTION), enclosing(nullptr) {
	parameters.clear();

	inner = new Scope();
	inner->enclosing = this;
}

FunctionSymbol::~FunctionSymbol() {
}

IScope* FunctionSymbol::Enclosing() {
	return enclosing;
}

void FunctionSymbol::Define(Ref<Symbol> sym) {
	assert(!IsParameter(sym->Name()));
	parameters.push_back(sym);
}

Ref<Symbol> FunctionSymbol::Resolve(const string& name) {
	for (size_t i = 0; i < parameters.size(); ++i) {
		if (parameters[i]->Name() == name) {
			return parameters[i];
		}
	}

	if (enclosing != nullptr) {
		return enclosing->Resolve(name);
	}

	return nullptr;
}

Ref<Scope> FunctionSymbol::Inner() {
	return inner;
}

bool FunctionSymbol::IsParameter(const string& name) {
	for (size_t i = 0; i < parameters.size(); ++i) {
		if (parameters[i]->Name() == name) {
			return true;
		}
	}
	return false;
}

VariableSymbol::VariableSymbol(string name) :
	Symbol(name,ST_VARIABLE){
}

VariableSymbol::~VariableSymbol() {
}
