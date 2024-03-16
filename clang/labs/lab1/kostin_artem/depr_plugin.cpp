#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;

class CustomNodeVisitor : public RecursiveASTVisitor<CustomNodeVisitor> {
public:
  bool VisitFunctionDecl(FunctionDecl *pfunction) {
    std::string nameOfFunction = pfunction->getNameInfo().getAsString();
    std::transform(nameOfFunction.begin(), nameOfFunction.end(),
                   nameOfFunction.begin(), ::tolower);
    if (nameOfFunction.find("deprecated") != std::string::npos) {
      DiagnosticsEngine &diagnostics =
          pfunction->getASTContext().getDiagnostics();
      unsigned int diagnostics_id = diagnostics.getCustomDiagID(
          DiagnosticsEngine::Warning,
          "The function name contains \"deprecated\"");
      SourceLocation position_of_function = pfunction->getLocation();
      diagnostics.Report(position_of_function, diagnostics_id)
          << nameOfFunction;
    }
    return true;
  }
};

class CustomConsumer : public ASTConsumer {
public:
  void HandleTranslationUnit(ASTContext &Context) override {
    CustomNodeVisitor cnv;
    cnv.TraverseDecl(Context.getTranslationUnitDecl());
  }
};

class PluginDeprFunc : public PluginASTAction {
  std::unique_ptr<ASTConsumer>
  CreateASTConsumer(CompilerInstance &Compiler,
                    llvm::StringRef InFile) override {
    return std::make_unique<CustomConsumer>();
  }
  bool ParseArgs(const CompilerInstance &Compiler,
                 const std::vector<std::string> &args) override {
    return true;
  }
};

static FrontendPluginRegistry::Add<PluginDeprFunc>
    X("plugin_for_deprecated_functions",
      "If the function name contains \"deprecated\" plugin writes a warning");
