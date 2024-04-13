static void print_section(char *level, char *text, u8 *addr,
			  unsigned int length)
{
	metadata_access_enable();
	print_hex_dump(level, text, DUMP_PREFIX_ADDRESS, 16, 1, addr,
			length, 1);
	metadata_access_disable();
}