SWFInput_file_eof(SWFInput input)
{
	return feof((FILE *)input->data);
}