// RUN: %clang -cc1 -load %llvmshlibdir/DeprecatedPlugin_SadikovD%pluginext -plugin DeprecatedPlugin_SadikovD %s 2>&1 | FileCheck %s

// CHECK-NOT: warning: Name of function 'just_regular_sum_function' contains 'deprecated'
int just_regular_sum_function(int a, int b) {
	return a + b;
}

// CHECK: warning: Name of function 'strange_deprecated_sum_function' contains 'deprecated'
int strange_deprecated_sum_function(int a, int b) {
	return a + b;
}

// CHECK-NOT: warning: Name of function 'IsItAlsoDeprecated' contains 'deprecated'
int IsItAlsoDeprecated(int a, int b, int c) {
	return a + b + c;
}

struct foo {
// CHECK-NOT: warning: Name of function 'bar' contains 'deprecated'
	void bar() {}
// CHECK: warning: Name of function 'bar_deprecated' contains 'deprecated'
	void bar_deprecated() {}
};
