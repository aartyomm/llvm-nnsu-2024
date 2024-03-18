#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;

class PrintClassVisitor : public RecursiveASTVisitor<PrintClassVisitor> {
public:
  bool VisitCXXRecordDecl(CXXRecordDecl *declaration) {
    llvm::outs() << declaration->getNameAsString().c_str() << "\n";
    for (const auto &field : declaration->fields()) {
      llvm::outs() << " |_" << field->getNameAsString().c_str() << "\n";
    }
    return true;
  }
};

class PrintClassConsumer : public ASTConsumer {
public:
  explicit PrintClassConsumer(CompilerInstance &CI) : Visitor() {}

  void HandleTranslationUnit(ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  PrintClassVisitor Visitor;
};

class PrintClassASTAction : public PluginASTAction {
public:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &Compiler,
                    llvm::StringRef InFile) override {
    return std::make_unique<PrintClassConsumer>(Compiler);
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    if (!args.empty() && args[0] == "help") {
      PrintHelp(llvm::errs());
    }
    return true;
  }
  void PrintHelp(llvm::raw_ostream &ros) {
    ros << "#Help for the Clang \"PrintClassPlugin\" plugin\n\n"
        << "##Description\n"
        << "This clang plugin outputs the names of all classes(structures) and "
           "their fields in the C/C++ source file.\n\n"
        << "##Usage\n"
        << "To use the plugin, upload it to Clang using the following command "
           ":\n"
        << "clang -cc1 -load /path/to/plugin.so(.dll) -plugin -plugin "
           "print-class-plugin /path/to/source.cpp\n\n"
        << "##Output format\n"
        << "The plugin outputs the names of classes(structures) and fields in "
           "the following format :\n"
        << "NameClass\n"
        << "|_nameField\n\n"
        << "##Version\n"
        << "Version 1.1\n\n";
  }
};

static clang::FrontendPluginRegistry::Add<PrintClassASTAction>
    X("print-class-plugin",
      "A plugin that prints the names of classes (structures).");