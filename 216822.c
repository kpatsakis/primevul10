static int SWFInput_file_read(SWFInput input, unsigned char *buffer, int count)
{	int len = fread(buffer, 1, count, (FILE *)input->data);
	input->offset += len;
	return len;
}