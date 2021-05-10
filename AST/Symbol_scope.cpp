#include "Symbol.h"

Scope::Scope() : enclosing(nullptr) {
	children.clear();
}

Scope::~Scope() {
	enclosing = nullptr;
	children.clear();
}

IScope* Scope::Enclosing() {
	return enclosing;
}

void Scope::Define(Ref<Symbol> sym) {
	assert(sym != nullptr);
	const Ref<Symbol>* stored = symbols.Get(sym->Name());
	assert(stored == nullptr);
	symbols.Put(sym->Name(), sym);
	if (sym->Type() == ST_FUNCTION) {
		FunctionSymbol* fun = (FunctionSymbol*)sym.get();
		fun->enclosing = this;
	}
}

Ref<Symbol> Scope::Resolve(const string& name) {
	Ref<Symbol>* stored = symbols.Get(name);
	if (stored == nullptr && enclosing != nullptr) {
		return enclosing->Resolve(name);
	}
	return *stored;
}

void Scope::Attach(Ref<Scope>& child) {
	for (size_t i = 0; i < children.size(); ++i) {
		assert(children[i] != child.get());
	}
	child->enclosing = this;
	children.push_back(child);
}

size_t Scope::NumChildren() const {
	return children.size();
}

Ref<IScope> Scope::Child(size_t index) {
	return children[index];
}