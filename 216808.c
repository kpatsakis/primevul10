newSWFInput_file(FILE *f)
{
	SWFInput input;
	struct stat buf;

	/* XXX - doesn't check for validity of f.. */

	if ( fseek(f, 0, SEEK_CUR) == -1 )
		return newSWFInput_stream(f);

	if ( fstat(fileno(f), &buf) == -1 )
		SWF_error("Couldn't fstat filehandle in newSWFInput_file");;

	input = (SWFInput) malloc(sizeof(struct SWFInput_s));

	/* If malloc failed, return NULL to signify this */
	if (NULL == input)
		return NULL;

	input->getChar = SWFInput_file_getChar;
	input->destroy = SWFInput_dtor;
	input->eof = SWFInput_file_eof;
	input->seek = SWFInput_file_seek;
	input->read = SWFInput_file_read;
	input->data = f;
	input->bufbits = 0;
	input->buffer = 0;
	input->offset = 0;
	input->length = buf.st_size;

#if TRACK_ALLOCS
	input->gcnode = ming_gc_add_node(input, (dtorfunctype) destroySWFInput);
#endif

	return input;
}