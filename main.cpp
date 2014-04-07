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

void packunpack(uint64_t dword)
{
	if ((dword >> 56) & 1)
	{
		printf("pm=1 ");
	}
	else
		printf("pm=0 ");

	uint64_t pack, unpack;

	pack = (dword >> 52) & 0xf;
	unpack = (dword >> 57) & 0x7;

	printf("pack %2lx unpack %lx ", pack, unpack);
}

void cond_add_mul(uint64_t dword)
{
	uint64_t add = (dword >> 49) & 0x7;
	uint64_t mul = (dword >> 46) & 0x7;

	printf("cond_add=%s cond_mul=%s ", pConditionCodeNames[add], pConditionCodeNames[mul]);
}

void set_flags(uint64_t dword)
{
	printf("sf=%ld ", (dword >> 45) & 1);
}

void write_swap(uint64_t dword)
{
	printf("ws=%ld ", (dword >> 44) & 1);
}

void waddr(uint64_t dword)
{
	printf("add=%2ld mul=%2ld", (dword >> 38) & 63, (dword >> 32) & 63);
}

void disassemble(uint64_t dword)
{
	if ((dword >> 57) == 0x74)
	{
		printf("semaphore\n");
		packunpack(dword);
		cond_add_mul(dword);
		set_flags(dword);
		write_swap(dword);
		waddr(dword);
		printf("\n");
	}
	else if ((dword >> 57) == 0x73)
	{
		printf("load imm per-elmt unsigned\n");
		packunpack(dword);
		cond_add_mul(dword);
		set_flags(dword);
		write_swap(dword);
		waddr(dword);
		printf("\n");
	}
	else if ((dword >> 57) == 0x71)
	{
		printf("load imm per-elmt signed\n");
		packunpack(dword);
		cond_add_mul(dword);
		set_flags(dword);
		write_swap(dword);
		waddr(dword);
		printf("\n");
	}
	else if ((dword >> 57) == 0x70)
	{
		printf("load imm 32\n");
		packunpack(dword);
		cond_add_mul(dword);
		set_flags(dword);
		write_swap(dword);
		waddr(dword);
		printf("\n");
	}
	else if (((dword >> 57) & 0x78) == 0x78)
	{
		printf("branch\n");
		write_swap(dword);
		waddr(dword);
		printf("\n");
	}
	else if (((dword >> 57) & 0x78) == 0x68)
	{
		printf("alu small imm\n");
		packunpack(dword);
		cond_add_mul(dword);
		set_flags(dword);
		write_swap(dword);
		waddr(dword);
		printf("\n");
	}
	else
	{
		printf("alu\n");
		packunpack(dword);
		cond_add_mul(dword);
		waddr(dword);
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
		unsigned int address;

		while (fread(&dword, 8, 1, fp) == 1)
		{
			printf("%5x: %016lx\n", address, dword);
			address += 8;
			disassemble(dword);
		}

		return 0;
	}
}


