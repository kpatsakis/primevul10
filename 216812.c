SWFInput_input_read(SWFInput input, unsigned char* buffer, int count)
{
	int ret;
	struct SWFInputPtr *ptr;
	int old_offset;

	if ( count > input->length - input->offset )
		count = input->length - input->offset;

	ptr = (struct SWFInputPtr *)input->data;
        old_offset = SWFInput_tell(ptr->input);
	SWFInput_seek(ptr->input, ptr->offset + input->offset, SEEK_SET);

	ret = SWFInput_read((SWFInput)ptr->input, buffer, count);
	if(ret != count)
		SWF_warn("SWFInput_input_read: ret %i, count %i\n", ret, count);
	input->offset += count;

	SWFInput_seek(ptr->input, old_offset, SEEK_SET);

	return count;
}