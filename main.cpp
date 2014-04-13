/*
 * main.cpp
 *
 *  Created on: 7 Apr 2014
 *      Author: simon
 */

#include <stdio.h>
#include <assert.h>
#include <stdint.h>

#include "shared.h"

unsigned int code[] = {
#include "shader_256.hex"
};

const char *pMuxEncodingNames[8] = {
		"acc0",
		"acc1",
		"acc2",
		"acc3",
		"acc4",
		"acc5",
		"ra",
		"rb",
};

const char *pConditionCodeNames[8] = {
		"never",
		"al",
		"zs",
		"zc",
		"ns",
		"nc",
		"cs",
		"cc",
};

const char *pBranchConditionNames[16] = {
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

const char *pMulOpNames[8] = {
		"mulnop",
		"fmul",
		"mul24",
		"v8muld",
		"v8min",
		"v8max",
		"mulv8adds",
		"mulv8subs",
};

const char *pSmallImm[64] = {
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15",
	"-16", "-15", "-14", "-13", "-12", "-11", "-10", "-9", "-8", "-7", "-6", "-5", "-4", "-3", "-2", "-1",
	"1.0","2.0", "4.0", "8.0", "16.0", "32.0", "64.0", "128.0",
	"1.0/256.0", "1.0/128.0", "1.0/64.0", "1.0/32.0", "1.0/16.0", "1.0/8.0", "1.0/4.0", "1.0/2.0",
	"vecrot acc5", "vecrot 1", "vecrot 2", "vecrot 3", "vecrot 4", "vecrot 5", "vecrot 6", "vecrot 7", "vecrot 8", "vecrot 9", "vecrot 10", "vecrot 11", "vecrot 12", "vecrot 13", "vecrot 14", "vecrot 15"
};

void packunpack(uint64_t dword)
{
	uint64_t pack, unpack;

	pack = (dword >> 52) & 0xf;
	unpack = (dword >> 57) & 0x7;

	assert(pack == 0 && unpack == 0);

	if (0)
		if (pack || unpack)
		{
			if ((dword >> 56) & 1)
			{
				fprintf(stderr, "pm=1 ");
			}
			else
				fprintf(stderr, "pm=0 ");

			fprintf(stderr, "pack %2lx unpack %lx ", pack, unpack);
		}
}

void cond_add_mul(uint64_t dword, ConditionCode &rAdd, ConditionCode &rMul)
{
	uint64_t add = (dword >> 49) & 0x7;
	uint64_t mul = (dword >> 46) & 0x7;

	rAdd = (ConditionCode)add;
	rMul = (ConditionCode)mul;

	if (0)
		fprintf(stderr, "cond_add=%s cond_mul=%s ", pConditionCodeNames[add], pConditionCodeNames[mul]);
}

bool set_flags(uint64_t dword)
{
	if (0)
		if ((dword >> 45) & 1)
			fprintf(stderr, "sf=%ld ", (dword >> 45) & 1);
	return (bool)((dword >> 45) & 1);
}

bool write_swap(uint64_t dword)
{
	if (0)
		if ((dword >> 44) & 1)
			fprintf(stderr, "ws=%ld ", (dword >> 44) & 1);
	return (bool)((dword >> 44) & 1);
}

void waddr(uint64_t dword, bool swap, ConditionCode enableA, ConditionCode enableB, uint32_t &rDestAdd, uint32_t &rDestMul)
{
	if (0)
	{
		if (enableA != kNever)
			fprintf(stderr, "add=%c%ld ", swap ? 'B' : 'A', (dword >> 38) & 63);

		if (enableB != kNever)
			fprintf(stderr, "mul=%c%ld ", swap ? 'A' : 'B', (dword >> 32) & 63);
	}

	rDestAdd = (dword >> 38) & 63;
	rDestMul = (dword >> 32) & 63;
}

uint32_t immediate32(uint64_t dword)
{
	if (0)
		fprintf(stderr, "imm32=%lx/%ld ",
				dword & 0xffffffff,
				dword & 0xffffffff);
	return dword & 0xffffffff;
}

Signal signal(uint64_t dword)
{
	if (0)
		if ((Signal)(dword >> 60) != kNoSignal)
			fprintf(stderr, "sig=%s ", pSignalNames[dword >> 60]);

	return (Signal)(dword >> 60);
}

void op_mul_add(uint64_t dword, ConditionCode enableA, ConditionCode enableB, AddOp &rAdd, MulOp &rMul)
{
	rAdd = (AddOp)((dword >> 24) & 31);
	rMul = (MulOp)((dword >> 29) & 7);

	if (0)
	{
		if (!(enableA == kNever && rAdd == kAddNop))
			fprintf(stderr, "addop=%s ", pAddOpNames[(dword >> 24) & 31]);
		if (!(enableB == kNever && rMul == kMulNop))
			fprintf(stderr, "mulop=%s ", pMulOpNames[(dword >> 29) & 7]);

		if (rAdd == kAddNop && rMul == kMulNop)
			fprintf(stderr, "nop \n");
	}
}

MuxEncoding mux_decode(uint64_t section)
{
	MuxEncoding m = (MuxEncoding)section;
	if (0)
		fprintf(stderr, "%s ", pMuxEncodingNames[section]);
	return m;
}

void emit_il(ConditionCode cc, bool setFlags, bool swapOut, uint32_t dest, uint32_t imm)
{
	fprintf(stderr, "il");
	if (cc != kAlways)
		fprintf(stderr, "%s", pConditionCodeNames[cc]);
	if (setFlags)
		fprintf(stderr, "s");

	fprintf(stderr, " ");

	if (!swapOut)
		fprintf(stderr, "ra");
	else
		fprintf(stderr, "rb");
	fprintf(stderr, "%d", dest);

	fprintf(stderr, ", %d", imm);
}

void emit_alu(ConditionCode cc, bool setFlags, bool swapOut, uint32_t dest, const char *pOp, MuxEncoding muxA, MuxEncoding muxB, uint64_t ra, uint64_t rb)
{
	fprintf(stderr, "%s", pOp);
	if (setFlags)
		fprintf(stderr, "s");
	if (cc != kAlways)
		fprintf(stderr, "%s", pConditionCodeNames[cc]);

	fprintf(stderr, " ");

	if (swapOut)
		fprintf(stderr, "rb");
	else
		fprintf(stderr, "ra");
	fprintf(stderr, "%d, ", dest);

	if (muxA == kRfA)
		fprintf(stderr, "%s%ld", pMuxEncodingNames[muxA], ra);
	else if (muxA == kRfB)
		fprintf(stderr, "%s%ld", pMuxEncodingNames[muxA], rb);
	else
		fprintf(stderr, "%s", pMuxEncodingNames[muxA]);

	fprintf(stderr, ", ");

	if (muxB == kRfA)
		fprintf(stderr, "%s%ld", pMuxEncodingNames[muxB], ra);
	else if (muxB == kRfB)
		fprintf(stderr, "%s%ld", pMuxEncodingNames[muxB], rb);
	else
		fprintf(stderr, "%s", pMuxEncodingNames[muxB]);
}

void emit_alu_small(bool isAdd, ConditionCode cc, bool setFlags, bool swapOut, uint32_t dest, const char *pOp, MuxEncoding muxA, MuxEncoding muxB, uint64_t ra, uint64_t imm)
{
	fprintf(stderr, "%s", pOp);
	if (setFlags)
		fprintf(stderr, "s");
	if (cc != kAlways)
		fprintf(stderr, "%s", pConditionCodeNames[cc]);

	fprintf(stderr, " ");

	if (swapOut)
		fprintf(stderr, "rb");
	else
		fprintf(stderr, "ra");
	fprintf(stderr, "%d, ", dest);

	if (muxA == kRfA)
		fprintf(stderr, "%s%ld", pMuxEncodingNames[muxA], ra);
	else if (muxA == kRfB)
		assert(0);
	else
		fprintf(stderr, "%s", pMuxEncodingNames[muxA]);

	fprintf(stderr, ", ");
	
	if (muxB == kRfA || muxB == kRfB)
		fprintf(stderr, "%s", pSmallImm[imm]);
	else
	{
		fprintf(stderr, "%s", pMuxEncodingNames[muxB]);

		if (!isAdd)
			fprintf(stderr, " %s", pSmallImm[imm]);
	}
}

bool disassemble(uint64_t dword, uint32_t address)
{
	ConditionCode addCc, mulCc;
	AddOp addOp;
	MulOp mulOp;
	uint32_t destAdd, destMul;
	bool flags;
	bool swap;

	if ((dword >> 57) == 0x74)
	{
		cond_add_mul(dword, addCc, mulCc);
		flags = set_flags(dword);
		swap = write_swap(dword);
		waddr(dword, write_swap(dword), addCc, mulCc, destAdd, destMul);

		assert(addCc == kNever && mulCc == kNever);

		uint32_t sem = (uint32_t)(dword & 15);
		bool inc = (bool)((dword >> 4) & 1);

		fprintf(stderr, "sem %d, %s\n", sem, inc ? "inc" : "dec");

		return true;
	}
	else if ((dword >> 57) == 0x73)
	{
		fprintf(stderr, "load imm per-elmt unsigned\n");
		packunpack(dword);
		cond_add_mul(dword, addCc, mulCc);
		set_flags(dword);
		waddr(dword, write_swap(dword), addCc, mulCc, destAdd, destMul);
		fprintf(stderr, "\n");
		return false;
	}
	else if ((dword >> 57) == 0x71)
	{
		fprintf(stderr, "load imm per-elmt signed\n");
		packunpack(dword);
		cond_add_mul(dword, addCc, mulCc);
		set_flags(dword);
		write_swap(dword);
		waddr(dword, write_swap(dword), addCc, mulCc, destAdd, destMul);
		fprintf(stderr, "\n");
		return false;
	}
	else if ((dword >> 57) == 0x70)
	{
		packunpack(dword);
		cond_add_mul(dword, addCc, mulCc);
		flags = set_flags(dword);
		swap = write_swap(dword);
		waddr(dword, write_swap(dword), addCc, mulCc, destAdd, destMul);
		uint32_t imm = immediate32(dword);

		if (addCc != kNever)
			emit_il(addCc, flags, swap, destAdd, imm);

		if (mulCc != kNever)
		{
			fprintf(stderr, "; ");
			emit_il(mulCc, flags, !swap, destMul, imm);
		}

		fprintf(stderr, "\n");
		return true;
	}
	else if (((dword >> 57) & 0x78) == 0x78)
	{
		swap = write_swap(dword);
		waddr(dword, write_swap(dword), kAlways, kAlways, destAdd, destMul);
		bool rel = (bool)((dword >> 51) & 1);
		bool reg = (bool)((dword >> 50) & 1);
		BranchCondition cc = (BranchCondition)((dword >> 52) & 15);
		uint64_t raddr = (dword >> 45) & 31;
		uint32_t imm = immediate32(dword);

		fprintf(stderr, "bl");
		if (!rel)
			fprintf(stderr, "a");
		fprintf(stderr, "%s", pBranchConditionNames[cc]);

		fprintf(stderr, " %s%d, %s%d, ",
			swap ? "rb" : "ra", destAdd,
			swap ? "ra" : "rb", destMul);

		if (reg)
		{
			fprintf(stderr, "ra%ld", raddr);
			if (imm)
				fprintf(stderr, ", ");
		}

		if (imm || !reg)
			fprintf(stderr, "%d", imm);

		fprintf(stderr, "\n");
		return true;
	}
	else if (((dword >> 57) & 0x78) == 0x68)
	{
		bool have_prev = false;

		packunpack(dword);
		cond_add_mul(dword, addCc, mulCc);
		flags = set_flags(dword);
		swap = write_swap(dword);
		waddr(dword, write_swap(dword), addCc, mulCc, destAdd, destMul);
		op_mul_add(dword, addCc, mulCc, addOp, mulOp);

		if (!(addCc == kNever && addOp == kAddNop))
		{
			emit_alu_small(true, addCc, flags, swap, destAdd, pAddOpNames[addOp],
					mux_decode((dword >> 9) & 7), mux_decode((dword >> 6) & 7),
					(dword >> 18) & 63, (dword >> 12) & 63);
			have_prev = true;
		}

		if (!(mulCc == kNever && mulOp == kMulNop))
		{
			if (have_prev)
				fprintf(stderr, "; ");

			emit_alu_small(false, mulCc, flags, !swap, destMul, pMulOpNames[mulOp],
					mux_decode((dword >> 3) & 7), mux_decode(dword & 7),
					(dword >> 18) & 63, (dword >> 12) & 63);
			have_prev = true;
		}

		if (!have_prev)		//we have nothing
			fprintf(stderr, "nop");

		fprintf(stderr, "\n");
		return true;
	}
	else
	{
		bool have_prev = false;

		packunpack(dword);
		cond_add_mul(dword, addCc, mulCc);
		flags = set_flags(dword);
		swap = write_swap(dword);
		waddr(dword, write_swap(dword), addCc, mulCc, destAdd, destMul);
		Signal sig = signal(dword);
		op_mul_add(dword, addCc, mulCc, addOp, mulOp);

		if (!(addCc == kNever && addOp == kAddNop))
		{
			emit_alu(addCc, flags, swap, destAdd, pAddOpNames[addOp],
					mux_decode((dword >> 9) & 7), mux_decode((dword >> 6) & 7),
					(dword >> 18) & 63, (dword >> 12) & 63);
			have_prev = true;
		}

		if (!(mulCc == kNever && mulOp == kMulNop))
		{
			if (have_prev)
				fprintf(stderr, "; ");

			emit_alu(mulCc, flags, !swap, destMul, pMulOpNames[mulOp],
					mux_decode((dword >> 3) & 7), mux_decode(dword & 7),
					(dword >> 18) & 63, (dword >> 12) & 63);
			have_prev = true;
		}

		if (sig != kNoSignal)
		{
			if (have_prev)
				fprintf(stderr, "; ");
			fprintf(stderr, "%s", pSignalNames[sig]);
		}

		if (!have_prev && sig == kNoSignal)		//we have nothing
			fprintf(stderr, "nop");

		fprintf(stderr, "\n");
		return true;
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
			if (!disassemble(dword, address))
				fprintf(stderr, "%5x: %016lx\n", address, dword);
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
			fprintf(stderr, "%5x: %016lx\n", address, dword);
			disassemble(dword, address);
			address += 8;
		}

		return 0;
	}
}


