#include "AST.h"
#include "Parser.h"
#include "Dict.h"
#include "Symbol.h"
#include "Engine.h"

#include <Windows.h>

class ASTPostOrderPrinter {
protected:
	int indentation = 0;
	void PrintIndent() {
		for (int i = 0; i < indentation; ++i) {
			printf(" ");
		}
	}
public:

	void Print(ASTNode* node) {
		assert(node != nullptr);
		switch (node->Type()) {
			case AST_INT_LITERAL:
				Print((ASTIntLiteral*)node); break;
			case AST_ADD:
				Print((ASTAdd*)node); break;
			case AST_SUBTRACT:
				Print((ASTSubtract*)node); break;
			case AST_IDENTIFIER:
				Print((ASTIdentifier*)node); break;
			case AST_ASSIGN:
				Print((ASTAssign*)node); break;
			case AST_BLOCK:
				Print((ASTBlock*)node); break;
			case AST_CALL:
				Print((ASTCall*)node); break;
			case AST_FUNC_DEF:
				Print((ASTFuncDef*)node); break;
			case AST_PARAMETER:
				Print((ASTParameter*)node); break;
			case AST_INCREMENT:
				Print((ASTIncrement*)node); break;
			case AST_DECREMENT:
				Print((ASTDecrement*)node); break;
			case AST_IF:
				Print((ASTIf*)node); break;
			case AST_WHILE:
				Print((ASTWhile*)node); break;
			case AST_BREAK:
				Print((ASTBreak*)node); break;
			case AST_RETURN:
				Print((ASTReturn*)node); break;
			case AST_NOT:
				Print((ASTNot*)node); break;
			default:
				assert(false); break;
		}
	}
	
	void Print(ASTNot* node) {
		PrintIndent();
		printf("Not\n");
	}

	void Print(ASTReturn* node) {
		indentation++;
		Print(node->Expression().get());
		indentation--;
		PrintIndent();
		printf("Return\n");
	}

	void Print(ASTBreak* node) {
		PrintIndent();
		printf("Break\n");
	}

	void Print(ASTProgram* node) {
		indentation++;
		for (size_t i = 0; i < node->NumChildren(); ++i) {
			Print(node->Child(i).get());
		}
		printf("Program\n");
		indentation--;
	}

	void Print(ASTIntLiteral* node) {
		PrintIndent();
		printf("Int Literal %d\n", node->Value());
	}

	void Print(ASTIdentifier* node) {
		PrintIndent();
		printf("Identifier %s\n", node->Name().c_str());
	}

	void Print(ASTIncrement* node) {
		indentation++;
		Print(node->Child(0).get());
		indentation--;
		PrintIndent(); printf("Increment (++)\n");
	}

	void Print(ASTDecrement* node) {
		indentation++;
		Print(node->Child(0).get());
		indentation--;
		PrintIndent(); printf("Decrement (--)\n");
	}

	void Print(ASTIf* node) {
		indentation++;
		Print(node->Expression().get());
		Print(node->Statement().get());
		indentation--;
		PrintIndent(); printf("If\n");
	}

	void Print(ASTWhile* node) {
		indentation++;
		Print(node->Expression().get());
		Print(node->Statement().get());
		indentation--;
		PrintIndent(); printf("While\n");
	}

	void Print(ASTBlock* node) {
		indentation++;
		for (size_t i = 0; i < node->NumChildren(); ++i) {
			Print(node->Child(i).get());
		}
		indentation--;
		PrintIndent(); printf("Block\n");
	}

	void Print(ASTAssign* node) {
		indentation++;
		Print(node->Child(0).get());
		Print(node->Child(1).get());
		indentation--;
		PrintIndent();  printf("=\n");
	}

	void Print(ASTCall* node) {
		indentation++;
		for (size_t i = 0; i < node->NumChildren(); ++i) {
			Print(node->Child(i).get());
		}
		indentation--;
		PrintIndent(); printf("Call\n");
	}

	void Print(ASTFuncDef* node) {
		indentation++;
		for (size_t i = 0; i < node->NumChildren(); ++i) {
			Print(node->Child(i).get());
		}
		indentation--;
		PrintIndent(); printf("Function %s\n", node->Name().c_str());
	}

	void Print(ASTAdd* add) {
		indentation++;
		Print(add->Child(0).get());
		Print(add->Child(1).get());
		indentation--;
		PrintIndent(); printf("+\n");
	}

	void Print(ASTSubtract* add) {
		indentation++;
		Print(add->Child(0).get());
		Print(add->Child(1).get());
		indentation--;
		PrintIndent(); printf("-\n");
	}

	void Print(ASTParameter* param) {
		PrintIndent();
		printf("Parameter %s\n", param->Name().c_str());
	}
};

string GetFileText(const char* fileName) {
	string buf;
	FILE* p = nullptr;
	fopen_s(&p, fileName, "rt");
	if (p) {
		fseek(p, 0, SEEK_END);
		size_t size = ftell(p);
		fseek(p, 0, SEEK_SET);
		buf.resize(size);
		fread(&buf[0], 1, size, p);
		fclose(p);
	}
	return buf;
}

bool myPrint(const vector<object_t>& args, 
	object_t* ret, callbackFailure_t* failure) {

	printf("myPrint invoked ");
	for (size_t i = 0; i < args.size(); ++i) {
		printf("%d ", args[i].value._int);
	}
	printf("\n");
	return true;
}

bool mySleep(const vector<object_t>& args,
	object_t* ret, callbackFailure_t* failure) {

	if (args.size() < 1) {
		Sleep(1);
		return true;
	}

	int x = args[0].value._int;
	Sleep(x);
	return true;
}

int __cdecl main() {
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	/*Ref<Scope> s0 = new Scope();
	Ref<Scope> s1 = new Scope();
	Ref<Scope> s2 = new Scope();

	s1->Attach(s2);
	s0->Attach(s1);

	Ref<Symbol> sym = new VariableSymbol("X");
	s0->Define(sym);

	Ref<FunctionSymbol> myFun = new FunctionSymbol("test");
	Ref<Scope> innerScope = myFun->Inner();
	Ref<Symbol> m = new VariableSymbol("Y");
	innerScope->Define(m);
	s0->Define(myFun);

	Ref<Symbol> res = s2->Resolve("X");
	assert(res != nullptr);
	printf("%s\n", res->Name().c_str());

	res = s2->Resolve("test");
	assert(res != nullptr);
	printf("%s\n", res->Name().c_str());

	FunctionSymbol* fs = (FunctionSymbol*)res.get();
	assert(fs->Inner() != nullptr);
	printf("%s\n",fs->Inner()->Resolve("Y")->Name().c_str());
	printf("%s\n", fs->Inner()->Resolve("X")->Name().c_str());*/

	string example = GetFileText("example.wire");

	Parser parser(example.c_str());
	parseResult_t result = parser.Parse();

	if (!result.ast) {
		printf("AST is nullptr\n");
	}

	if (parser.HasError()) {
		parseError_t e = parser.Error();
		printf("Syntax error: %d %s\n", e.line + 1, e.details.c_str());
	} else {
		ASTPostOrderPrinter printer;
		printer.Print(result.ast.get());

		Engine engine;
		engine.DefineCallback("println", 1, myPrint);
		engine.DefineCallback("sleep", 1, mySleep);
		engine.Execute(result.ast.get());

		//int result = engine.Execute(root.get());
		//printf("Execution finished with result %d\n", result);
	}

	system("pause");
	return 0;
}