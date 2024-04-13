SWFInput_getUInt16(SWFInput input)
{
	int num = SWFInput_getChar(input);
	num += SWFInput_getChar(input) << 8;
	return num;
}