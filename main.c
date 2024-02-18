#include "slob.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
	slob_init(0x00000000, 1024);
	print_blocks();

	void *ptr = slob_malloc(SLOB_BLOCK_OWNER_KERNEL, 512);
	print_blocks();

	slob_free(SLOB_BLOCK_OWNER_KERNEL, ptr);
	print_blocks();

	return EXIT_SUCCESS;
}
