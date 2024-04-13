SWFInput_input_seek(SWFInput input, long offset, int whence)
{
	if ( whence == SEEK_CUR )
	{
		if ( offset >= 0 )
			input->offset = min(input->length, input->offset + offset);
		else
			input->offset = max(0, input->offset + offset);
	}

	else if ( whence == SEEK_END )
		input->offset = max(0, input->length - offset);

	else if ( whence == SEEK_SET )
		input->offset = min(input->length, offset);
}