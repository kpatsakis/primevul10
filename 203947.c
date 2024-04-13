extract_resources_callback (WinLibrary *fi, WinResource *wr,
                            WinResource *type_wr, WinResource *name_wr,
                            WinResource *lang_wr)
{
	int size;
	bool free_it;
	void *memory;
	char *outname;
	FILE *out;

	memory = extract_resource(fi, wr, &size, &free_it, type_wr->id, (lang_wr == NULL ? NULL : lang_wr->id), arg_raw);
	free_it = false;
	if (memory == NULL) {
		/* extract resource has printed error */
		return;
	}

	/* determine where to extract to */
	outname = get_destination_name(fi, type_wr->id, name_wr->id, (lang_wr == NULL ? NULL : lang_wr->id));
	if (outname == NULL) {
		out = stdout;
	} else {
		out = fopen(outname, "wb");
		if (out == NULL) {
			warn_errno("%s", outname);
			goto cleanup;
		}
	}

	/* write the actual data */
	fwrite(memory, size, 1, out);
	
	cleanup:
	if (free_it)
		free(memory);
	if (out != NULL && out != stdout)
		fclose(out);
}