SWFInput_length(SWFInput input)
{
	int pos = SWFInput_tell(input);

	SWFInput_seek(input, 0, SEEK_END);
	SWFInput_seek(input, pos, SEEK_SET);

	return input->length;
}