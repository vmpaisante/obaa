/*
 * RELIC is an Efficient LIbrary for Cryptography
 * Copyright (C) 2007-2014 RELIC Authors
 *
 * This file is part of RELIC. RELIC is legal property of its developers,
 * whose names are not listed here. Please refer to the COPYRIGHT file
 * for contact information.
 *
 * RELIC is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * RELIC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with RELIC. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file
 *
 * Implementation of the low-level prime field modular reduction functions.
 *
 * @version $Id$
 * @ingroup fp
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define BN_DIGIT 		64
#define FP_DIGS 		4

/**
 * Indicates that a comparison returned that the first argument was lesser than
 * the second argument.
 */
#define CMP_LT			-1

/**
 * Indicates that a comparison returned that the first argument was equal to
 * the second argument.
 */
#define CMP_EQ			0

/**
 * Indicates that a comparison returned that the first argument was greater than
 * the second argument.
 */
#define CMP_GT			1

typedef uint64_t dig_t;
typedef __uint128_t dbl_t;

dig_t prime[] = { 0xA700000000000013, 0x6121000000000013, 0xBA344D8000000008, 0x2523648240000001};

/*============================================================================*/
/* Private definitions                                                        */
/*============================================================================*/

/**
 * Accumulates a double precision digit in a triple register variable.
 *
 * @param[in,out] R2		- most significant word of the triple register.
 * @param[in,out] R1		- middle word of the triple register.
 * @param[in,out] R0		- lowest significant word of the triple register.
 * @param[in] A				- the first digit to multiply.
 * @param[in] B				- the second digit to multiply.
 */
#define COMBA_STEP(R2, R1, R0, A, B)										\
	dbl_t r = (dbl_t)(A) * (dbl_t)(B);										\
	dig_t _r = (R1);														\
	(R0) += (dig_t)(r);														\
	(R1) += (R0) < (dig_t)(r);												\
	(R2) += (R1) < _r;														\
	(R1) += (dig_t)(r >> (dbl_t)BN_DIGIT);									\
	(R2) += (R1) < (dig_t)(r >> (dbl_t)BN_DIGIT);							\

#define COMBA_STEP2(R2, R1, R0, A, B)										\
	__asm__ volatile (                                                		\
    	"movq  %6,%%rax     \n\t"                            				\
    	"mulq  %7           \n\t"                            				\
    	"addq  %%rax,%0     \n\t"                            				\
    	"adcq  %%rdx,%1     \n\t"                            				\
    	"adcq  $0,%2        \n\t"                            				\
    	: "=r" (R0), "=r" (R1), "=r" (R2)									\
    	: "0"(R0), "1"(R1), "2"(R2), "g"(A), "g"(B)							\
    	: "%rax","%rdx"												\
    );

/**
 * Accumulates a single precision digit in a triple register variable.
 *
 * @param[in,out] R2		- most significant word of the triple register.
 * @param[in,out] R1		- middle word of the triple register.
 * @param[in,out] R0		- lowest significant word of the triple register.
 * @param[in] A				- the first digit to accumulate.
 */
#define COMBA_ADD(R2, R1, R0, A)											\
	dig_t __r = (R1);														\
	(R0) += (A);															\
	(R1) += (R0) < (A);														\
	(R2) += (R1) < __r;														\

#define COMBA_ADD2(R2, R1, R0, A)											\
	__asm__ volatile (                                                		\
    	"addq  %6,%0     \n\t"                            					\
    	"adcq  $0,%1     \n\t"                            					\
    	"adcq  $0,%2     \n\t"                            					\
    	: "=r" (R0), "=r" (R1), "=r" (R2)									\
    	: "0"(R0), "1"(R1), "2"(R2), "g"(A) 								\
    	: 															\
    );

/*============================================================================*/
/* Public definitions                                                         */
/*============================================================================*/

dig_t fp_addn_low(dig_t *c, const dig_t *a, const dig_t *b) {
	int i;
	dig_t carry, c0, c1, r0, r1;

	carry = 0;
	for (i = 0; i < FP_DIGS; i++, a++, b++, c++) {
		r0 = (*a) + (*b);
		c0 = (r0 < (*a));
		r1 = r0 + carry;
		c1 = (r1 < r0);
		carry = c0 | c1;
		(*c) = r1;
	}
	return carry;
}

dig_t fp_subn_const(dig_t *c, const dig_t *a, const dig_t *b) {
	// Zero the carry. 
	__asm__ volatile (															
		"movq  0(%1),%%r8 \n\t"
		"subq  0(%2),%%r8 \n\t"
		"movq  %%r8,0(%0) \n\t"
		"movq  8(%1),%%r8 \n\t"
		"sbbq  8(%2),%%r8 \n\t"
		"movq  %%r8,8(%0) \n\t"		
		"movq  16(%1),%%r8 \n\t"
		"sbbq  16(%2),%%r8 \n\t"
		"movq  %%r8,16(%0) \n\t"
		"movq  24(%1),%%r8 \n\t"
		"sbbq  24(%2),%%r8 \n\t"
		"movq  %%r8,24(%0) \n\t"					
    	: "=r"(c)
    	: "r"(a), "r"(b)
    	: "%r8"
	);
	return 0;
}

void dv_copy_cond(dig_t *c, const dig_t *a, int digits, dig_t cond) {
	dig_t mask, t;
	int i;

	mask = -cond;
	for (i = 0; i < digits; i++) {
		t = (a[i] ^ c[i]) & mask;
		c[i] ^= t;
	}
}



dig_t fp_subn_low(dig_t *c, const dig_t *a, const dig_t *b) {
	int i;
	dig_t carry, r0, diff;

	// Zero the carry.
	carry = 0;
	for (i = 0; i < FP_DIGS; i++, a++, b++) {
		diff = (*a) - (*b);
		r0 = diff - carry;
		carry = ((*a) < (*b)) || (carry && !diff);
		c[i] = r0;
	}
	return carry;
}

int fp_cmpn_low(const dig_t *a, const dig_t *b) {
	int i, r;

	a += (FP_DIGS - 1);
	b += (FP_DIGS - 1);

	r = CMP_EQ;
	for (i = 0; i < FP_DIGS; i++, --a, --b) {
		if (*a != *b && r == CMP_EQ) {
			r = (*a > *b ? CMP_GT : CMP_LT);
		}
	} 
	return r;
}

void fp_rdcn_var(dig_t *c, dig_t *a) {
	int i, j;
	dig_t r0, r1, r2, *tmp, *tmpc;
	const dig_t *tmpm;

	dig_t u = 0x08435E50D79435E5;
	tmpc = c;

	*tmpc = u;


	r0 = r1 = r2 = 0;
	for (i = 0; i < FP_DIGS; i++, tmpc++, a++) {
		tmp = c;
		tmpm = prime + i;
		for (j = 0; j < i; j++, tmp++, tmpm--) {
			COMBA_STEP(r2, r1, r0, *tmp, *tmpm);
		}
		COMBA_ADD(r2, r1, r0, *a);
		*tmpc = (dig_t)(r0 * u);
		COMBA_STEP(r2, r1, r0, *tmpc, *prime);
		r0 = r1;
		r1 = r2;
		r2 = 0;
	}

	for (i = FP_DIGS; i < 2 * FP_DIGS - 1; i++, a++) {
		tmp = c + (i - FP_DIGS + 1);
		tmpm = prime + FP_DIGS - 1;
		for (j = i - FP_DIGS + 1; j < FP_DIGS; j++, tmp++, tmpm--) {
			COMBA_STEP(r2, r1, r0, *tmp, *tmpm);
		}
		COMBA_ADD(r2, r1, r0, *a);
		c[i - FP_DIGS] = r0;
		r0 = r1;
		r1 = r2;
		r2 = 0;
	}
	COMBA_ADD(r2, r1, r0, *a);
	c[FP_DIGS - 1] = r0;

	if (r1 || fp_cmpn_low(c, prime) != CMP_LT) {
		fp_subn_low(c, c, prime);
	}

}

void fp_rdcn_const(dig_t *c, dig_t *a) {
	int i, j;
	dig_t r0, r1, r2, *tmp, *tmpc, d[4];
	const dig_t *tmpm;

	dig_t u = 0x08435E50D79435E5;
	tmpc = c;

	r0 = r1 = r2 = 0;
	for (i = 0; i < FP_DIGS; i++, tmpc++, a++) {
		tmp = c;
		tmpm = prime + i;
		for (j = 0; j < i; j++, tmp++, tmpm--) {
			COMBA_STEP2(r2, r1, r0, *tmp, *tmpm);
		}
		COMBA_ADD2(r2, r1, r0, *a);
		*tmpc = (dig_t)(r0 * u);
		COMBA_STEP(r2, r1, r0, *tmpc, *prime);
		r0 = r1;
		r1 = r2;
		r2 = 0;
	}

	for (i = FP_DIGS; i < 2 * FP_DIGS - 1; i++, a++) {
		tmp = c + (i - FP_DIGS + 1);
		tmpm = prime + FP_DIGS - 1;
		for (j = i - FP_DIGS + 1; j < FP_DIGS; j++, tmp++, tmpm--) {
			COMBA_STEP2(r2, r1, r0, *tmp, *tmpm);
		}
		COMBA_ADD2(r2, r1, r0, *a);
		c[i - FP_DIGS] = r0;
		r0 = r1;
		r1 = r2;
		r2 = 0;
	}
	COMBA_ADD2(r2, r1, r0, *a);
	c[FP_DIGS - 1] = r0;

	fp_subn_const(d, c, prime);
	dv_copy_cond(c, d, 4, r1);
}

int main(int argc, char *argv[]) {
	dig_t dbl[8] = { 0 }, r[4];
	memcpy(dbl + 4, prime, sizeof(prime));
	/* VARIABLE TIME */
	fp_rdcn_var(r, dbl);
//	printf("%d %d %d %d\n", r[0], r[1], r[2], r[3]);

	/* CONSTANT TIME */
	//fp_rdcn_const(r, dbl);
	//printf("%d %d %d %d", r[0], r[1], r[2], r[3]);

}
