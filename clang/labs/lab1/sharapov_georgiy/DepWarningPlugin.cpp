#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Lex/Lexer.h"

class DepWarningVisitor : public clang::RecursiveASTVisitor<DepWarningVisitor> {
public:
  explicit DepWarningVisitor(std::string style, bool help)
      : attrStyle(style), helpFlag(help) {}

  bool VisitFunctionDecl(clang::FunctionDecl *F) {
    if (F->hasAttr<clang::DeprecatedAttr>() && !helpFlag) {
      clang::DiagnosticsEngine &Diag = F->getASTContext().getDiagnostics();
      unsigned DiagID = 0;

      if (decl2str(F).find("__attribute__((deprecated") != std::string::npos) {
        if (attrStyle == "c++14") {
          return true;
        }
        DiagID = Diag.getCustomDiagID(clang::DiagnosticsEngine::Warning,
                                      "function '%0' is deprecated (gcc)");
      } else {
        if (attrStyle == "gcc") {
          return true;
        }
        DiagID = Diag.getCustomDiagID(clang::DiagnosticsEngine::Warning,
                                      "function '%0' is deprecated (c++14)");
      }

      Diag.Report(F->getLocation(), DiagID) << F->getNameAsString();
    }
    return true;
  }

private:
  std::string attrStyle;
  bool helpFlag;

  std::string decl2str(clang::FunctionDecl *F) {
    clang::SourceRange srcRange = F->getSourceRange();
    return clang::Lexer::getSourceText(
               clang::CharSourceRange::getTokenRange(srcRange),
               F->getASTContext().getSourceManager(),
               F->getASTContext().getLangOpts(), 0)
        .str();
  }
};

class DepWarningConsumer : public clang::ASTConsumer {
public:
  explicit DepWarningConsumer(std::string style, bool help)
      : Visitor(style, help) {}

  void HandleTranslationUnit(clang::ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  DepWarningVisitor Visitor;
};

class DepWarningAction : public clang::PluginASTAction {
public:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &Compiler,
                    llvm::StringRef InFile) override {
    return std::make_unique<DepWarningConsumer>(attrStyle, help);
  }

protected:
  bool ParseArgs(const clang::CompilerInstance &Compiler,
                 const std::vector<std::string> &args) override {
    for (const auto &Arg : args) {
      if (Arg == "-help") {
        help = true;
        llvm::outs() << ""
                        "\n  --attr-style=<string>    - Select which style of "
                        "attributes will be displayed,"
                        ""
                        ""
                        "\n                             options: gcc, c++14, "
                        "both. Default value 'both'.\n\n"
                        "";
      } else if (Arg.find("--attr-style=") == 0) {
        attrStyle = Arg.substr(std::string("--attr-style=").length());
        if (attrStyle != "c++14" && attrStyle != "gcc" && attrStyle != "both") {
          clang::DiagnosticsEngine &Diag = Compiler.getDiagnostics();
          unsigned DiagID = Diag.getCustomDiagID(
              clang::DiagnosticsEngine::Warning,
              "Invalid value for attr-style. Using default value 'both'.");
          Diag.Report(DiagID);
          attrStyle = "both";
        }
      }
    }
    return true;
  }

private:
  std::string attrStyle = "both";
  bool help = false;
};

static clang::FrontendPluginRegistry::Add<DepWarningAction>
    X("DepEmitWarning", "Emit warning for each deprecated function");
