; ModuleID = 'b06.ssify.bc'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: nounwind uwtable
define void @bench_F(i32* %v, i32 %N) #0 {
entry:
  %sub = sub nsw i32 %N, 1
  br label %while.cond

while.cond:                                       ; preds = %if.end, %entry
  %cmp = icmp slt i32 0, %N
  br i1 %cmp, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %idxprom = sext i32 0 to i64
  %arrayidx = getelementptr inbounds i32* %v, i64 %idxprom
  %tmp = load i32* %arrayidx, align 4
  %idxprom1 = sext i32 %sub to i64
  %arrayidx2 = getelementptr inbounds i32* %v, i64 %idxprom1
  %tmp1 = load i32* %arrayidx2, align 4
  %cmp3 = icmp sgt i32 %tmp, %tmp1
  br i1 %cmp3, label %if.then, label %while.body.if.end_crit_edge

while.body.if.end_crit_edge:                      ; preds = %while.body
  br label %if.end

if.then:                                          ; preds = %while.body
  %idxprom4 = sext i32 0 to i64
  %arrayidx5 = getelementptr inbounds i32* %v, i64 %idxprom4
  %tmp2 = load i32* %arrayidx5, align 4
  %idxprom6 = sext i32 %sub to i64
  %arrayidx7 = getelementptr inbounds i32* %v, i64 %idxprom6
  %tmp3 = load i32* %arrayidx7, align 4
  %idxprom8 = sext i32 0 to i64
  %arrayidx9 = getelementptr inbounds i32* %v, i64 %idxprom8
  store i32 %tmp3, i32* %arrayidx9, align 4
  %idxprom10 = sext i32 %sub to i64
  %arrayidx11 = getelementptr inbounds i32* %v, i64 %idxprom10
  store i32 %tmp2, i32* %arrayidx11, align 4
  br label %if.end

if.end:                                           ; preds = %if.then, %while.body.if.end_crit_edge
  br label %while.cond

while.end:                                        ; preds = %while.cond
  ret void
}

; Function Attrs: nounwind uwtable
define i32 @main(i32 %argc, i8** %argv) #0 {
entry:
  %call = call noalias i8* @malloc(i64 400) #3
  %tmp = ptrtoint i8* %call to i32
  %call1 = call i32 (i32, i32, ...)* bitcast (i32 (...)* @benc_F to i32 (i32, i32, ...)*)(i32 %tmp, i32 %argc)
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
