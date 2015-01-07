/*
** $Id: lopcodes.h,v 1.148 2014/10/25 11:50:46 roberto Exp $
** Opcodes for Lua virtual machine
** See Copyright Notice in lua.h
*/

#ifndef lopcodes_h
#define lopcodes_h

#include "llimits.h"


/*===========================================================================
  We assume that instructions are unsigned numbers.
  All instructions have an opcode in the first 6 bits.
  Instructions can have the following fields:
	'A' : 8 bits
	'B' : 9 bits
	'C' : 9 bits
	'Ax' : 26 bits ('A', 'B', and 'C' together)
	'Bx' : 18 bits ('B' and 'C' together)
	'sBx' : signed Bx

  A signed argument is represented in excess K; that is, the number
  value is the unsigned value minus K. K is exactly the maximum value
  for that argument (so that -max is represented by 0, and +max is
  represented by 2*max), which is half the maximum for the corresponding
  unsigned argument.
===========================================================================*/


enum OpMode {iABC, iABx, iAsBx, iAx};  /* basic instruction format */


/*
** size and position of opcode arguments.
*/
#define SIZE_C		9
#define SIZE_B		9
#define SIZE_Bx		(SIZE_C + SIZE_B)
#define SIZE_A		8
#define SIZE_Ax		(SIZE_C + SIZE_B + SIZE_A)

#define SIZE_OP		6

#define POS_OP		0
#define POS_A		(POS_OP + SIZE_OP)
#define POS_C		(POS_A + SIZE_A)
#define POS_B		(POS_C + SIZE_C)
#define POS_Bx		POS_C
#define POS_Ax		POS_A


/*
** limits for opcode arguments.
** we use (signed) int to manipulate most arguments,
** so they must fit in LUAI_BITSINT-1 bits (-1 for sign)
*/
#if SIZE_Bx < LUAI_BITSINT-1
#define MAXARG_Bx        ((1<<SIZE_Bx)-1)
#define MAXARG_sBx        (MAXARG_Bx>>1)         /* 'sBx' is signed */
#else
#define MAXARG_Bx        MAX_INT
#define MAXARG_sBx        MAX_INT
#endif

#if SIZE_Ax < LUAI_BITSINT-1
#define MAXARG_Ax	((1<<SIZE_Ax)-1)
#else
#define MAXARG_Ax	MAX_INT
#endif


#define MAXARG_A        ((1<<SIZE_A)-1)
#define MAXARG_B        ((1<<SIZE_B)-1)
#define MAXARG_C        ((1<<SIZE_C)-1)


/* creates a mask with 'n' 1 bits at position 'p' */
#define MASK1(n,p)	((~((~(Instruction)0)<<(n)))<<(p))

/* creates a mask with 'n' 0 bits at position 'p' */
#define MASK0(n,p)	(~MASK1(n,p))

/*
** the following macros help to manipulate instructions
*/

#define GET_OPCODE(i)	(cast(OpCode, ((i)>>POS_OP) & MASK1(SIZE_OP,0)))
#define SET_OPCODE(i,o)	((i) = (((i)&MASK0(SIZE_OP,POS_OP)) | \
		((cast(Instruction, o)<<POS_OP)&MASK1(SIZE_OP,POS_OP))))

#define getarg(i,pos,size)	(cast(int, ((i)>>pos) & MASK1(size,0)))
#define setarg(i,v,pos,size)	((i) = (((i)&MASK0(size,pos)) | \
                ((cast(Instruction, v)<<pos)&MASK1(size,pos))))

#define GETARG_A(i)	getarg(i, POS_A, SIZE_A)
#define SETARG_A(i,v)	setarg(i, v, POS_A, SIZE_A)

#define GETARG_B(i)	getarg(i, POS_B, SIZE_B)
#define SETARG_B(i,v)	setarg(i, v, POS_B, SIZE_B)

#define GETARG_C(i)	getarg(i, POS_C, SIZE_C)
#define SETARG_C(i,v)	setarg(i, v, POS_C, SIZE_C)

#define GETARG_Bx(i)	getarg(i, POS_Bx, SIZE_Bx)
#define SETARG_Bx(i,v)	setarg(i, v, POS_Bx, SIZE_Bx)

#define GETARG_Ax(i)	getarg(i, POS_Ax, SIZE_Ax)
#define SETARG_Ax(i,v)	setarg(i, v, POS_Ax, SIZE_Ax)

#define GETARG_sBx(i)	(GETARG_Bx(i)-MAXARG_sBx)
#define SETARG_sBx(i,b)	SETARG_Bx((i),cast(unsigned int, (b)+MAXARG_sBx))

#define CREATE_ABC(o,a,b,c)	((cast(Instruction, o)<<POS_OP) \
			| (cast(Instruction, a)<<POS_A) \
			| (cast(Instruction, b)<<POS_B) \
			| (cast(Instruction, c)<<POS_C))

#define CREATE_ABx(o,a,bc)	((cast(Instruction, o)<<POS_OP) \
			| (cast(Instruction, a)<<POS_A) \
			| (cast(Instruction, bc)<<POS_Bx))

#define CREATE_Ax(o,a)		((cast(Instruction, o)<<POS_OP) \
			| (cast(Instruction, a)<<POS_Ax))

#define RAVI_GET_OPCODE(i)	(cast(OpCode, ((i)>>SIZE_OP) & MASK1(10,0)))
#define RAVI_SET_OPCODE(i,o)	((i) = (((i)&MASK0(16,0)) | \
		(((cast(Instruction, o)<<SIZE_OP)|OP_RAVI)&MASK1(SIZE_OP,POS_OP))))

#define RAVI_GETARG_A(i)	getarg(i, 16, 16)
#define RAVI_SETARG_A(i,v)	setarg(i, v, 16, 16)

#define RAVI_GETARG_B(i)	getarg(i, 0, 16)
#define RAVI_SETARG_B(i,v)	setarg(i, v, 0, 16)

#define RAVI_GETARG_C(i)	getarg(i, 16, 16)
#define RAVI_SETARG_C(i,v)	setarg(i, v, 16, 16)

/* A (16 bits) Opcode (10 bits) OP_RAVI (6 bits) */
#define RAVI_CREATE_A(o,a) ((cast(Instruction, a)<<16) \
                         |  (cast(Instruction, o)<<6) \
                         |  cast(Instruction, OP_RAVI))

/* B (16 bits) C (16 bits) */
#define RAVI_CREATE_BC(b,c)	((cast(Instruction, c)<<16) \
                           | (cast(Instruction, b)))


/*
** Macros to operate RK indices
*/

/* this bit 1 means constant (0 means register) */
#define BITRK		(1 << (SIZE_B - 1))

/* test whether value is a constant */
#define ISK(x)		((x) & BITRK)

/* gets the index of the constant */
#define INDEXK(r)	((int)(r) & ~BITRK)

#define MAXINDEXRK	(BITRK - 1)

/* code a constant index as a RK value */
#define RKASK(x)	((x) | BITRK)


/*
** invalid register that fits in 8 bits
*/
#define NO_REG		MAXARG_A


/*
** R(x) - register
** Kst(x) - constant (in constant table)
** RK(x) == if ISK(x) then Kst(INDEXK(x)) else R(x)
*/


/*
** grep "ORDER OP" if you change these enums
*/

typedef enum {
/*----------------------------------------------------------------------
name		args	description
------------------------------------------------------------------------*/
OP_MOVE,/*	A B	R(A) := R(B)					*/
OP_LOADK,/*	A Bx	R(A) := Kst(Bx)					*/
OP_LOADKX,/*	A 	R(A) := Kst(extra arg)				*/
OP_LOADBOOL,/*	A B C	R(A) := (Bool)B; if (C) pc++			*/
OP_LOADNIL,/*	A B	R(A), R(A+1), ..., R(A+B) := nil		*/
OP_GETUPVAL,/*	A B	R(A) := UpValue[B]				*/

OP_GETTABUP,/*	A B C	R(A) := UpValue[B][RK(C)]			*/
OP_GETTABLE,/*	A B C	R(A) := R(B)[RK(C)]				*/

OP_SETTABUP,/*	A B C	UpValue[A][RK(B)] := RK(C)			*/
OP_SETUPVAL,/*	A B	UpValue[B] := R(A)				*/
OP_SETTABLE,/*	A B C	R(A)[RK(B)] := RK(C)				*/

OP_NEWTABLE,/*	A B C	R(A) := {} (size = B,C)				*/

OP_SELF,/*	A B C	R(A+1) := R(B); R(A) := R(B)[RK(C)]		*/

OP_ADD,/*	A B C	R(A) := RK(B) + RK(C)				*/
OP_SUB,/*	A B C	R(A) := RK(B) - RK(C)				*/
OP_MUL,/*	A B C	R(A) := RK(B) * RK(C)				*/
OP_MOD,/*	A B C	R(A) := RK(B) % RK(C)				*/
OP_POW,/*	A B C	R(A) := RK(B) ^ RK(C)				*/
OP_DIV,/*	A B C	R(A) := RK(B) / RK(C)				*/
OP_IDIV,/*	A B C	R(A) := RK(B) // RK(C)				*/
OP_BAND,/*	A B C	R(A) := RK(B) & RK(C)				*/
OP_BOR,/*	A B C	R(A) := RK(B) | RK(C)				*/
OP_BXOR,/*	A B C	R(A) := RK(B) ~ RK(C)				*/
OP_SHL,/*	A B C	R(A) := RK(B) << RK(C)				*/
OP_SHR,/*	A B C	R(A) := RK(B) >> RK(C)				*/
OP_UNM,/*	A B	R(A) := -R(B)					*/
OP_BNOT,/*	A B	R(A) := ~R(B)					*/
OP_NOT,/*	A B	R(A) := not R(B)				*/
OP_LEN,/*	A B	R(A) := length of R(B)				*/

OP_CONCAT,/*	A B C	R(A) := R(B).. ... ..R(C)			*/

OP_JMP,/*	A sBx	pc+=sBx; if (A) close all upvalues >= R(A - 1)	*/
OP_EQ,/*	A B C	if ((RK(B) == RK(C)) ~= A) then pc++		*/
OP_LT,/*	A B C	if ((RK(B) <  RK(C)) ~= A) then pc++		*/
OP_LE,/*	A B C	if ((RK(B) <= RK(C)) ~= A) then pc++		*/

OP_TEST,/*	A C	if not (R(A) <=> C) then pc++			*/
OP_TESTSET,/*	A B C	if (R(B) <=> C) then R(A) := R(B) else pc++	*/

OP_CALL,/*	A B C	R(A), ... ,R(A+C-2) := R(A)(R(A+1), ... ,R(A+B-1)) */
OP_TAILCALL,/*	A B C	return R(A)(R(A+1), ... ,R(A+B-1))		*/
OP_RETURN,/*	A B	return R(A), ... ,R(A+B-2)	(see note)	*/

OP_FORLOOP,/*	A sBx	R(A)+=R(A+2);
			if R(A) <?= R(A+1) then { pc+=sBx; R(A+3)=R(A) }*/
OP_FORPREP,/*	A sBx	R(A)-=R(A+2); pc+=sBx				*/

OP_TFORCALL,/*	A C	R(A+3), ... ,R(A+2+C) := R(A)(R(A+1), R(A+2));	*/
OP_TFORLOOP,/*	A sBx	if R(A+1) ~= nil then { R(A)=R(A+1); pc += sBx }*/

OP_SETLIST,/*	A B C	R(A)[(C-1)*FPF+i] := R(A+i), 1 <= i <= B	*/

OP_CLOSURE,/*	A Bx	R(A) := closure(KPROTO[Bx])			*/

OP_VARARG,/*	A B	R(A), R(A+1), ..., R(A+B-2) = vararg		*/
OP_EXTRAARG,/*	Ax	extra (larger) argument for previous opcode	*/

OP_RAVI,        /* Extension point for RAVI */

OP_RAVI_UNMF,  /*	A B	R(A) := -R(B) floating point      */
OP_RAVI_UNMI,  /*   A B R(A) := -R(B) integer */

OP_RAVI_ADDFFKK,/*	A B C	R(A) := Kst(B) + Kst(C)				*/
OP_RAVI_ADDFFKR,/*	A B C	R(A) := Kst(B) + R(C)				*/
OP_RAVI_ADDFFRK,/*	A B C	R(A) := R(B) + Kst(C)				*/
OP_RAVI_ADDFFRR,/*	A B C	R(A) := R(B) + R(C)				*/
OP_RAVI_ADDFIKK,/*	A B C	R(A) := Kst(B) + Kst(C)				*/
OP_RAVI_ADDFIKR,/*	A B C	R(A) := Kst(B) + R(C)				*/
OP_RAVI_ADDFIRK,/*	A B C	R(A) := R(B) + Kst(C)				*/
OP_RAVI_ADDFIRR,/*	A B C	R(A) := R(B) + R(C)				*/
OP_RAVI_ADDIFKK,/*	A B C	R(A) := Kst(B) + Kst(C)				*/
OP_RAVI_ADDIFKR,/*	A B C	R(A) := Kst(B) + R(C)				*/
OP_RAVI_ADDIFRK,/*	A B C	R(A) := R(B) + Kst(C)				*/
OP_RAVI_ADDIFRR,/*	A B C	R(A) := R(B) + R(C)				*/
OP_RAVI_ADDIIKK,/*	A B C	R(A) := Kst(B) + Kst(C)				*/
OP_RAVI_ADDIIKR,/*	A B C	R(A) := Kst(B) + R(C)				*/
OP_RAVI_ADDIIRK,/*	A B C	R(A) := R(B) + Kst(C)				*/
OP_RAVI_ADDIIRR,/*	A B C	R(A) := R(B) + R(C)				*/

OP_RAVI_SUBFFKK,/*	A B C	R(A) := Kst(B) - Kst(C)				*/
OP_RAVI_SUBFFKR,/*	A B C	R(A) := Kst(B) - R(C)				*/
OP_RAVI_SUBFFRK,/*	A B C	R(A) := R(B) - Kst(C)				*/
OP_RAVI_SUBFFRR,/*	A B C	R(A) := R(B) - R(C)				*/
OP_RAVI_SUBFIKK,/*	A B C	R(A) := Kst(B) - Kst(C)				*/
OP_RAVI_SUBFIKR,/*	A B C	R(A) := Kst(B) - R(C)				*/
OP_RAVI_SUBFIRK,/*	A B C	R(A) := R(B) - Kst(C)				*/
OP_RAVI_SUBFIRR,/*	A B C	R(A) := R(B) - R(C)				*/
OP_RAVI_SUBIFKK,/*	A B C	R(A) := Kst(B) - Kst(C)				*/
OP_RAVI_SUBIFKR,/*	A B C	R(A) := Kst(B) - R(C)				*/
OP_RAVI_SUBIFRK,/*	A B C	R(A) := R(B) - Kst(C)				*/
OP_RAVI_SUBIFRR,/*	A B C	R(A) := R(B) - R(C)				*/
OP_RAVI_SUBIIKK,/*	A B C	R(A) := Kst(B) - Kst(C)				*/
OP_RAVI_SUBIIKR,/*	A B C	R(A) := Kst(B) - R(C)				*/
OP_RAVI_SUBIIRK,/*	A B C	R(A) := R(B) - Kst(C)				*/
OP_RAVI_SUBIIRR,/*	A B C	R(A) := R(B) - R(C)				*/

OP_RAVI_MULFFKK,/*	A B C	R(A) := Kst(B) * Kst(C)				*/
OP_RAVI_MULFFKR,/*	A B C	R(A) := Kst(B) * R(C)				*/
OP_RAVI_MULFFRK,/*	A B C	R(A) := R(B) * Kst(C)				*/
OP_RAVI_MULFFRR,/*	A B C	R(A) := R(B) * R(C)				*/
OP_RAVI_MULFIKK,/*	A B C	R(A) := Kst(B) * Kst(C)				*/
OP_RAVI_MULFIKR,/*	A B C	R(A) := Kst(B) * R(C)				*/
OP_RAVI_MULFIRK,/*	A B C	R(A) := R(B) * Kst(C)				*/
OP_RAVI_MULFIRR,/*	A B C	R(A) := R(B) * R(C)				*/
OP_RAVI_MULIFKK,/*	A B C	R(A) := Kst(B) * Kst(C)				*/
OP_RAVI_MULIFKR,/*	A B C	R(A) := Kst(B) * R(C)				*/
OP_RAVI_MULIFRK,/*	A B C	R(A) := R(B) * Kst(C)				*/
OP_RAVI_MULIFRR,/*	A B C	R(A) := R(B) * R(C)				*/
OP_RAVI_MULIIKK,/*	A B C	R(A) := Kst(B) * Kst(C)				*/
OP_RAVI_MULIIKR,/*	A B C	R(A) := Kst(B) * R(C)				*/
OP_RAVI_MULIIRK,/*	A B C	R(A) := R(B) * Kst(C)				*/
OP_RAVI_MULIIRR,/*	A B C	R(A) := R(B) * R(C)				*/

OP_RAVI_DIVFFKK,/*	A B C	R(A) := Kst(B) / Kst(C)				*/
OP_RAVI_DIVFFKR,/*	A B C	R(A) := Kst(B) / R(C)				*/
OP_RAVI_DIVFFRK,/*	A B C	R(A) := R(B) / Kst(C)				*/
OP_RAVI_DIVFFRR,/*	A B C	R(A) := R(B) / R(C)				*/
OP_RAVI_DIVFIKK,/*	A B C	R(A) := Kst(B) / Kst(C)				*/
OP_RAVI_DIVFIKR,/*	A B C	R(A) := Kst(B) / R(C)				*/
OP_RAVI_DIVFIRK,/*	A B C	R(A) := R(B) / Kst(C)				*/
OP_RAVI_DIVFIRR,/*	A B C	R(A) := R(B) / R(C)				*/
OP_RAVI_DIVIFKK,/*	A B C	R(A) := Kst(B) / Kst(C)				*/
OP_RAVI_DIVIFKR,/*	A B C	R(A) := Kst(B) / R(C)				*/
OP_RAVI_DIVIFRK,/*	A B C	R(A) := R(B) / Kst(C)				*/
OP_RAVI_DIVIFRR,/*	A B C	R(A) := R(B) / R(C)				*/
OP_RAVI_DIVIIKK,/*	A B C	R(A) := Kst(B) / Kst(C)				*/
OP_RAVI_DIVIIKR,/*	A B C	R(A) := Kst(B) / R(C)				*/
OP_RAVI_DIVIIRK,/*	A B C	R(A) := R(B) / Kst(C)				*/
OP_RAVI_DIVIIRR,/*	A B C	R(A) := R(B) / R(C)				*/

OP_RAVI_EQFFKK,/*	A B C	if ((Kst(B) == Kst(C)) ~= A) then pc++		*/
OP_RAVI_EQFFKR,/*	A B C	if ((Kst(B) == R(C)) ~= A) then pc++		*/
OP_RAVI_EQFFRK,/*	A B C	if ((R(B) == Kst(C)) ~= A) then pc++		*/
OP_RAVI_EQFFRR,/*	A B C	if ((R(B) == R(C)) ~= A) then pc++		*/
OP_RAVI_EQIIKK,/*	A B C	if ((Kst(B) == Kst(C)) ~= A) then pc++		*/
OP_RAVI_EQIIKR,/*	A B C	if ((Kst(B) == R(C)) ~= A) then pc++		*/
OP_RAVI_EQIIRK,/*	A B C	if ((R(B) == Kst(C)) ~= A) then pc++		*/
OP_RAVI_EQIIRR,/*	A B C	if ((R(B) == R(C)) ~= A) then pc++		*/

OP_RAVI_LTFFKK,/*	A B C	if ((Kst(B) < Kst(C)) ~= A) then pc++		*/
OP_RAVI_LTFFKR,/*	A B C	if ((Kst(B) < R(C)) ~= A) then pc++		*/
OP_RAVI_LTFFRK,/*	A B C	if ((R(B) < Kst(C)) ~= A) then pc++		*/
OP_RAVI_LTFFRR,/*	A B C	if ((R(B) < R(C)) ~= A) then pc++		*/
OP_RAVI_LTIIKK,/*	A B C	if ((Kst(B) < Kst(C)) ~= A) then pc++		*/
OP_RAVI_LTIIKR,/*	A B C	if ((Kst(B) < R(C)) ~= A) then pc++		*/
OP_RAVI_LTIIRK,/*	A B C	if ((R(B) < Kst(C)) ~= A) then pc++		*/
OP_RAVI_LTIIRR,/*	A B C	if ((R(B) < R(C)) ~= A) then pc++		*/

OP_RAVI_LEFFKK,/*	A B C	if ((Kst(B) <= Kst(C)) ~= A) then pc++		*/
OP_RAVI_LEFFKR,/*	A B C	if ((Kst(B) <= R(C)) ~= A) then pc++		*/
OP_RAVI_LEFFRK,/*	A B C	if ((R(B) <= Kst(C)) ~= A) then pc++		*/
OP_RAVI_LEFFRR,/*	A B C	if ((R(B) <= R(C)) ~= A) then pc++		*/
OP_RAVI_LEIIKK,/*	A B C	if ((Kst(B) <= Kst(C)) ~= A) then pc++		*/
OP_RAVI_LEIIKR,/*	A B C	if ((Kst(B) <= R(C)) ~= A) then pc++		*/
OP_RAVI_LEIIRK,/*	A B C	if ((R(B) <= Kst(C)) ~= A) then pc++		*/
OP_RAVI_LEIIRR,/*	A B C	if ((R(B) <= R(C)) ~= A) then pc++		*/

OP_RAVI_ARRAYGET_SIK,/*	A B C	R(A) := R(B)[Kst(C)]				*/
OP_RAVI_ARRAYGET_SIR,/*	A B C	R(A) := R(B)[R(C)]				*/
OP_RAVI_ARRAYGET_IIK,/*	A B C	R(A) := R(B)[Kst(C)]				*/
OP_RAVI_ARRAYGET_IIR,/*	A B C	R(A) := R(B)[R(C)]				*/
OP_RAVI_ARRAYGET_FIK,/*	A B C	R(A) := R(B)[Kst(C)]				*/
OP_RAVI_ARRAYGET_FIR,/*	A B C	R(A) := R(B)[R(C)]				*/
OP_RAVI_ARRAYGET_LIK,/*	A B C	R(A) := R(B)[Kst(C)]				*/
OP_RAVI_ARRAYGET_LIR,/*	A B C	R(A) := R(B)[R(C)]				*/

OP_RAVI_ARRAYSET_ISKK,/*	A B C	R(A)[Kst(B)] := Kst(C)				*/
OP_RAVI_ARRAYSET_ISKR,/*	A B C	R(A)[Kst(B)] := R(C)				*/
OP_RAVI_ARRAYSET_ISRK,/*	A B C	R(A)[R(B)] := Kst(C)				*/
OP_RAVI_ARRAYSET_ISRR,/*	A B C	R(A)[R(B)] := R(C)				*/
OP_RAVI_ARRAYSET_IIKK,/*	A B C	R(A)[Kst(B)] := Kst(C)				*/
OP_RAVI_ARRAYSET_IIKR,/*	A B C	R(A)[Kst(B)] := R(C)				*/
OP_RAVI_ARRAYSET_IIRK,/*	A B C	R(A)[R(B)] := Kst(C)				*/
OP_RAVI_ARRAYSET_IIRR,/*	A B C	R(A)[R(B)] := R(C)				*/
OP_RAVI_ARRAYSET_IFKK,/*	A B C	R(A)[Kst(B)] := Kst(C)				*/
OP_RAVI_ARRAYSET_IFKR,/*	A B C	R(A)[Kst(B)] := R(C)				*/
OP_RAVI_ARRAYSET_IFRK,/*	A B C	R(A)[R(B)] := Kst(C)				*/
OP_RAVI_ARRAYSET_IFRR,/*	A B C	R(A)[R(B)] := R(C)				*/
OP_RAVI_ARRAYSET_ILKK,/*	A B C	R(A)[Kst(B)] := Kst(C)				*/
OP_RAVI_ARRAYSET_ILKR,/*	A B C	R(A)[Kst(B)] := R(C)				*/
OP_RAVI_ARRAYSET_ILRK,/*	A B C	R(A)[R(B)] := Kst(C)				*/
OP_RAVI_ARRAYSET_ILRR,/*	A B C	R(A)[R(B)] := R(C)				*/

} OpCode;


#define NUM_OPCODES	(cast(int, OP_RAVI_ARRAYSET_ILRR) + 1)

/*===========================================================================
  Notes:
  (*) In OP_CALL, if (B == 0) then B = top. If (C == 0), then 'top' is
  set to last_result+1, so next open instruction (OP_CALL, OP_RETURN,
  OP_SETLIST) may use 'top'.

  (*) In OP_VARARG, if (B == 0) then use actual number of varargs and
  set top (like in OP_CALL with C == 0).

  (*) In OP_RETURN, if (B == 0) then return up to 'top'.

  (*) In OP_SETLIST, if (B == 0) then B = 'top'; if (C == 0) then next
  'instruction' is EXTRAARG(real C).

  (*) In OP_LOADKX, the next 'instruction' is always EXTRAARG.

  (*) For comparisons, A specifies what condition the test should accept
  (true or false).

  (*) All 'skips' (pc++) assume that next instruction is a jump.

===========================================================================*/


/*
** masks for instruction properties. The format is:
** bits 0-1: op mode
** bits 2-3: C arg mode
** bits 4-5: B arg mode
** bit 6: instruction set register A
** bit 7: operator is a test (next instruction must be a jump)
*/

enum OpArgMask {
  OpArgN,  /* argument is not used */
  OpArgU,  /* argument is used */
  OpArgR,  /* argument is a register or a jump offset */
  OpArgK   /* argument is a constant or register/constant */
};

LUAI_DDEC const lu_byte luaP_opmodes[NUM_OPCODES];

#define getOpMode(m)	(cast(enum OpMode, luaP_opmodes[m] & 3))
#define getBMode(m)	(cast(enum OpArgMask, (luaP_opmodes[m] >> 4) & 3))
#define getCMode(m)	(cast(enum OpArgMask, (luaP_opmodes[m] >> 2) & 3))
#define testAMode(m)	(luaP_opmodes[m] & (1 << 6))
#define testTMode(m)	(luaP_opmodes[m] & (1 << 7))


LUAI_DDEC const char *const luaP_opnames[NUM_OPCODES+1];  /* opcode names */


/* number of list items to accumulate before a SETLIST instruction */
#define LFIELDS_PER_FLUSH	50


#endif