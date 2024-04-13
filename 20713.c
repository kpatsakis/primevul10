static void read_block_list(unsigned int *block_list, char *block_ptr, int blocks)
{
	unsigned short block_size;
	int i;

	TRACE("read_block_list: blocks %d\n", blocks);

	for(i = 0; i < blocks; i++, block_ptr += 2) {
		if(swap) {
			unsigned short sblock_size;
			memcpy(&sblock_size, block_ptr, sizeof(unsigned short));
			SQUASHFS_SWAP_SHORTS_3((&block_size), &sblock_size, 1);
		} else
			memcpy(&block_size, block_ptr, sizeof(unsigned short));
		block_list[i] = SQUASHFS_COMPRESSED_SIZE(block_size) |
			(SQUASHFS_COMPRESSED(block_size) ? 0 :
			SQUASHFS_COMPRESSED_BIT_BLOCK);
	}
}