SWFInput_getUInt32(SWFInput input)
{
	unsigned long num = SWFInput_getChar(input);
	num += SWFInput_getChar(input) << 8;
	num += SWFInput_getChar(input) << 16;
	num += SWFInput_getChar(input) << 24;
	return num;
}