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

inline const char *GetMuxEncodingName(MuxEncoding m)
{
	static const char *pMuxEncodingNames[8] = {
			"acc0",
			"acc1",
			"acc2",
			"acc3",
			"acc4",
			"acc5",
			"ra",
			"rb",
	};

	return pMuxEncodingNames[m];
}

inline const char *GetConditionCodeName(ConditionCode c)
{
	static const char *pConditionCodeNames[8] = {
			"never",
			"al",
			"zs",
			"zc",
			"ns",
			"nc",
			"cs",
			"cc",
	};

	return pConditionCodeNames[c];
}

inline const char *GetBranchConditionName(BranchCondition b)
{
	static const char *pBranchConditionNames[16] = {
		"allzs",
		"allzc",

		"anyzs",
		"anyzc",

		"allns",
		"allnc",

		"anyns",
		"anync",

		"allcs",
		"allcc",

		"anycs",
		"anycc",

		"RESERVED",
		"RESERVED",
		"RESERVED",

		""
	};

	return pBranchConditionNames[b];
}

inline const char *GetSignalName(Signal s)
{
	static const char *pSignalNames[16] = {
			"Breakpoint",
			"NoSignal",
			"ThreadSwitch",
			"ProgramEnd",
			"ScoreboardWait",
			"ScoreboardUnlock",
			"LastThreadSwitch",
			"CoverageLoad",
			"ColourLoad",
			"ColourLoadPe",
			"LoadTmu0",
			"LoadTmu1",
			"AlphaMaskLoad",
			"SmallImmOrVecRot",
			"LoadImm",
			"Branch",
	};

	return pSignalNames[s];
}

inline const char *GetAddOpName(AddOp a)
{
	static const char *pAddOpNames[32] = {
			"addnop",
			"fadd",
			"fsub",
			"fmin",
			"fmax",
			"fminabs",
			"fmaxabs",
			"ftoi",
			"itof",
			"reserved",
			"reserved",
			"reserved",
			"add",
			"sub",
			"shr",
			"asr",
			"ror",
			"shl",
			"min",
			"max",
			"and",
			"or",
			"xor",
			"not",
			"clz",
			"reserved",
			"reserved",
			"reserved",
			"reserved",
			"reserved",
			"addv8adds",
			"addv8subs",
	};

	return pAddOpNames[a];
}

inline const char *GetMulOpName(MulOp m)
{
	static const char *pMulOpNames[8] = {
			"mulnop",
			"fmul",
			"mul24",
			"v8muld",
			"v8min",
			"v8max",
			"mulv8adds",
			"mulv8subs",
	};

	return pMulOpNames[m];
}

inline const char *GetSmallImm(int i)
{
	static const char *pSmallImm[64] = {
		"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15",
		"-16", "-15", "-14", "-13", "-12", "-11", "-10", "-9", "-8", "-7", "-6", "-5", "-4", "-3", "-2", "-1",
		"1.0", "2.0", "4.0", "8.0", "16.0", "32.0", "64.0", "128.0",
		"1.0/256.0", "1.0/128.0", "1.0/64.0", "1.0/32.0", "1.0/16.0", "1.0/8.0", "1.0/4.0", "1.0/2.0",
		"vecrot acc5", "vecrot 1", "vecrot 2", "vecrot 3", "vecrot 4", "vecrot 5", "vecrot 6", "vecrot 7", "vecrot 8", "vecrot 9", "vecrot 10", "vecrot 11", "vecrot 12", "vecrot 13", "vecrot 14", "vecrot 15"
	};

	return pSmallImm[i];
}

#endif /* SHARED_H_ */
