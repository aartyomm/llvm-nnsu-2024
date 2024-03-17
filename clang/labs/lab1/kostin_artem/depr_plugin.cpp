#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;

class CustomNodeVisitor : public RecursiveASTVisitor<CustomNodeVisitor> {
  bool caseInsensitive;

public:
  CustomNodeVisitor(bool _caseInsensitive) : caseInsensitive(_caseInsensitive) {}
  bool VisitFunctionDecl(FunctionDecl *pfunction) {
    std::string nameOfFunction = pfunction->getNameInfo().getAsString();
    if (caseInsensitive) {
      std::transform(nameOfFunction.begin(), nameOfFunction.end(),
                     nameOfFunction.begin(), ::tolower);
    }
    if (nameOfFunction.find("deprecated") != std::string::npos) {
      DiagnosticsEngine &diagnostics = pfunction->getASTContext().getDiagnostics();
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
  bool caseInsensitive;

public:
  explicit CustomConsumer(bool _caseInsensitive) : caseInsensitive(_caseInsensitive) {}
  void HandleTranslationUnit(ASTContext &Context) override {
    CustomNodeVisitor cnv(caseInsensitive);
    cnv.TraverseDecl(Context.getTranslationUnitDecl());
  }
};

class PluginDeprFunc : public PluginASTAction {
  bool caseInsensitive = false;
  std::unique_ptr<ASTConsumer>
  CreateASTConsumer(CompilerInstance &Instance,
                    llvm::StringRef InFile) override {
    return std::make_unique<CustomConsumer>(caseInsensitive);
  }
  bool ParseArgs(const CompilerInstance &Compiler,
                 const std::vector<std::string> &args) override {
    for (const auto &arg : args) {
      if (arg == "-i") {
        caseInsensitive = true;
      }
    }
    return true;
  }
};

static FrontendPluginRegistry::Add<PluginDeprFunc>
    X("plugin_for_deprecated_functions",
      "If the function name contains \"deprecated\" plugin writes a warning");
