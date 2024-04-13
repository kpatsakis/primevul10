SWFInput_file_getChar(SWFInput input)
{
	int c = fgetc((FILE *)input->data);

	if ( c == EOF )
		input->length = input->offset;
	else
		++input->offset;

	return c;
}