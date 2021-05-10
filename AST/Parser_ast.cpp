#include "Parser_ast.h"

#ifdef _DEBUG
//#if 0
#define DEBUG_TRACE(X) {printf(X); printf("\n");}
#define DEBUG_TRACE_FMT(X,Y) {printf(X,Y); printf("\n");}
#else
#define DEBUG_TRACE(X)
#define DEBUG_TRACE_FMT(X,Y)
#endif

Parser_AST::Parser_AST() {
	program = nullptr;
	statement = nullptr;
	block = nullptr;
	assign = nullptr;
	add = nullptr;
	lhs = nullptr;
	call = nullptr;
	primary = nullptr;
	ctrlIf = nullptr;
	ctrlBreak = nullptr;
	unary = nullptr;
	functionIdentifier = nullptr;
}

Parser_AST::~Parser_AST() {
}

Ref<ASTProgram> Parser_AST::AST() {
	return program;
}

void Parser_AST::Speculate(bool val) {
	speculative = val;
}

void Parser_AST::MakePrimaryFromIdentifier(const string& value) {
	if (speculative)
		return;
	DEBUG_TRACE_FMT("MakePrimaryFromIdentifier (%s)",value.c_str());
	primary = new ASTIdentifier(value);
}

void Parser_AST::MakePrimaryFromDecIntLiteral(const string& value) {
	if (speculative)
		return;
	DEBUG_TRACE_FMT("MakePrimaryFromIntLiteral (%s)",value.c_str());
	primary = new ASTIntLiteral(value);
}

void Parser_AST::MakePrimaryFromExpression() {
	if (speculative)
		return;
	DEBUG_TRACE("MakePrimaryFromExpression");
	primary = expression;
}

void Parser_AST::PushCallIdentifier(const string& value) {
	if (speculative)
		return;
	DEBUG_TRACE_FMT("CallIdentifier (%s)",value.c_str());
	callIdentifierStack.push_back(new ASTIdentifier(value));
}

void Parser_AST::PushCallArgumentFromAssign() {
	if (speculative)
		return;
	DEBUG_TRACE("CallArgumentFromAssign");
	assert(assign != nullptr);
	callArgumentStack.push_back(assign);
	assign = nullptr;
}

void Parser_AST::PushCallArgumentBoundary() {
	if (speculative)
		return;
	DEBUG_TRACE("PushCallArgumentBoundary");
	callArgumentBoundaries.push_back(callArgumentStack.size());
}

void Parser_AST::PopCallArgumentBoundary() {
	if (speculative)
		return;
	DEBUG_TRACE("PopCallArgumentBoundary");
	callArgumentBoundaries.pop_back();
}

void Parser_AST::MakeCall() {
	if (speculative)
		return;
	DEBUG_TRACE("MakeCall");

	size_t numArgs = callArgumentStack.size() - callArgumentBoundaries.back();
	callArgumentBoundaries.pop_back();

	ASTIdentifierRef identifier = callIdentifierStack.back();
	callIdentifierStack.pop_back();

	call = new ASTCall(identifier);
	for (size_t i = 0; i < numArgs; ++i) {
		call->AttachChild(callArgumentStack.back());
		callArgumentStack.pop_back();
	}
}

void Parser_AST::FunctionIdentifier(const string & value) {
	if (speculative)
		return;
	DEBUG_TRACE("FunctionIdentifier");
	assert(functionIdentifier == nullptr);
	functionIdentifier = new ASTIdentifier(value);
}

void Parser_AST::PushFunctionParameter(const string & value) {
	if (speculative)
		return;
	DEBUG_TRACE("FunctionParameter");
	functionParameters.push_back(new ASTParameter(value));
}

void Parser_AST::FunctionBlockFromBlock() {
	if (speculative)
		return;
	DEBUG_TRACE("FunctionBlockFromBlock");
	functionBlock = block;
	block = nullptr;
}

void Parser_AST::MakeFunction() {
	if (speculative)
		return;
	DEBUG_TRACE("MakeFunction");

	function = new ASTFuncDef(functionIdentifier->Name(), functionBlock);
	for (size_t i = 0; i < functionParameters.size(); ++i) {
		function->AttachParameter(functionParameters[i]);
	}

	functionParameters.clear();
	functionIdentifier = nullptr;
}

void Parser_AST::MakeLhsFromPrimary() {
	if (speculative)
		return;
	DEBUG_TRACE("MakeLhsFromPrimary");
	assert(primary != nullptr);
	lhs = primary;
	primary = nullptr;
}

void Parser_AST::MakeLhsFromCall() {
	if (speculative)
		return;
	DEBUG_TRACE("MakeLhsFromCall");
	assert(call != nullptr);
	lhs = call;
	call = nullptr;
}

void Parser_AST::MakePostfixFromLhs() {
	if (speculative)
		return;
	DEBUG_TRACE("MakePostfixFromLhs");
	assert(lhs != nullptr);
	postfix = lhs;
	lhs = nullptr;
}

void Parser_AST::MakePostfixDecrementFromLhs() {
	if (speculative)
		return;
	DEBUG_TRACE("MakePostfixIncrementFromLhs");
	assert(lhs != nullptr);
	postfix = new ASTDecrement(lhs);
	lhs = nullptr;
}

void Parser_AST::MakePostfixIncrementFromLhs() {
	if (speculative)
		return;
	DEBUG_TRACE("MakePostfixIncrementFromLhs");
	assert(lhs != nullptr);
	postfix = new ASTIncrement(lhs);
	lhs = nullptr;
}

void Parser_AST::PushAddTermFromUnary() {
	if (speculative)
		return;
	DEBUG_TRACE("PushAddTermFromUnary");
	assert(unary != nullptr);
	addTermStack.push_back(unary);
	unary = nullptr;
}

void Parser_AST::PushAddTermBoundary() {
	if (speculative)
		return;
	DEBUG_TRACE("PushAddTermBoundary");
	addTermBoundaries.push_back(addTermStack.size());
}

void Parser_AST::PopAddTermBoundary() {
	if (speculative)
		return;
	DEBUG_TRACE("PopAddTermBoundary");
	addTermBoundaries.pop_back();
}

void Parser_AST::PushAddTokenType(const tokenType_t& token) {
	if (speculative)
		return;
	DEBUG_TRACE("PushAddToken");
	addTokenStack.push_back(token);
}

void Parser_AST::MakeAdd() {
	if (speculative)
		return;
	DEBUG_TRACE("MakeAdd");

	size_t boundary = addTermBoundaries.back();
	addTermBoundaries.pop_back();

	size_t numTerms = addTermStack.size() - boundary;
	size_t numOps = numTerms - 1;

	if (numTerms == 1) {
		add = addTermStack.back(); addTermStack.pop_back();
	} else if (numTerms >= 2) {

		auto NewAdd = [](ASTNodeRef a, ASTNodeRef b, tokenType_t type) -> ASTNodeRef {
			assert(type == TOK_PLUS || type == TOK_MINUS);
			if (type == TOK_PLUS) return new ASTAdd(a, b);
			if (type == TOK_MINUS) return new ASTSubtract(a, b);
			assert(false);
			return nullptr;
		};

		ASTNodeRef addNode = addTermStack[boundary];
		for (size_t i = 0; i < numOps; ++i) {
			ASTNodeRef b = addTermStack[boundary + i + 1];
			addNode = NewAdd(addNode,b,addTokenStack[boundary + i]);
		}

		add = addNode;

		// Clear stacks
		for (size_t i = 0; i < numTerms; ++i)
			addTermStack.pop_back();

		for (size_t i = 0; i < numOps; ++i)
			addTokenStack.pop_back();
	}
}

void Parser_AST::PushAssignLhsFromAdd() {
	if (speculative)
		return;
	DEBUG_TRACE("PushAssignLhsFromAdd");
	assert(add != nullptr);
	assignLhsStack.push_back(add);
	add = nullptr;
}

void Parser_AST::PushAssignLhsFromLhs() {
	if (speculative)
		return;
	DEBUG_TRACE("PushAssignLhsFromLhs");
	assert(lhs != nullptr);
	assignLhsStack.push_back(lhs);
	lhs = nullptr;
}

void Parser_AST::PushAssignLhsBoundary() {
	if (speculative)
		return;
	DEBUG_TRACE("PushAssignLhsBoundary");
	assignLhsBoundaries.push_back(assignLhsStack.size());
}

void Parser_AST::PopAssignLhsBoundary() {
	if (speculative)
		return;
	DEBUG_TRACE("PopAssignLhsBoundary");
	assignLhsBoundaries.pop_back();
}

void Parser_AST::MakeAssign() {
	if (speculative)
		return;
	DEBUG_TRACE("MakeAssign");

	size_t numAssigns = assignLhsStack.size() - assignLhsBoundaries.back();
	assignLhsBoundaries.pop_back();

	if (numAssigns == 1) {
		assign = assignLhsStack.back();
		assignLhsStack.pop_back();
	} else if (numAssigns >= 2) {
		ASTNodeRef b = assignLhsStack.back(); assignLhsStack.pop_back();
		ASTNodeRef a = assignLhsStack.back(); assignLhsStack.pop_back();
		ASTAssignRef assignNode = new ASTAssign(a, b);
		for (size_t i = 0; i < numAssigns - 2; ++i) {
			ASTNodeRef x = assignLhsStack.back(); assignLhsStack.pop_back();
			assignNode = new ASTAssign(x, assignNode);
		}
		assign = assignNode;
	}
}

void Parser_AST::MakeExpressionFromAssign() {
	if (speculative)
		return;
	DEBUG_TRACE("MakeExpressionFromAssign");
	assert(assign != nullptr);
	expression = assign;
	assign = nullptr;
}

void Parser_AST::MakeExpression() {
	if (speculative)
		return;
	// Forward
	MakeExpressionFromAssign();
}

void Parser_AST::ReturnExpressionFromExpression() {
	if (speculative)
		return;
	DEBUG_TRACE("ReturnExpressionFromExpression");
	assert(expression != nullptr);
	ctrlReturnExpression = expression;
	expression = nullptr;
}

void Parser_AST::MakeReturn() {
	if (speculative)
		return;
	DEBUG_TRACE("MakeReturn");
	assert(ctrlReturnExpression != nullptr);
	ctrlReturn = new ASTReturn(ctrlReturnExpression);
	ctrlReturnExpression = nullptr;
}

void Parser_AST::MakeBreak() {
	if (speculative)
		return;
	DEBUG_TRACE("MakeBreak");
	ctrlBreak = new ASTBreak();
}

void Parser_AST::MakeStatementFromBreak() {
	if (speculative)
		return;
	DEBUG_TRACE("MakeStatementFromBreak");
	assert(ctrlBreak != nullptr);
	statement = ctrlBreak;
	ctrlBreak = nullptr;
}

void Parser_AST::MakeStatementFromExpression() {
	if (speculative)
		return;
	DEBUG_TRACE("MakeStatementFromExpression");
	assert(expression != nullptr);
	statement = expression;
	expression = nullptr;
}

void Parser_AST::MakeStatementFromFunction() {
	if (speculative)
		return;
	DEBUG_TRACE("MakeStatementFromFunction");
	assert(function != nullptr);
	statement = function;
	function = nullptr;
}

void Parser_AST::MakeStatementFromBlock() {
	if (speculative)
		return;
	DEBUG_TRACE("MakeStatementFromBlock");
	assert(block != nullptr);
	statement = block;
	block = nullptr;
}


void Parser_AST::MakeStatementFromIf() {
	if (speculative)
		return;
	DEBUG_TRACE("MakeStatementFromIf");
	assert(ctrlIf != nullptr);
	statement = ctrlIf;
	ctrlIf = nullptr;
}

void Parser_AST::MakeStatementFromWhile() {
	if (speculative)
		return;
	DEBUG_TRACE("MakeStatementFromWhile");
	assert(ctrlWhile != nullptr);
	statement = ctrlWhile;
	ctrlWhile = nullptr;
}

void Parser_AST::MakeStatementFromReturn() {
	if (speculative)
		return;
	DEBUG_TRACE("MakeStatementFromReturn");
	assert(ctrlReturn != nullptr);
	statement = ctrlReturn;
	ctrlReturn = nullptr;
}

void Parser_AST::PushBlockStatementFromStatement() {
	if (speculative)
		return;
	DEBUG_TRACE("BlockStatementFromStatement");
	assert(statement != nullptr);
	blockStatements.push_back(statement);
	statement = nullptr;
}

void Parser_AST::PushBlockStatementBoundary() {
	if (speculative)
		return;
	DEBUG_TRACE("PushBlockStatementBoundary");
	blockBoundaries.push_back(blockStatements.size());
}

void Parser_AST::PopBlockStatementBoundary() {
	if (speculative)
		return;
	DEBUG_TRACE("PopBlockStatementBoundary");
	blockBoundaries.pop_back();
}

void Parser_AST::MakeBlock() {
	if (speculative)
		return;
	DEBUG_TRACE("MakeBlock");

	size_t boundary = blockBoundaries.back();
	blockBoundaries.pop_back();

	size_t numStatements = blockStatements.size() - boundary;

	block = new ASTBlock();
	for (size_t i = 0; i < numStatements; ++i) {
		block->AttachChild(blockStatements[boundary + i]);
	}

	// Remove numStatements
	for (size_t i = 0; i < numStatements; ++i)
		blockStatements.pop_back();
}

void Parser_AST::ProgramStatementFromStatement() {
	if (speculative)
		return;
	DEBUG_TRACE("ProgramStatementFromStatement");
	assert(statement != nullptr);
	programStatements.push_back(statement);
	statement = nullptr;
}

void Parser_AST::MakeProgram() {
	if (speculative)
		return;
	DEBUG_TRACE("MakeProgram");
	program = new ASTProgram();
	for (size_t i = 0; i < programStatements.size(); ++i) {
		program->AttachChild(programStatements[i]);
	}
	programStatements.clear();
	assert(assignLhsBoundaries.size() == 0);
	assert(callArgumentBoundaries.size() == 0);
	assert(blockBoundaries.size() == 0);
	assert(addTermBoundaries.size() == 0);
}

void Parser_AST::PushIfExpressionFromExpression() {
	if (speculative)
		return;
	DEBUG_TRACE("PushIfExpressionFromExpression");
	assert(expression != nullptr);
	ctrlIfExpressionStack.push_back(expression);
	expression = nullptr;
}

void Parser_AST::PushIfStatementFromStatement() {
	if (speculative)
		return;
	DEBUG_TRACE("PushIfStatementFromStatement");
	assert(statement != nullptr);
	ctrlIfStatementStack.push_back(statement);
	statement = nullptr;
}

void Parser_AST::MakeIf() {
	if (speculative)
		return;
	DEBUG_TRACE("MakeIf");

	ASTNodeRef expr = ctrlIfExpressionStack.back();
	ctrlIfExpressionStack.pop_back();

	ASTNodeRef stat = ctrlIfStatementStack.back();
	ctrlIfStatementStack.pop_back();

	ctrlIf = new ASTIf(expr, stat);
}

void Parser_AST::PushWhileExpressionFromExpression() {
	if (speculative)
		return;
	DEBUG_TRACE("PushWhileExpressionFromExpression");
	assert(expression != nullptr);
	ctrlWhileExpressionStack.push_back(expression);
	expression = nullptr;
}

void Parser_AST::PushWhileStatementFromStatement() {
	if (speculative)
		return;
	DEBUG_TRACE("PushWhileStatementFromStatement");
	assert(statement != nullptr);
	ctrlWhileStatementStack.push_back(statement);
	statement = nullptr;
}

void Parser_AST::MakeWhile() {
	if (speculative)
		return;
	DEBUG_TRACE("MakeWhile");

	ASTNodeRef expr = ctrlWhileExpressionStack.back();
	ctrlWhileExpressionStack.pop_back();

	ASTNodeRef stat = ctrlWhileStatementStack.back();
	ctrlWhileStatementStack.pop_back();

	ctrlWhile = new ASTWhile(expr, stat);
}

void Parser_AST::MakeUnaryFromPostfix() {
	if (speculative)
		return;
	DEBUG_TRACE("MakeUnaryFromPostfix");
	assert(postfix != nullptr);
	unary = postfix;
	postfix = nullptr;
}

void Parser_AST::MakeUnaryNotFromPostfix() {
	if (speculative)
		return;
	DEBUG_TRACE("MakeUnaryNotFromPostfix");
	assert(postfix != nullptr);
	unary = new ASTNot(postfix);
	postfix = nullptr;
}