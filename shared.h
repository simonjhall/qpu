/*
 * shared.h
 *
 *  Created on: 11 Apr 2014
 *      Author: simon
 */

#ifndef SHARED_H_
#define SHARED_H_

enum ConditionCode
{
	kNever = 0,
	kAlways,
	kZset,
	kZclear,
	kNset,
	kNclear,
	kCset,
	kCclear
};

enum BranchCondition
{
	kAllZSet = 0,
	kAllZClear,

	kAnyZSet,
	kAnyZClear,

	kAllNSet,
	kAllNClear,

	kAnyNSet,
	kAnyNClear,

	kAllCSet,
	kAllCClear,

	kAnyCSet,
	kAnyCClear,

	kAlwaysBr = 15,
};

enum Signal
{
	kBreakpoint = 0,
	kNoSignal,
	kThreadSwitch,
	kProgramEnd,
	kScoreboardWait,
	kScoreboardUnlock,
	kLastThreadSwitch,
	kCoverageLoad,
	kColourLoad,
	kColourLoadPe,
	kLoadTmu0,
	kLoadTmu1,
	kAlphaMaskLoad,
	kSmallImmOrVecRot,
	kLoadImm,
	kBranch,
};

enum AddOp
{
	kAddNop = 0,
	kFadd,
	kFsub,
	kFmin,
	kFmax,
	kFminAbs,
	kFmaxAbs,
	kFtoi,
	kItof,
	kAdd = 12,
	kSub,
	kShr,
	kAsr,
	kRor,
	kShl,
	kMin,
	kMax,
	kAnd,
	kOr,
	kXor,
	kNot,
	kClz,
	kV8addsAdd = 30,
	kV8subsAdd,
};

enum MulOp
{
	kMulNop = 0,
	kFmul,
	kMul24,
	kV8muld,
	kV8min,
	kV8max,
	kV8addsMul,
	kV8subsMul,
};

enum MuxEncoding
{
	kAcc0 = 0,
	kAcc1,
	kAcc2,
	kAcc3,
	kAcc4,
	kAcc5,
	kRfA,
	kRfB,
};



#endif /* SHARED_H_ */
