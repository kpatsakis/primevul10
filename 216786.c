SWFInput_getUInt32_BE(SWFInput input)
{
	unsigned long num = SWFInput_getChar(input) << 24;
	num += SWFInput_getChar(input) << 16;
	num += SWFInput_getChar(input) << 8;
	num += SWFInput_getChar(input);
	return num;
}