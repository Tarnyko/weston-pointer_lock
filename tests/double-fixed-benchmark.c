/*
 * Copyright © 2012 Intel Corporation
 * Copyright © 2015 Red Hat Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <stdint.h>
#include "shared/util.h"

volatile double global_d;

static void
noop_conversion(void)
{
	int32_t fp;

	union {
		int64_t i;
		double d;
	} u;

	for (fp = 0; fp < INT32_MAX; fp++) {
		u.i = fp;
		global_d = u.d;
	}
}

static void
magic_conversion(void)
{
	int32_t fp;

	for (fp = 0; fp < INT32_MAX; fp++) {
		global_d = wl_double_fixed_to_double(fp, fp);
	}
}

static void
mul_conversion(void)
{
	int32_t fp;

	for (fp = 0; fp < INT32_MAX; fp++) {
		global_d = fp + fp / 2147483648.0;
	}
}

double factor = 2147483648.0;

static void
div_conversion(void)
{
	int32_t fp;

	for (fp = 0; fp < INT32_MAX; fp++) {
		global_d = fp + fp / factor;
	}
}

static void
benchmark(const char *s, void (*f)(void))
{
	struct timespec start, stop, elapsed;

	clock_gettime(CLOCK_MONOTONIC, &start);
	f();
	clock_gettime(CLOCK_MONOTONIC, &stop);

	elapsed.tv_sec = stop.tv_sec - start.tv_sec;
	elapsed.tv_nsec = stop.tv_nsec - start.tv_nsec;
	if (elapsed.tv_nsec < 0) {
		elapsed.tv_nsec += 1000000000;
		elapsed.tv_sec--;
	}
	printf("benchmarked %s:\t%ld.%09lds\n",
	       s, elapsed.tv_sec, elapsed.tv_nsec);
}

int
main(int argc, char *argv[])
{
	benchmark("noop", noop_conversion);
	benchmark("magic", magic_conversion);
	benchmark("div", div_conversion);
	benchmark("mul", mul_conversion);

	return 0;
}
