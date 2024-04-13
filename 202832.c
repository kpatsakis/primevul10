filemode_str(int val)
{
	static char *str[] = {
		"none", "read", "write", "read/write",
	};
	return str[val & SNDRV_SEQ_OSS_FILE_ACMODE];
}