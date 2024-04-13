static int nextSrcImage (TIFF *tif, char **imageSpec)
/*
  seek to the next image specified in *imageSpec
  returns 1 if success, 0 if no more images to process
  *imageSpec=NULL if subsequent images should be processed in sequence
*/
{
	if (**imageSpec == comma) {  /* if not @comma, we've done all images */
		char *start = *imageSpec + 1;
		tdir_t nextImage = (tdir_t)strtol(start, imageSpec, 0);
		if (start == *imageSpec) nextImage = TIFFCurrentDirectory (tif);
		if (**imageSpec)
		{
			if (**imageSpec == comma) {
				/* a trailing comma denotes remaining images in sequence */
				if ((*imageSpec)[1] == '\0') *imageSpec = NULL;
			}else{
				fprintf (stderr,
				    "Expected a %c separated image # list after %s\n",
				    comma, TIFFFileName (tif));
				exit (-4);   /* syntax error */
			}
		}
		if (TIFFSetDirectory (tif, nextImage)) return 1;
		fprintf (stderr, "%s%c%d not found!\n",
		    TIFFFileName(tif), comma, (int) nextImage);
	}
	return 0;
}