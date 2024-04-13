SWFInput_getUInt16_BE(SWFInput input)
{
	int num = SWFInput_getChar(input) << 8;
	num += SWFInput_getChar(input);
	return num;
}