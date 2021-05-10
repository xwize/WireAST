#ifndef __AST_H__
#define __AST_H__

#include "Common.h"
#include "Ref.h"

enum astNodeType_t {
	AST_PROGRAM,
	AST_INT_LITERAL,
	AST_IDENTIFIER,
	AST_ASSIGN,
	AST_BLOCK,
	AST_PARAMETER,
	AST_FUNC_DEF,
	AST_CALL,
	AST_ADD,
	AST_SUBTRACT,
	AST_INCREMENT,
	AST_DECREMENT,
	AST_NOT,
	AST_BREAK,
	AST_RETURN,
	AST_IF,
	AST_WHILE
};

class ASTNode : public virtual RefObject {
private:
	typedef Ref<ASTNode> AstNodeRef;
	astNodeType_t			type;
	vector<AstNodeRef>		children;
protected:
	ASTNode() {
	}
	void _Attach(const AstNodeRef& child) {
		assert(child != nullptr);
		children.push_back(child);
	}
public:
	ASTNode(astNodeType_t tp) : type(tp) {
	}

	virtual ~ASTNode() {
	}

	astNodeType_t Type() const {
		return type;
	}

	void Clear() {
		children.clear();
	}

	size_t NumChildren() const {
		return children.size();
	}

	Ref<ASTNode> Child(size_t index) const {
		return children[index];
	}
};

class ASTIntLiteral : public ASTNode {
private:
	int literal;
public:
	ASTIntLiteral(string value) : ASTNode(AST_INT_LITERAL) {
		literal = atoi(value.c_str());
	}

	ASTIntLiteral(int value) : ASTNode(AST_INT_LITERAL) {
		literal = value;
	}

	void SetValue(int v) {
		literal = v;
	}

	int Value() const {
		return literal;
	}
};

class ASTIdentifier : public ASTNode {
private:
	string name;
public:
	ASTIdentifier(string value) : ASTNode(AST_IDENTIFIER) {
		name = value;
	}

	string Name() const {
		return name;
	}
};

class ASTAdd : public ASTNode {
public:
	ASTAdd(Ref<ASTNode> a, Ref<ASTNode> b) : ASTNode(AST_ADD) {
		assert(a->Type() == AST_IDENTIFIER || a->Type() == AST_INT_LITERAL ||
			a->Type() == AST_ADD || a->Type() == AST_SUBTRACT || a->Type() == AST_CALL);

		assert(b->Type() == AST_IDENTIFIER || b->Type() == AST_INT_LITERAL ||
			b->Type() == AST_ADD || b->Type() == AST_SUBTRACT || b->Type() == AST_CALL);

		_Attach(a);
		_Attach(b);
	}
};

class ASTSubtract : public ASTNode {
public:
	ASTSubtract(Ref<ASTNode> a, Ref<ASTNode> b) : ASTNode(AST_SUBTRACT) {
		assert(a->Type() == AST_IDENTIFIER || a->Type() == AST_INT_LITERAL ||
			a->Type() == AST_ADD || a->Type() == AST_SUBTRACT || a->Type() == AST_CALL);

		assert(b->Type() == AST_IDENTIFIER || b->Type() == AST_INT_LITERAL ||
			b->Type() == AST_ADD || b->Type() == AST_SUBTRACT || b->Type() == AST_CALL);

		_Attach(a);
		_Attach(b);
	}
};

class ASTIncrement : public ASTNode {
public:
	ASTIncrement(Ref<ASTNode> a) : ASTNode(AST_INCREMENT) {
		_Attach(a);
	}
};

class ASTDecrement : public ASTNode {
public:
	ASTDecrement(Ref<ASTNode> a) : ASTNode(AST_DECREMENT) {
		_Attach(a);
	}
};

class ASTIf : public ASTNode {
public:
	ASTIf(Ref<ASTNode> expr, Ref<ASTNode> stat) : ASTNode(AST_IF) {
		_Attach(expr);
		_Attach(stat);
	}

	inline Ref<ASTNode> Expression() const {
		return Child(0).get();
	}

	inline Ref<ASTNode> Statement() const {
		return Child(1).get();
	}
};


class ASTWhile : public ASTNode {
public:
	ASTWhile(Ref<ASTNode> expr, Ref<ASTNode> stat) : ASTNode(AST_WHILE) {
		_Attach(expr);
		_Attach(stat);
	}

	inline Ref<ASTNode> Expression() const {
		return Child(0).get();
	}

	inline Ref<ASTNode> Statement() const {
		return Child(1).get();
	}
};


class ASTAssign : public ASTNode {
public:
	ASTAssign(Ref<ASTNode> a, Ref<ASTNode> b) : ASTNode(AST_ASSIGN) {
		assert(a->Type() == AST_IDENTIFIER);
		_Attach(a);
		assert(
			b->Type() == AST_IDENTIFIER ||
			b->Type() == AST_INT_LITERAL ||
			b->Type() == AST_ADD ||
			b->Type() == AST_SUBTRACT ||
			b->Type() == AST_NOT ||
			b->Type() == AST_ASSIGN ||
			b->Type() == AST_CALL || 
			b->Type() == AST_INCREMENT
		);
		_Attach(b);
	}

	ASTAssign(Ref<ASTIdentifier> a, Ref<ASTNode> b) : 
		ASTAssign((ASTNode*)a.get(),b) {
	}

	inline Ref<ASTIdentifier> LHS() const {
		return (ASTIdentifier*)Child(0).get();
	}

	inline Ref<ASTNode> RHS() const {
		return Child(1);
	}
};

class ASTNot : public ASTNode {
public:
	ASTNot(Ref<ASTNode> a) : ASTNode(AST_NOT) {
		_Attach(a);
	}

	inline Ref<ASTNode> Expression() const {
		return (ASTNode*)Child(0).get();
	}
};

class ASTCall : public ASTNode {
public:
	ASTCall(Ref<ASTIdentifier> a) : ASTNode(AST_CALL) {
		_Attach(a);
	}

	void AttachChild(Ref<ASTNode> node) {
		_Attach(node);
	}

	inline Ref<ASTIdentifier> Identifier() const {
		return (ASTIdentifier*)Child(0).get();
	}

	inline Ref<ASTNode> Argument(size_t index) const {
		return Child(1 + index);
	}

	inline size_t NumArguments() const {
		return NumChildren() - 1;
	}
};

class ASTParameter : public ASTNode {
	string name;
public:
	ASTParameter(string value) : ASTNode(AST_PARAMETER) {
		name = value;
	}

	ASTParameter(Ref<ASTIdentifier> value) : ASTNode(AST_PARAMETER) {
		name = value->Name();
	}

	string Name() const {
		return name;
	}
};

class ASTBreak : public ASTNode {
public:
	ASTBreak() : ASTNode(AST_BREAK) {
	}
};


class ASTReturn : public ASTNode {
public:
	ASTReturn(Ref<ASTNode> node) : ASTNode(AST_RETURN) {
		_Attach(node);
	}

	inline Ref<ASTNode> Expression() const {
		return Child(0).get();
	}
};

class ASTBlock : public ASTNode {
public:
	ASTBlock() : ASTNode(AST_BLOCK) {
	}

	void AttachChild(Ref<ASTNode> node) {
		_Attach(node);
	}
};

class ASTFuncDef : public ASTNode {
	string name;
public:
	ASTFuncDef(const string& value, const Ref<ASTBlock>& block) : ASTNode(AST_FUNC_DEF) {
		name = value;
		_Attach(block);
	}

	void AttachParameter(const Ref<ASTParameter>& param) {
		_Attach(param);
	}

	string Name() const {
		return name;
	}

	inline Ref<ASTBlock> Block() const {
		return (ASTBlock*)Child(0).get();
	}

	inline Ref<ASTParameter> Parameter(size_t index) const {
		return (ASTParameter*)Child(1 + index).get();
	}

	inline size_t NumParameters() const {
		return NumChildren() - 1;
	}
};

class ASTProgram : public ASTNode {
public:
	ASTProgram() : ASTNode(AST_PROGRAM) {
	}

	void AttachChild(Ref<ASTNode> node) {
		_Attach(node);
	}
};

typedef Ref<ASTNode>		ASTNodeRef;
typedef Ref<ASTAdd>			ASTAddRef;
typedef Ref<ASTIdentifier>	ASTIdentifierRef;
typedef Ref<ASTIntLiteral>	ASTIntLiteralRef;
typedef Ref<ASTProgram>		ASTProgramRef;
typedef Ref<ASTBlock>		ASTBlockRef;
typedef Ref<ASTAssign>		ASTAssignRef;
typedef Ref<ASTCall>		ASTCallRef;
typedef Ref<ASTFuncDef>		ASTFuncDefRef;
typedef Ref<ASTParameter>	ASTParameterRef;
typedef Ref<ASTBreak>		ASTBreakRef;
typedef Ref<ASTReturn>		ASTReturnRef;

#endif // __AST_H__
