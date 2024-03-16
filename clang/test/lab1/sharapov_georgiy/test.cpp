// RUN: split-file %s %t
// RUN: %clang++ -cc1 -load %llvmshlibdir/DepWarningPlugin%pluginext -plugin DepEmitWarning %t/no_args.cpp 2>&1 | FileCheck %t/no_args.cpp
// RUN: %clang++ -cc1 -load %llvmshlibdir/DepWarningPlugin%pluginext -plugin DepEmitWarning -plugin-arg-DepEmitWarning --attr-style=gcc %t/gcc.cpp 2>&1 | FileCheck %t/gcc.cpp
// RUN: %clang++ -cc1 -load %llvmshlibdir/DepWarningPlugin%pluginext -plugin DepEmitWarning -plugin-arg-DepEmitWarning --attr-style=c++14 %t/c++14.cpp 2>&1 | FileCheck %t/c++14.cpp
// RUN: %clang++ -cc1 -load %llvmshlibdir/DepWarningPlugin%pluginext -plugin DepEmitWarning -plugin-arg-DepEmitWarning --attr-style=both %t/both.cpp 2>&1 | FileCheck %t/both.cpp
// RUN: %clang++ -cc1 -load %llvmshlibdir/DepWarningPlugin%pluginext -plugin DepEmitWarning -plugin-arg-DepEmitWarning --attr-style=aaaaa %t/wrong_style.cpp 2>&1 | FileCheck %t/wrong_style.cpp


//--- no_args.cpp

// CHECK-NOT: warning: Invalid value for attr-style. Using default value 'both'.

// CHECK: no_args.cpp:5:20: warning: function 'oldSum' is deprecated (c++14)
[[deprecated]] int oldSum(int a, int b);

// CHECK: no_args.cpp:8:6: warning: function 'oldFunc1' is deprecated (gcc)
void oldFunc1(int a, int b) __attribute__((deprecated));

// CHECK: no_args.cpp:11:34: warning: function 'oldFunc2' is deprecated (gcc)
__attribute__((deprecated)) void oldFunc2(int a, int b);

// CHECK: no_args.cpp:15:6: warning: function 'oldFunc3' is deprecated (gcc)
__attribute__((deprecated))
void oldFunc3(int a, int b);

// CHECK: no_args.cpp:19:6: warning: function 'oldFunc4' is deprecated (c++14)
[[deprecated]]
void oldFunc4(int a, int b);

// CHECK-NOT: warning: 
void Func(int a, int b);

//--- gcc.cpp

// CHECK-NOT: warning: Invalid value for attr-style. Using default value 'both'.

// CHECK-NOT: warning:
[[deprecated]] int oldSum(int a, int b);

// CHECK: gcc.cpp:8:6: warning: function 'oldFunc1' is deprecated (gcc)
void oldFunc1(int a, int b) __attribute__((deprecated));

// CHECK: gcc.cpp:11:34: warning: function 'oldFunc2' is deprecated (gcc)
__attribute__((deprecated)) void oldFunc2(int a, int b);

// CHECK: gcc.cpp:15:6: warning: function 'oldFunc3' is deprecated (gcc)
__attribute__((deprecated))
void oldFunc3(int a, int b);

// CHECK-NOT: warning:
[[deprecated]]
void oldFunc4(int a, int b);

// CHECK-NOT: warning: 
void Func(int a, int b);

//--- c++14.cpp

// CHECK-NOT: warning: Invalid value for attr-style. Using default value 'both'.

// CHECK: c++14.cpp:5:20: warning: function 'oldSum' is deprecated (c++14)
[[deprecated]] int oldSum(int a, int b);

// CHECK-NOT: warning:
void oldFunc1(int a, int b) __attribute__((deprecated));

// CHECK-NOT: warning:
__attribute__((deprecated)) void oldFunc2(int a, int b);

// CHECK-NOT: warning:
__attribute__((deprecated))
void oldFunc3(int a, int b);

// CHECK: c++14.cpp:19:6: warning: function 'oldFunc4' is deprecated (c++14)
[[deprecated]]
void oldFunc4(int a, int b);

// CHECK-NOT: warning: 
void Func(int a, int b);

//--- both.cpp

// CHECK-NOT: warning: Invalid value for attr-style. Using default value 'both'.

// CHECK: both.cpp:5:20: warning: function 'oldSum' is deprecated (c++14)
[[deprecated]] int oldSum(int a, int b);

// CHECK: both.cpp:8:6: warning: function 'oldFunc1' is deprecated (gcc)
void oldFunc1(int a, int b) __attribute__((deprecated));

// CHECK: both.cpp:11:34: warning: function 'oldFunc2' is deprecated (gcc)
__attribute__((deprecated)) void oldFunc2(int a, int b);

// CHECK: both.cpp:15:6: warning: function 'oldFunc3' is deprecated (gcc)
__attribute__((deprecated))
void oldFunc3(int a, int b);

// CHECK: both.cpp:19:6: warning: function 'oldFunc4' is deprecated (c++14)
[[deprecated]]
void oldFunc4(int a, int b);

// CHECK-NOT: warning:
void Func(int a, int b);

//--- wrong_style.cpp

// CHECK: warning: Invalid value for attr-style. Using default value 'both'.

// CHECK: wrong_style.cpp:5:20: warning: function 'oldSum' is deprecated (c++14)
[[deprecated]] int oldSum(int a, int b);

// CHECK: wrong_style.cpp:8:6: warning: function 'oldFunc1' is deprecated (gcc)
void oldFunc1(int a, int b) __attribute__((deprecated));

// CHECK: wrong_style.cpp:11:34: warning: function 'oldFunc2' is deprecated (gcc)
__attribute__((deprecated)) void oldFunc2(int a, int b);

// CHECK: wrong_style.cpp:15:6: warning: function 'oldFunc3' is deprecated (gcc)
__attribute__((deprecated))
void oldFunc3(int a, int b);

// CHECK: wrong_style.cpp:19:6: warning: function 'oldFunc4' is deprecated (c++14)
[[deprecated]]
void oldFunc4(int a, int b);

// CHECK-NOT: warning:
void Func(int a, int b);
