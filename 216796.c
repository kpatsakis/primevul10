SWFInput_readBits(SWFInput input, int number)
{
	int ret = input->buffer;
	if ( number == input->bufbits )
	{
		input->bufbits = 0;
		input->buffer = 0;
		return ret;
	}

	if ( number > input->bufbits )
	{
		number -= input->bufbits;

		while( number > 8 )
		{
			ret <<= 8;
			ret += SWFInput_getChar(input);
			number -= 8;
		}
		
		input->buffer = SWFInput_getChar(input);
		
		if ( number > 0 )
		{
			ret <<= number;
			input->bufbits = 8-number;
			ret += input->buffer >> (8-number);
			input->buffer &= (1<<input->bufbits)-1;
		}

		return ret;
	}

	ret = input->buffer >> (input->bufbits-number);
	input->bufbits -= number;
	input->buffer &= (1<<input->bufbits)-1;
//	printf("done: readBits(%i) numer < bufbits %i\n", number, ret);
	return ret;
}