static u32 preparser_disable(bool state)
{
	return MI_ARB_CHECK | 1 << 8 | state;
}