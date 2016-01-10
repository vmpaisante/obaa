; ModuleID = 'b06.bc'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nounwind uwtable
define void @bench_F(i32* %v, i32 %N) #0 {
entry:
  %v.addr = alloca i32*, align 8
  %N.addr = alloca i32, align 4
  %i = alloca i32, align 4
  %j = alloca i32, align 4
  %tmp = alloca i32, align 4
  store i32* %v, i32** %v.addr, align 8
  store i32 %N, i32* %N.addr, align 4
  store i32 0, i32* %i, align 4
  %0 = load i32* %N.addr, align 4
  %sub = sub nsw i32 %0, 1
  store i32 %sub, i32* %j, align 4
  br label %while.cond

while.cond:                                       ; preds = %if.end, %entry
  %1 = load i32* %i, align 4
  %2 = load i32* %N.addr, align 4
  %cmp = icmp slt i32 %1, %2
  br i1 %cmp, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %3 = load i32* %i, align 4
  %idxprom = sext i32 %3 to i64
  %4 = load i32** %v.addr, align 8
  %arrayidx = getelementptr inbounds i32* %4, i64 %idxprom
  %5 = load i32* %arrayidx, align 4
  %6 = load i32* %j, align 4
  %idxprom1 = sext i32 %6 to i64
  %7 = load i32** %v.addr, align 8
  %arrayidx2 = getelementptr inbounds i32* %7, i64 %idxprom1
  %8 = load i32* %arrayidx2, align 4
  %cmp3 = icmp sgt i32 %5, %8
  br i1 %cmp3, label %if.then, label %if.end

if.then:                                          ; preds = %while.body
  %9 = load i32* %i, align 4
  %idxprom4 = sext i32 %9 to i64
  %10 = load i32** %v.addr, align 8
  %arrayidx5 = getelementptr inbounds i32* %10, i64 %idxprom4
  %11 = load i32* %arrayidx5, align 4
  store i32 %11, i32* %tmp, align 4
  %12 = load i32* %j, align 4
  %idxprom6 = sext i32 %12 to i64
  %13 = load i32** %v.addr, align 8
  %arrayidx7 = getelementptr inbounds i32* %13, i64 %idxprom6
  %14 = load i32* %arrayidx7, align 4
  %15 = load i32* %i, align 4
  %idxprom8 = sext i32 %15 to i64
  %16 = load i32** %v.addr, align 8
  %arrayidx9 = getelementptr inbounds i32* %16, i64 %idxprom8
  store i32 %14, i32* %arrayidx9, align 4
  %17 = load i32* %tmp, align 4
  %18 = load i32* %j, align 4
  %idxprom10 = sext i32 %18 to i64
  %19 = load i32** %v.addr, align 8
  %arrayidx11 = getelementptr inbounds i32* %19, i64 %idxprom10
  store i32 %17, i32* %arrayidx11, align 4
  br label %if.end

if.end:                                           ; preds = %if.then, %while.body
  br label %while.cond

while.end:                                        ; preds = %while.cond
  ret void
}

; Function Attrs: nounwind uwtable
define i32 @main(i32 %argc, i8** %argv) #0 {
entry:
  %argc.addr = alloca i32, align 4
  %argv.addr = alloca i8**, align 8
  %x = alloca i32, align 4
  store i32 %argc, i32* %argc.addr, align 4
  store i8** %argv, i8*** %argv.addr, align 8
  %call = call noalias i8* @malloc(i64 400) #3
  %0 = ptrtoint i8* %call to i32
  store i32 %0, i32* %x, align 4
  %1 = load i32* %x, align 4
  %2 = load i32* %argc.addr, align 4
  %call1 = call i32 (i32, i32, ...)* bitcast (i32 (...)* @benc_F to i32 (i32, i32, ...)*)(i32 %1, i32 %2)
  ret i32 0
}

; Function Attrs: nounwind
declare noalias i8* @malloc(i64) #1

declare i32 @benc_F(...) #2

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"clang version 3.5.0 (tags/RELEASE_350/final)"}
