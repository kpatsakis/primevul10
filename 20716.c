static void read_block_list(unsigned int *block_list, char *block_ptr, int blocks)
{
	TRACE("read_block_list: blocks %d\n", blocks);

	if(swap) {
		SQUASHFS_SWAP_INTS_3(block_list, block_ptr, blocks);
	} else
		memcpy(block_list, block_ptr, blocks * sizeof(unsigned int));
}