static void SWFInput_dtor_close(SWFInput input)
{
	fclose((FILE *)input->data);
	SWFInput_dtor(input);
}