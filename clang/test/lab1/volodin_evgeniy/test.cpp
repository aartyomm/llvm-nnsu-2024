// RUN: %clang_cc1 -load %llvmshlibdir/PrintClassPlugin%pluginext -plugin print-class-plugin %s | FileCheck %s
class SimpleClass {
    int a;
    float b;
};
// CHECK: SimpleClass
// CHECK-NEXT: |_a
// CHECK-NEXT: |_b

class EmptyClass {};
// CHECK: EmptyClass

class Empire {
    int world;
};
// CHECK: Empire
// CHECK-NEXT: |_world

template <typename T> class One {
    T two;
};
// CHECK: One
// CHECK-NEXT: |_two

class MyClass {
    One<int> t;    
};
// CHECK: MyClass
// CHECK-NEXT: |_t

struct B {
    int e;
    float b;
    double c;
    long long d;
};
// CHECK: B
// CHECK-NEXT: |_e
// CHECK-NEXT: |_b
// CHECK-NEXT: |_c
// CHECK-NEXT: |_d

struct Computer {
    struct Processor {
      int clock_frequency;
    };
    struct Memory{
      int size;
    };
    struct Motherboard{};
    struct GPU{};
};
// CHECK: Computer
// CHECK: Processor
// CHECK-NEXT: |_clock_frequency
// CHECK: Memory
// CHECK-NEXT: |_size
// CHECK: Motherboard
// CHECK: GPU

// RUN: %clang_cc1 -load %llvmshlibdir/PrintClassPlugin%pluginext -plugin print-class-plugin %s -plugin-arg-print-class-plugin help 2>&1 | FileCheck %s --check-prefix=HELP
// HELP: #Help for the Clang