static void read_block_list(unsigned int *block_list, char *block_ptr, int blocks)
{
	TRACE("read_block_list: blocks %d\n", blocks);

	memcpy(block_list, block_ptr, blocks * sizeof(unsigned int));
	SQUASHFS_INSWAP_INTS(block_list, blocks);
}