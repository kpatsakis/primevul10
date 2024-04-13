SWFInput_buffer_read(SWFInput input, unsigned char* buffer, int count)
{
	if ( count > input->length - input->offset )
		count = input->length - input->offset;

	memcpy(buffer, (unsigned char*)input->data + input->offset, count);
	input->offset += count;

	return count;
}