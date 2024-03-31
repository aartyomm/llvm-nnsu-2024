; RUN: opt -load-pass-plugin=%llvmshlibdir/loopStartEndPlugin%shlibext -passes=loop-start-end -S %s | FileCheck %s

; void foo(int n, int m) {
;     int c0;
;     int c1;
;     for (c0 = n; c0 > 0; c0--) {
;         c1++;
;     }
; }

; void a() {
;     int c = 10;
;     for (int i = 0; i < 10; i++) {
;         c++;
;     }
;     int q = c + 42;
; }

define dso_local void @_Z3fooii(i32 noundef %n, i32 noundef %m) {
entry:
  %n.addr = alloca i32, align 4
  %m.addr = alloca i32, align 4
  %c0 = alloca i32, align 4
  %c1 = alloca i32, align 4
  store i32 %n, ptr %n.addr, align 4
  store i32 %m, ptr %m.addr, align 4
  %0 = load i32, ptr %n.addr, align 4
  store i32 %0, ptr %c0, align 4
; CHECK: call void @loop_start()
  br label %for.cond

for.cond:
  %1 = load i32, ptr %c0, align 4
  %cmp = icmp sgt i32 %1, 0
  br i1 %cmp, label %for.body, label %for.end

for.body:
  %2 = load i32, ptr %c1, align 4
  %inc = add nsw i32 %2, 1
  store i32 %inc, ptr %c1, align 4
  br label %for.inc

for.inc:
  %3 = load i32, ptr %c0, align 4
  %dec = add nsw i32 %3, -1
  store i32 %dec, ptr %c0, align 4
  br label %for.cond

for.end:
; CHECK: call void @loop_end()
  ret void
}


define dso_local void @a() {
entry:
  %c = alloca i32, align 4
  %i = alloca i32, align 4
  %q = alloca i32, align 4
  store i32 10, ptr %c, align 4
  store i32 0, ptr %i, align 4
; CHECK:    call void @loop_start()
  br label %for.cond

for.cond:
  %0 = load i32, ptr %i, align 4
  %cmp = icmp slt i32 %0, 10
  br i1 %cmp, label %for.body, label %for.end

for.body:
  %1 = load i32, ptr %c, align 4
  %inc = add nsw i32 %1, 1
  store i32 %inc, ptr %c, align 4
  br label %for.inc

for.inc:
  %2 = load i32, ptr %i, align 4
  %inc1 = add nsw i32 %2, 1
  store i32 %inc1, ptr %i, align 4
  br label %for.cond

for.end:
; CHECK:    call void @loop_end()
  %3 = load i32, ptr %c, align 4
  %add = add nsw i32 %3, 42
  store i32 %add, ptr %q, align 4
  ret void
}