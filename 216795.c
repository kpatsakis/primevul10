SWFInput_file_seek(SWFInput input, long offset, int whence)
{
	if ( fseek((FILE *)input->data, offset, whence) == -1 )
	{
		if ( errno == EBADF )
			SWF_error("This is not a seekable stream- use newSWFInput_stream instead");
		else if ( errno == EINVAL )
			SWF_error("Invalid whence argument");
		else
			SWF_error("Unknown error");
	}

	if ( whence == SEEK_SET )
		input->offset = offset;

	else if ( whence == SEEK_END )
		input->offset = input->length - offset;

	else if ( whence == SEEK_CUR )
		input->offset += offset;
}