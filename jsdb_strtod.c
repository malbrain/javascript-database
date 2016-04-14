// strtod.c --
//
//	Source code for the "strtod" library procedure.
//
// Copyright (c) 1988-1993 The Regents of the University of California.
// Copyright (c) 1994 Sun Microsystems, Inc.
//
// See the file "license.terms" for information on usage and redistribution
// of this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// RCS: @(#) $Id: strtod.c,v 1.1.1.4 2003/03/06 00:09:04 landonf Exp $

#include <ctype.h>
#include "jsdb.h"

// Largest possible base 10 exponent.  Any
// exponent larger than this will already
// produce underflow or overflow, so there's
// no need to worry about additional digits.

static int maxExponent = 511;

// Table giving binary powers of 10.  Entry
// is 10^2^i.  Used to convert decimal
// exponents into floating-point numbers.

static double powersOf10[] = {
	10.,		
	100.,		
	1.0e4,
	1.0e8,
	1.0e16,
	1.0e32,
	1.0e64,
	1.0e128,
	1.0e256
};

// strtod --
//
//	This procedure converts a floating-point number from an ASCII
//	decimal representation to internal double-precision format.
//
// Results:
//	The return value is the double-precision floating-point
//	representation of the characters in string.  If endPtr isn't
//	NULL, then *endPtr is filled in with the address of the
//	next character after the last one that was part of the
//	floating-point number.
//
// Side effects:
//	None.
//

// A decimal ASCII floating-point number,
// optionally preceded by white space.
// Must have form "-I.FE-X", where I is the
// integer part of the mantissa, F is the
// fractional part of the mantissa, and X
// is the exponent.  Either of the signs
// may be "+", "-", or omitted.  Either I
// or F may be omitted, or both.  The decimal
// point isn't necessary unless F is present.
// The "E" may actually be an "e".  E and X
// may both be omitted (but not just one).

value_t jsdb_strtod(value_t val) {
	bool sign, expSign = false, intVal = true;
	double dblExp, *d;
	int max = val.aux;
	int64_t fraction;
	int fracExp = 0;
	value_t result;
	int off = 0;
	int exp = 0;		// Exponent read from "EX" field.
	int c;

	// Exponent that derives from the fractional
	// part.  Under normal circumstatnces, it is
	// the negative of the number of digits in F.
	// However, if I is very long, the last digits
	// of I get dropped (otherwise a long I with a
	// large negative exponent could cause an
	// unnecessary overflow on I alone).  In this
	// case, fracExp is incremented one for each
	// dropped digit.

	int mantSize;	// Number of digits in mantissa.
	int decPt;		// Number of mantissa digits BEFORE decimal point.
	int pExp;		// Temporarily holds location of exponent in string.

	// Strip off leading blanks and check for a sign.

	while (off < max)
		if (isspace(val.str[off]))
			off++;
		else
			break;

	if (off < max && val.str[off] == '-')
		sign = true, off++;
	else if (off < max && val.str[off] == '+')
		sign = false, off++;

	// Count the number of digits in the mantissa (including the decimal
	// point), and also locate the decimal point.

	decPt = -1;

	for (mantSize = 0; off < max; off++, mantSize += 1) {
		if (isdigit(val.str[off]))
			continue;
		else if ((val.str[off] != '.') || (decPt >= 0))
			break;
		else
			decPt = mantSize;
	}

	// Now suck up the digits in the mantissa.  Use two integers to
	// collect 9 digits each (this is faster than using floating-point).
	// If the mantissa has more than 18 digits, ignore the extras, since
	// they can't affect the value anyway.
	
	pExp  = off;
	off -= mantSize;

	if ((intVal = decPt < 0))
		decPt = mantSize;
	else
		mantSize -= 1;	// One of the digits was the point.

	if (!intVal)
	  if (mantSize > 18) {
		fracExp = decPt - 18;
		mantSize = 18;
	  } else
		fracExp = decPt - mantSize;

	for (fraction = 0; off < pExp; off++)
		if (val.str[off] != '.')
			fraction = 10*fraction + (val.str[off] - '0');

	// Skim off the exponent.

	if (!intVal && off < max)
	  if (!(intVal = !(val.str[off] == 'E') || (val.str[off] == 'e')))
		if (++off < max) {
		  if (val.str[off] == '-')
			expSign = true;
		  else if (val.str[off] == '+')
			expSign = false;

		  if (!isdigit(val.str[off]))
			return result.dbl = 0, result.bits = vt_nan, result;

		  while (++off < max && isdigit(val.str[off]))
			exp = 10*exp + (val.str[off] - '0');
		}

	// examine trailing characters

	while (off < max)
		if (!isspace(val.str[off++]))
			return result.nval = 0, result.bits = vt_nan, result;

	if (intVal) {
	  result.bits = vt_int;

	  if (sign)
		result.nval = -fraction;
	  else
		result.nval = fraction;

	  return result;
	}

	if (expSign)
		exp = fracExp - exp;
	else
		exp = fracExp + exp;

	// Generate a floating-point number that represents the exponent.
	// Do this by processing the exponent one bit at a time to combine
	// many powers of 2 of 10. Then combine the exponent with the
	// fraction.
	
	if (exp < 0) {
		expSign = true;
		exp = -exp;
	} else {
		expSign = false;
	}

	if (exp > maxExponent) {
		result.bits = vt_infinite;
		result.negative = sign;
		return result;
	}

	dblExp = 1.0;

	for (int d = 0; exp != 0; exp >>= 1, d++)
		if (exp & 01)
			dblExp *= powersOf10[d];

	if (expSign)
		result.dbl = fraction / dblExp;
	else
		result.dbl = fraction * dblExp;

	if (sign)
		result.dbl = -result.dbl;
	else
		result.dbl = result.dbl;

	if (result.dbl - (int)result.dbl)
		result.bits = vt_dbl;
	else {
		result.nval = result.dbl;
		result.bits = vt_int;
	}

	return result;
}
