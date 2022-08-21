source_filename = "src\5CIntMath.ll"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc19.25.28614"

define noundef i16 @AddReturnOverflowI16(i16 noundef %a, i16 noundef %b, i16* nocapture noundef writeonly %carryOut) local_unnamed_addr #0
{
    %added = tail call { i16, i1 } @llvm.sadd.with.overflow.i16(i16 %a, i16 %b)
    %addRes = extractvalue { i16, i1 } %added, 0
    %carryRes = extractvalue { i16, i1 } %added, 1
    %carryInt = zext i1 %carryRes to i16
    store i16 %carryInt, i16* %carryOut, align 2
    ret i16 %addRes
}

define noundef i32 @AddReturnOverflowI32(i32 noundef %a, i32 noundef %b, i32* nocapture noundef writeonly %carryOut) local_unnamed_addr #0
{
    %added = tail call { i32, i1 } @llvm.sadd.with.overflow.i32(i32 %a, i32 %b)
    %addRes = extractvalue { i32, i1 } %added, 0
    %carryRes = extractvalue { i32, i1 } %added, 1
    %carryInt = zext i1 %carryRes to i32
    store i32 %carryInt, i32* %carryOut, align 4
    ret i32 %addRes
}

define noundef i64 @AddReturnOverflowI64(i64 noundef %a, i64 noundef %b, i64* nocapture noundef writeonly %carryOut) local_unnamed_addr #0
{
    %added = tail call { i64, i1 } @llvm.sadd.with.overflow.i64(i64 %a, i64 %b)
    %addRes = extractvalue { i64, i1 } %added, 0
    %carryRes = extractvalue { i64, i1 } %added, 1
    %carryInt = zext i1 %carryRes to i64
    store i64 %carryInt, i64* %carryOut, align 8
    ret i64 %addRes
}

define noundef i16 @AddReturnOverflowU16(i16 noundef %a, i16 noundef %b, i16* nocapture noundef writeonly %carryOut) local_unnamed_addr #0
{
    %added = tail call { i16, i1 } @llvm.uadd.with.overflow.i16(i16 %a, i16 %b)
    %addRes = extractvalue { i16, i1 } %added, 0
    %carryRes = extractvalue { i16, i1 } %added, 1
    %carryInt = zext i1 %carryRes to i16
    store i16 %carryInt, i16* %carryOut, align 2
    ret i16 %addRes
}

define noundef i32 @AddReturnOverflowU32(i32 noundef %a, i32 noundef %b, i32* nocapture noundef writeonly %carryOut) local_unnamed_addr #0
{
    %added = tail call { i32, i1 } @llvm.uadd.with.overflow.i32(i32 %a, i32 %b)
    %addRes = extractvalue { i32, i1 } %added, 0
    %carryRes = extractvalue { i32, i1 } %added, 1
    %carryInt = zext i1 %carryRes to i32
    store i32 %carryInt, i32* %carryOut, align 4
    ret i32 %addRes
}

define noundef i64 @AddReturnOverflowU64(i64 noundef %a, i64 noundef %b, i64* nocapture noundef writeonly %carryOut) local_unnamed_addr #0
{
    %added = tail call { i64, i1 } @llvm.uadd.with.overflow.i64(i64 %a, i64 %b)
    %addRes = extractvalue { i64, i1 } %added, 0
    %carryRes = extractvalue { i64, i1 } %added, 1
    %carryInt = zext i1 %carryRes to i64
    store i64 %carryInt, i64* %carryOut, align 8
    ret i64 %addRes
}

define noundef i16 @SubReturnBorrowI16(i16 noundef %a, i16 noundef %b, i16* nocapture noundef writeonly %borrowOut) local_unnamed_addr #0
{
    %subbed = tail call { i16, i1 } @llvm.ssub.with.overflow.i16(i16 %a, i16 %b)
    %subRes = extractvalue { i16, i1 } %subbed, 0
    %borrowRes = extractvalue { i16, i1 } %subbed, 1
    %borrowInt = zext i1 %borrowRes to i16
    store i16 %borrowInt, i16* %borrowOut, align 2
    ret i16 %subRes
}

define noundef i32 @SubReturnBorrowI32(i32 noundef %a, i32 noundef %b, i32* nocapture noundef writeonly %borrowOut) local_unnamed_addr #0
{
    %subbed = tail call { i32, i1 } @llvm.ssub.with.overflow.i32(i32 %a, i32 %b)
    %subRes = extractvalue { i32, i1 } %subbed, 0
    %borrowRes = extractvalue { i32, i1 } %subbed, 1
    %borrowInt = zext i1 %borrowRes to i32
    store i32 %borrowInt, i32* %borrowOut, align 4
    ret i32 %subRes
}

define noundef i64 @SubReturnBorrowI64(i64 noundef %a, i64 noundef %b, i64* nocapture noundef writeonly %borrowOut) local_unnamed_addr #0
{
    %subbed = tail call { i64, i1 } @llvm.ssub.with.overflow.i64(i64 %a, i64 %b)
    %subRes = extractvalue { i64, i1 } %subbed, 0
    %borrowRes = extractvalue { i64, i1 } %subbed, 1
    %borrowInt = zext i1 %borrowRes to i64
    store i64 %borrowInt, i64* %borrowOut, align 8
    ret i64 %subRes
}

define noundef i16 @SubReturnBorrowU16(i16 noundef %a, i16 noundef %b, i16* nocapture noundef writeonly %borrowOut) local_unnamed_addr #0
{
    %subbed = tail call { i16, i1 } @llvm.usub.with.overflow.i16(i16 %a, i16 %b)
    %subRes = extractvalue { i16, i1 } %subbed, 0
    %borrowRes = extractvalue { i16, i1 } %subbed, 1
    %borrowInt = zext i1 %borrowRes to i16
    store i16 %borrowInt, i16* %borrowOut, align 2
    ret i16 %subRes
}

define noundef i32 @SubReturnBorrowU32(i32 noundef %a, i32 noundef %b, i32* nocapture noundef writeonly %borrowOut) local_unnamed_addr #0
{
    %subbed = tail call { i32, i1 } @llvm.usub.with.overflow.i32(i32 %a, i32 %b)
    %subRes = extractvalue { i32, i1 } %subbed, 0
    %borrowRes = extractvalue { i32, i1 } %subbed, 1
    %borrowInt = zext i1 %borrowRes to i32
    store i32 %borrowInt, i32* %borrowOut, align 4
    ret i32 %subRes
}

define noundef i64 @SubReturnBorrowU64(i64 noundef %a, i64 noundef %b, i64* nocapture noundef writeonly %borrowOut) local_unnamed_addr #0
{
    %subbed = tail call { i64, i1 } @llvm.usub.with.overflow.i64(i64 %a, i64 %b)
    %subRes = extractvalue { i64, i1 } %subbed, 0
    %borrowRes = extractvalue { i64, i1 } %subbed, 1
    %borrowInt = zext i1 %borrowRes to i64
    store i64 %borrowInt, i64* %borrowOut, align 8
    ret i64 %subRes
}

define noundef i16 @MulReturnOverflowI16(i16 noundef %a, i16 noundef %b, i16* nocapture noundef writeonly %overflowOut) local_unnamed_addr #0
{
    %mulled = tail call { i16, i1 } @llvm.smul.with.overflow.i16(i16 %a, i16 %b)
    %mulRes = extractvalue { i16, i1 } %mulled, 0
    %overflowRes = extractvalue { i16, i1 } %mulled, 1
    %overflowInt = zext i1 %overflowRes to i16
    store i16 %overflowInt, i16* %overflowOut, align 2
    ret i16 %mulRes
}

define noundef i32 @MulReturnOverflowI32(i32 noundef %a, i32 noundef %b, i32* nocapture noundef writeonly %overflowOut) local_unnamed_addr #0
{
    %mulled = tail call { i32, i1 } @llvm.smul.with.overflow.i32(i32 %a, i32 %b)
    %mulRes = extractvalue { i32, i1 } %mulled, 0
    %overflowRes = extractvalue { i32, i1 } %mulled, 1
    %overflowInt = zext i1 %overflowRes to i32
    store i32 %overflowInt, i32* %overflowOut, align 4
    ret i32 %mulRes
}

define noundef i64 @MulReturnOverflowI64(i64 noundef %a, i64 noundef %b, i64* nocapture noundef writeonly %overflowOut) local_unnamed_addr #0
{
    %mulled = tail call { i64, i1 } @llvm.smul.with.overflow.i64(i64 %a, i64 %b)
    %mulRes = extractvalue { i64, i1 } %mulled, 0
    %overflowRes = extractvalue { i64, i1 } %mulled, 1
    %overflowInt = zext i1 %overflowRes to i64
    store i64 %overflowInt, i64* %overflowOut, align 8
    ret i64 %mulRes
}

define noundef i16 @MulReturnOverflowU16(i16 noundef %a, i16 noundef %b, i16* nocapture noundef writeonly %overflowOut) local_unnamed_addr #0
{
    %mulled = tail call { i16, i1 } @llvm.umul.with.overflow.i16(i16 %a, i16 %b)
    %mulRes = extractvalue { i16, i1 } %mulled, 0
    %overflowRes = extractvalue { i16, i1 } %mulled, 1
    %overflowInt = zext i1 %overflowRes to i16
    store i16 %overflowInt, i16* %overflowOut, align 2
    ret i16 %mulRes
}

define noundef i32 @MulReturnOverflowU32(i32 noundef %a, i32 noundef %b, i32* nocapture noundef writeonly %overflowOut) local_unnamed_addr #0
{
    %mulled = tail call { i32, i1 } @llvm.umul.with.overflow.i32(i32 %a, i32 %b)
    %mulRes = extractvalue { i32, i1 } %mulled, 0
    %overflowRes = extractvalue { i32, i1 } %mulled, 1
    %overflowInt = zext i1 %overflowRes to i32
    store i32 %overflowInt, i32* %overflowOut, align 4
    ret i32 %mulRes
}

define noundef i64 @MulReturnOverflowU64(i64 noundef %a, i64 noundef %b, i64* nocapture noundef writeonly %overflowOut) local_unnamed_addr #0
{
    %mulled = tail call { i64, i1 } @llvm.umul.with.overflow.i64(i64 %a, i64 %b)
    %mulRes = extractvalue { i64, i1 } %mulled, 0
    %overflowRes = extractvalue { i64, i1 } %mulled, 1
    %overflowInt = zext i1 %overflowRes to i64
    store i64 %overflowInt, i64* %overflowOut, align 8
    ret i64 %mulRes
}

declare { i16, i1 } @llvm.sadd.with.overflow.i16(i16, i16)
declare { i32, i1 } @llvm.sadd.with.overflow.i32(i32, i32)
declare { i64, i1 } @llvm.sadd.with.overflow.i64(i64, i64)

declare { i16, i1 } @llvm.uadd.with.overflow.i16(i16, i16)
declare { i32, i1 } @llvm.uadd.with.overflow.i32(i32, i32)
declare { i64, i1 } @llvm.uadd.with.overflow.i64(i64, i64)

declare { i16, i1 } @llvm.ssub.with.overflow.i16(i16, i16)
declare { i32, i1 } @llvm.ssub.with.overflow.i32(i32, i32)
declare { i64, i1 } @llvm.ssub.with.overflow.i64(i64, i64)

declare { i16, i1 } @llvm.usub.with.overflow.i16(i16, i16)
declare { i32, i1 } @llvm.usub.with.overflow.i32(i32, i32)
declare { i64, i1 } @llvm.usub.with.overflow.i64(i64, i64)

declare { i16, i1 } @llvm.smul.with.overflow.i16(i16, i16)
declare { i32, i1 } @llvm.smul.with.overflow.i32(i32, i32)
declare { i64, i1 } @llvm.smul.with.overflow.i64(i64, i64)

declare { i16, i1 } @llvm.umul.with.overflow.i16(i16, i16)
declare { i32, i1 } @llvm.umul.with.overflow.i32(i32, i32)
declare { i64, i1 } @llvm.umul.with.overflow.i64(i64, i64)

attributes #0 = { inlinehint nofree norecurse willreturn nosync nounwind optsize }
