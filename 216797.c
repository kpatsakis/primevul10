SWFInput_getUInt24_BE(SWFInput input)
{
	unsigned long num = SWFInput_getChar(input) << 16;
	num += SWFInput_getChar(input) << 8;
	num += SWFInput_getChar(input);
	return num;
}