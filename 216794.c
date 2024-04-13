SWFInput_buffer_eof(SWFInput input)
{
	return input->offset >= input->length;
}