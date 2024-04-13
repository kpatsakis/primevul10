SWFInput_input_getChar(SWFInput input)
{
	struct SWFInputPtr *ptr;
 	int old_offset, result;

	if ( input->offset >= input->length )
		return EOF;
		
	ptr = (struct SWFInputPtr *)input->data;
	old_offset = SWFInput_tell(ptr->input);
 
	SWFInput_seek(ptr->input, ptr->offset + input->offset, SEEK_SET);
	result = SWFInput_getChar(ptr->input);
	input->offset++;
	SWFInput_seek(ptr->input, old_offset, SEEK_SET);

	return result;
}