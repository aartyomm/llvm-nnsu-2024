// RUN: %clang_cc1 -load %llvmshlibdir/ClassListPlugin%pluginext -plugin class_list_plugin %s 1>&1 | FileCheck %s

// CHECK: Empty
class Empty {};

// CHECK: MyClass
struct MyClass {
    // CHECK-NEXT: |_variable
    int variable;
};

// CHECK: MyClass1
class MyClass1 {
    // CHECK-NEXT: |_var
    float var;
    // CHECK-NEXT: |_var1
    double var1;
    // CHECK: InClass
    class InClass {
        //CHECK-NEXT: |_InVar
        int InVar;
    };
};

// CHECK: TClass
template<typename T> class TClass {
    // CHECK-NEXT: TVar
    T TVar;
};