SWFInput_buffer_getChar(SWFInput input)
{
	if ( input->offset >= input->length )
		return EOF;
	else
		return ((unsigned char *)input->data)[input->offset++];
}