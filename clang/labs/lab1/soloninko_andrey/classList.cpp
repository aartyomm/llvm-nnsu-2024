#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/Support/raw_ostream.h"

class PrintClass : public clang::RecursiveASTVisitor<PrintClass> {
public:
  bool VisitCXXRecordDecl(clang::CXXRecordDecl *declaration) {
    if (declaration->isClass() || declaration->isStruct()) {
      llvm::outs() << declaration->getNameAsString() << "\n";
      for (auto field_member : declaration->fields()) {
        llvm::outs() << "|_" << field_member->getNameAsString() << "\n";
      }
      llvm::outs() << "\n";
    }
    return true;
  }
};

class MyASTConsumer : public clang::ASTConsumer {
public:
  PrintClass Visitor;
  void HandleTranslationUnit(clang::ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
};

class PrintClassAction : public clang::PluginASTAction {
public:
  std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance &ci,
                                                 llvm::StringRef) override {
    return std::make_unique<MyASTConsumer>();
  }

  bool ParseArgs(const clang::CompilerInstance &ci,
                 const std::vector<std::string> &args) override {
    return true;
  }
};

static clang::FrontendPluginRegistry::Add<PrintClassAction>
    X("class_list_plugin", "Prints all members of the class");