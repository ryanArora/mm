#include "slob.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
	slob_init(0x00000000, 1024);
	print_blocks();
	slob_kmalloc(512);
	slob_kmalloc(256);
	slob_kmalloc(128);
	print_blocks();

	return EXIT_SUCCESS;
}
