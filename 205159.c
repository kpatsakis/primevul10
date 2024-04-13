static TIFF* openSrcImage (char **imageSpec)
/*
  imageSpec points to a pointer to a filename followed by optional ,image#'s
  Open the TIFF file and assign *imageSpec to either NULL if there are
  no images specified, or a pointer to the next image number text
*/
{
	TIFF *tif;
	char *fn = *imageSpec;
	*imageSpec = strchr (fn, comma);
	if (*imageSpec) {  /* there is at least one image number specifier */
		**imageSpec = '\0';
		tif = TIFFOpen (fn, "r");
		/* but, ignore any single trailing comma */
		if (!(*imageSpec)[1]) {*imageSpec = NULL; return tif;}
		if (tif) {
			**imageSpec = comma;  /* replace the comma */
			if (!nextSrcImage(tif, imageSpec)) {
				TIFFClose (tif);
				tif = NULL;
			}
		}
	}else
		tif = TIFFOpen (fn, "r");
	return tif;
}