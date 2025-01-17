newSWFInput_filename(const char *filename)
{
	FILE *file;
	SWFInput input;
	
	file = fopen(filename, "rb");
	if(file == NULL)
	{
		SWF_warn("newSWFInput_filename: %s: %s\n",
			filename, strerror(errno));
		return NULL;
	}

	input = newSWFInput_file(file);
	if(input == NULL)
		return NULL;

	input->destroy = SWFInput_dtor_close;
	return input;
}