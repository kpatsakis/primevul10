static int SWFInput_stream_eof(SWFInput input)
{
	return ((input->offset >= input->length) ? feof((FILE *)input->data) : 0);
}