// RUN: %clang_cc1 -load %llvmshlibdir/deprWarnPlugin%pluginext -plugin plugin_for_deprecated_functions %s 2>&1 | FileCheck %s

// CHECK: warning: The function name contains "deprecated"
void deprecatedFunction();

// CHECK: warning: The function name contains "deprecated"
void functionWithDeprecatedWord();

// CHECK-NOT: warning: The function name contains "deprecated"
void regularFunction();

class SomeClass {
// CHECK: warning: The function name contains "deprecated"
  void functionWithDePrEcAtEdWord();
// CHECK-NOT: warning: The function name contains "deprecated"
  void regularFunctionAgain();
};

