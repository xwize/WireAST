#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "Common.h"
#include "AST.h"
#include "Dict.h"
#include "Symbol.h"

enum objectType_t {
	OT_INTEGER,
	OT_STRING,
	OT_FUNCTION_REF,
	OT_NULL,
	OT_VOID
};

enum runtimeErrorCode_t {
	RT_ERR_NONE
};

struct objectValue_t {
	int 	_int;
	string 	_string;
};

struct object_t {
	objectType_t	type;
	objectValue_t	value;
};

struct callbackFailure_t {
	int		code;
	string	info;
};

typedef bool(*callbackFunction_t)(
	const vector<object_t>& args,
	object_t* ret, callbackFailure_t* failure);

struct callback_t {
	string				name;
	size_t				parameters;
	callbackFunction_t	callback;
};

class CallbackRegistry : public virtual RefObject,
	public Dict<string, callback_t> {
};

class VariableRegistry : public virtual RefObject,
	public Dict<string, object_t> {
};

typedef Ref<CallbackRegistry> CallbackRegistryRef;
typedef Ref<VariableRegistry> VariableRegistryRef;

class VariableSpace : public virtual RefObject {
private:
	vector<VariableRegistryRef> registries;
	VariableRegistry* currentRegistry;
public:
	VariableSpace();
	object_t*	Lookup(const string& name);
	object_t*	Assign(const string& name, const object_t& value);
	object_t*	Define(const string& name);
	void		PushScope();
	void		PopScope();
};

typedef Ref<VariableSpace> VariableSpaceRef;

enum flag_t {
	F_NONE		= 0x00,
	F_HLT		= 0x01,
	F_BREAK		= 0x02,
	F_RETURN	= 0x04,
	F_EXCEPTION = 0x08
};

class Engine {
protected:
	vector<VariableSpaceRef>	variableSpaces;
	CallbackRegistryRef			callbacks;
	Dict<string, ASTFuncDef*>	functions;
	VariableSpace*				globalVariableSpace;
	VariableSpace*				currentVariableSpace;
	flag_t						flags;
protected:
	bool Executing() const;
	bool Test(flag_t flag) const;
	void Set(flag_t flag);
	void Clear(flag_t flag);
protected:
	void _PushScope();
	void _PopScope();
	object_t*	_VariableAssign(const string& name, const object_t& value);
	object_t*	_VariableLookup(const string& name);
	void _PushSpace();
	void _PopSpace();
	object_t	_Invoke(const string& name, const vector<object_t>& args);
	object_t	_InvokeCallback(const string& name, const vector<object_t>& args);
	void _PopulateFunctions(ASTProgram* program);
protected:
	object_t Execute(ASTNode* node);
	object_t Execute(ASTAssign* node);
	object_t Execute(ASTIntLiteral* node);
	object_t Execute(ASTIdentifier* node);
	object_t Execute(ASTCall* node);
	object_t Execute(ASTBlock* node);
	object_t Execute(ASTAdd* node);
	object_t Execute(ASTSubtract* node);
	object_t Execute(ASTIncrement* node);
	object_t Execute(ASTDecrement* node);
	object_t Execute(ASTIf* node);
	object_t Execute(ASTWhile* node);
	object_t Execute(ASTBreak* node);
	object_t Execute(ASTReturn* node);
	object_t Execute(ASTNot* node);
public:
	Engine();
	~Engine();

	void DefineCallback(const string& name, size_t numParams, callbackFunction_t func);
	void DefineCallback(const callback_t& callback);
	void Execute(ASTProgram* program);
};

#endif // __ENGINE_H__
