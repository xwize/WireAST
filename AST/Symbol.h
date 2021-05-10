#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include "Common.h"
#include "Dict.h"
#include "Ref.h"

enum symbolType_t {
	ST_VARIABLE,
	ST_FUNCTION
};

class Symbol : public virtual RefObject {
protected:
	string						name;
	symbolType_t				type;
protected:
								Symbol(string nm, symbolType_t tp);
public:
	virtual						~Symbol();
	virtual string				Name() const;
	virtual symbolType_t		Type() const;
};

class IScope : public virtual RefObject {
public:
	virtual						~IScope() {}
	virtual IScope*				Enclosing() = 0;
	virtual void				Define(Ref<Symbol> sym) = 0;
	virtual Ref<Symbol>			Resolve(const string& name) = 0;
};

class Scope : public IScope {
protected:
	friend class FunctionSymbol;
	typedef Ref<IScope> IScopeRef;
	Dict<string,Ref<Symbol>>	symbols;
	vector<IScopeRef>			children;
	IScope*						enclosing;
public:
								Scope();
								~Scope();

	IScope*						Enclosing()					override;
	void						Define(Ref<Symbol> sym)		override;
	Ref<Symbol>					Resolve(const string& name)	override;

	void						Attach(Ref<Scope>& child);
	size_t						NumChildren() const;
	Ref<IScope>					Child(size_t index);
};

class VariableSymbol : public Symbol {
public:
								VariableSymbol(string name);
								~VariableSymbol();
};

class FunctionSymbol : public IScope, public Symbol {
	friend class Scope;
protected:
	vector<Ref<Symbol>>			parameters;
	Ref<Scope>					inner;
	IScope*						enclosing;
public:
								FunctionSymbol(string name);
								~FunctionSymbol();

	IScope*						Enclosing()					override;
	void						Define(Ref<Symbol> sym)		override;
	Ref<Symbol>					Resolve(const string& name)	override;

	bool						IsParameter(const string& name);
	Ref<Scope>					Inner();
};

#endif // __SYMBOL_H__
