SWFInput_byteAlign(SWFInput input)
{
	if(input->bufbits > 0)
	{
		input->bufbits = 0;
		input->buffer = 0;
	}
}