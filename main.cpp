/*
 * main.cpp
 *
 *  Created on: 7 Apr 2014
 *      Author: simon
 */

#include <stdio.h>
#include <stdint.h>

unsigned int code[] = {
#include "shader_256.hex"
};

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

const char *pMuxEncodingNames[8] = {
		"Acc0",
		"Acc1",
		"Acc2",
		"Acc3",
		"Acc4",
		"Acc5",
		"A",
		"B",
};

const char *pConditionCodeNames[8] = {
		"never",
		"always",
		"z set",
		"z clear",
		"n set",
		"n clear",
		"c set",
		"c clear",
};

const char *pSignalNames[16] = {
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

const char *pAddOpNames[32] = {
		"AddNop",
		"Fadd",
		"Fsub",
		"Fmin",
		"Fmax",
		"FminAbs",
		"FmaxAbs",
		"Ftoi",
		"Itof",
		"RESERVED",
		"RESERVED",
		"RESERVED",
		"Add",
		"Sub",
		"Shr",
		"Asr",
		"Ror",
		"Shl",
		"Min",
		"Max",
		"And",
		"Or",
		"Xor",
		"Not",
		"Clz",
		"RESERVED",
		"RESERVED",
		"RESERVED",
		"RESERVED",
		"RESERVED",
		"V8adds",
		"V8subs",
};

const char *pMulOpNames[8] = {
		"MulNop",
		"Fmul",
		"Mul24",
		"V8muld",
		"V8min",
		"V8max",
		"V8adds",
		"V8subs",
};

void packunpack(uint64_t dword)
{
	uint64_t pack, unpack;

	pack = (dword >> 52) & 0xf;
	unpack = (dword >> 57) & 0x7;

	if (pack || unpack)
	{
		if ((dword >> 56) & 1)
		{
			printf("pm=1 ");
		}
		else
			printf("pm=0 ");

		printf("pack %2lx unpack %lx ", pack, unpack);
	}
}

void cond_add_mul(uint64_t dword, ConditionCode &rAdd, ConditionCode &rMul)
{
	uint64_t add = (dword >> 49) & 0x7;
	uint64_t mul = (dword >> 46) & 0x7;

	rAdd = (ConditionCode)add;
	rMul = (ConditionCode)mul;

	printf("cond_add=%s cond_mul=%s ", pConditionCodeNames[add], pConditionCodeNames[mul]);
}

void set_flags(uint64_t dword)
{
	if ((dword >> 45) & 1)
		printf("sf=%ld ", (dword >> 45) & 1);
}

bool write_swap(uint64_t dword)
{
	if ((dword >> 44) & 1)
		printf("ws=%ld ", (dword >> 44) & 1);
	return (bool)((dword >> 44) & 1);
}

void waddr(uint64_t dword, bool swap, ConditionCode enableA, ConditionCode enableB)
{
	if (enableA != kNever)
		printf("add=%c%ld ", swap ? 'B' : 'A', (dword >> 38) & 63);

	if (enableB != kNever)
		printf("mul=%c%ld ", swap ? 'A' : 'B', (dword >> 32) & 63);
}

void immediate32(uint64_t dword)
{
	printf("imm32=%lx/%ld ",
			dword & 0xffffffff,
			dword & 0xffffffff);
}

void signal(uint64_t dword)
{
	if ((Signal)(dword >> 60) != kNoSignal)
		printf("sig=%s ", pSignalNames[dword >> 60]);
}

void op_mul_add(uint64_t dword, ConditionCode enableA, ConditionCode enableB, AddOp &rAdd, MulOp &rMul)
{
	rAdd = (AddOp)((dword >> 24) & 31);
	rMul = (MulOp)((dword >> 29) & 7);

	if (!(enableA == kNever && rAdd == kAddNop))
		printf("addop=%s ", pAddOpNames[(dword >> 24) & 31]);
	if (!(enableB == kNever && rMul == kMulNop))
		printf("mulop=%s ", pMulOpNames[(dword >> 29) & 7]);

	if (rAdd == kAddNop && rMul == kMulNop)
		printf("nop \n");
}

MuxEncoding mux_decode(uint64_t section)
{
	MuxEncoding m = (MuxEncoding)section;
	printf("%s ", pMuxEncodingNames[section]);
	return m;
}

void disassemble(uint64_t dword)
{
	ConditionCode addCc, mulCc;
	AddOp addOp;
	MulOp mulOp;

	if ((dword >> 57) == 0x74)
	{
		printf("semaphore\n");
		packunpack(dword);
		cond_add_mul(dword, addCc, mulCc);
		set_flags(dword);
		write_swap(dword);
		waddr(dword, write_swap(dword), addCc, mulCc);
		printf("\n");
	}
	else if ((dword >> 57) == 0x73)
	{
		printf("load imm per-elmt unsigned\n");
		packunpack(dword);
		cond_add_mul(dword, addCc, mulCc);
		set_flags(dword);
		waddr(dword, write_swap(dword), addCc, mulCc);
		printf("\n");
	}
	else if ((dword >> 57) == 0x71)
	{
		printf("load imm per-elmt signed\n");
		packunpack(dword);
		cond_add_mul(dword, addCc, mulCc);
		set_flags(dword);
		write_swap(dword);
		waddr(dword, write_swap(dword), addCc, mulCc);
		printf("\n");
	}
	else if ((dword >> 57) == 0x70)
	{
		printf("load imm 32\n");
		packunpack(dword);
		cond_add_mul(dword, addCc, mulCc);
		set_flags(dword);
		write_swap(dword);
		waddr(dword, write_swap(dword), addCc, mulCc);
		immediate32(dword);
		printf("\n");
	}
	else if (((dword >> 57) & 0x78) == 0x78)
	{
		printf("branch\n");
		write_swap(dword);
		waddr(dword, write_swap(dword), kAlways, kAlways);
		printf("\n");
	}
	else if (((dword >> 57) & 0x78) == 0x68)
	{
		printf("alu small imm\n");
		packunpack(dword);
		cond_add_mul(dword, addCc, mulCc);
		set_flags(dword);
		write_swap(dword);
		waddr(dword, write_swap(dword), addCc, mulCc);
		op_mul_add(dword, addCc, mulCc, addOp, mulOp);

		if (!(addCc == kNever && addOp == kAddNop))
		{
			MuxEncoding a = mux_decode((dword >> 9) & 7);
			MuxEncoding b = mux_decode((dword >> 6) & 7);

			uint64_t ra, rb;
			ra = (dword >> 18) & 63;
			rb = (dword >> 12) & 63;

			if (a == kRfA)
				printf("a=%s%ld ", pMuxEncodingNames[a], ra);
			if (b == kRfA)
				printf("b=%s%ld ", pMuxEncodingNames[b], rb);
			if (a == kRfB)
				printf("a=%ld ", ra);
			if (b == kRfB)
				printf("b=%ld ", rb);
		}
		if (!(mulCc == kNever && mulOp == kMulNop))
		{
			MuxEncoding a = mux_decode((dword >> 3) & 7);
			MuxEncoding b = mux_decode(dword & 7);

			uint64_t ra, rb;
			ra = (dword >> 18) & 63;
			rb = (dword >> 12) & 63;

			if (a == kRfA)
				printf("a=%s%ld ", pMuxEncodingNames[a], ra);
			if (b == kRfA)
				printf("b=%s%ld ", pMuxEncodingNames[b], rb);
			if (a == kRfB)
				printf("a=%ld ", ra);
			if (b == kRfB)
				printf("b=%ld ", rb);
		}

		printf("\n");
	}
	else
	{
		printf("alu\n");
		packunpack(dword);
		cond_add_mul(dword, addCc, mulCc);
		set_flags(dword);
		write_swap(dword);
		waddr(dword, write_swap(dword), addCc, mulCc);
		signal(dword);
		op_mul_add(dword, addCc, mulCc, addOp, mulOp);

		if (!(addCc == kNever && addOp == kAddNop))
		{
			MuxEncoding a = mux_decode((dword >> 9) & 7);
			MuxEncoding b = mux_decode((dword >> 6) & 7);

			uint64_t ra, rb;
			ra = (dword >> 18) & 63;
			rb = (dword >> 12) & 63;

			if (a == kRfA || a == kRfB)
				printf("a=%s%ld ", pMuxEncodingNames[a], ra);
			if (b == kRfA || b == kRfB)
				printf("b=%s%ld ", pMuxEncodingNames[b], rb);
		}
		if (!(mulCc == kNever && mulOp == kMulNop))
		{
			MuxEncoding a = mux_decode((dword >> 3) & 7);
			MuxEncoding b = mux_decode(dword & 7);

			uint64_t ra, rb;
			ra = (dword >> 18) & 63;
			rb = (dword >> 12) & 63;

			if (a == kRfA || a == kRfB)
				printf("a=%s%ld ", pMuxEncodingNames[a], ra);
			if (b == kRfA || b == kRfB)
				printf("b=%s%ld ", pMuxEncodingNames[b], rb);
		}
		printf("\n");
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		for (unsigned int count = 0; count < sizeof(code) / 8; count++)
		{
			unsigned int address = count * 8;

			uint64_t dword = ((uint64_t)code[count * 2 + 1] << 32) | (uint64_t)code[count * 2];
			printf("%5x: %016lx\n", address, dword);
			disassemble(dword);
		}

		return 0;
	}
	else
	{
		FILE *fp = fopen(argv[1], "rb");
		if (!fp)
			return -1;

		uint64_t dword;
		unsigned int address = 0;

		while (fread(&dword, 8, 1, fp) == 1)
		{
			printf("%5x: %016lx\n", address, dword);
			address += 8;
			disassemble(dword);
		}

		return 0;
	}
}


