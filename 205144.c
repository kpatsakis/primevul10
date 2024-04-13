main(int argc, char* argv[])
{
	uint16 defconfig = (uint16) -1;
	uint16 deffillorder = 0;
	uint32 deftilewidth = (uint32) -1;
	uint32 deftilelength = (uint32) -1;
	uint32 defrowsperstrip = (uint32) 0;
	uint64 diroff = 0;
	TIFF* in;
	TIFF* out;
	char mode[10];
	char* mp = mode;
	int c;
#if !HAVE_DECL_OPTARG
	extern int optind;
	extern char* optarg;
#endif

	*mp++ = 'w';
	*mp = '\0';
	while ((c = getopt(argc, argv, ",:b:c:f:l:o:p:r:w:aistBLMC8x")) != -1)
		switch (c) {
		case ',':
			if (optarg[0] != '=') usage();
			comma = optarg[1];
			break;
		case 'b':   /* this file is bias image subtracted from others */
			if (bias) {
				fputs ("Only 1 bias image may be specified\n", stderr);
				exit (-2);
			}
			{
				uint16 samples = (uint16) -1;
				char **biasFn = &optarg;
				bias = openSrcImage (biasFn);
				if (!bias) exit (-5);
				if (TIFFIsTiled (bias)) {
					fputs ("Bias image must be organized in strips\n", stderr);
					exit (-7);
				}
				TIFFGetField(bias, TIFFTAG_SAMPLESPERPIXEL, &samples);
				if (samples != 1) {
					fputs ("Bias image must be monochrome\n", stderr);
					exit (-7);
				}
			}
			break;
		case 'a':   /* append to output */
			mode[0] = 'a';
			break;
		case 'c':   /* compression scheme */
			if (!processCompressOptions(optarg))
				usage();
			break;
		case 'f':   /* fill order */
			if (streq(optarg, "lsb2msb"))
				deffillorder = FILLORDER_LSB2MSB;
			else if (streq(optarg, "msb2lsb"))
				deffillorder = FILLORDER_MSB2LSB;
			else
				usage();
			break;
		case 'i':   /* ignore errors */
			ignore = TRUE;
			break;
		case 'l':   /* tile length */
			outtiled = TRUE;
			deftilelength = atoi(optarg);
			break;
		case 'o':   /* initial directory offset */
			diroff = strtoul(optarg, NULL, 0);
			break;
		case 'p':   /* planar configuration */
			if (streq(optarg, "separate"))
				defconfig = PLANARCONFIG_SEPARATE;
			else if (streq(optarg, "contig"))
				defconfig = PLANARCONFIG_CONTIG;
			else
				usage();
			break;
		case 'r':   /* rows/strip */
			defrowsperstrip = atol(optarg);
			break;
		case 's':   /* generate stripped output */
			outtiled = FALSE;
			break;
		case 't':   /* generate tiled output */
			outtiled = TRUE;
			break;
		case 'w':   /* tile width */
			outtiled = TRUE;
			deftilewidth = atoi(optarg);
			break;
		case 'B':
			*mp++ = 'b'; *mp = '\0';
			break;
		case 'L':
			*mp++ = 'l'; *mp = '\0';
			break;
		case 'M':
			*mp++ = 'm'; *mp = '\0';
			break;
		case 'C':
			*mp++ = 'c'; *mp = '\0';
			break;
		case '8':
			*mp++ = '8'; *mp = '\0';
			break;
		case 'x':
			pageInSeq = 1;
			break;
		case '?':
			usage();
			/*NOTREACHED*/
		}
	if (argc - optind < 2)
		usage();
	out = TIFFOpen(argv[argc-1], mode);
	if (out == NULL)
		return (-2);
	if ((argc - optind) == 2)
		pageNum = -1;
	for (; optind < argc-1 ; optind++) {
		char *imageCursor = argv[optind];
		in = openSrcImage (&imageCursor);
		if (in == NULL) {
			(void) TIFFClose(out);
			return (-3);
		}
		if (diroff != 0 && !TIFFSetSubDirectory(in, diroff)) {
			TIFFError(TIFFFileName(in),
			    "Error, setting subdirectory at " TIFF_UINT64_FORMAT, diroff);
			(void) TIFFClose(in);
			(void) TIFFClose(out);
			return (1);
		}
		for (;;) {
			config = defconfig;
			compression = defcompression;
			predictor = defpredictor;
                        preset = defpreset;
			fillorder = deffillorder;
			rowsperstrip = defrowsperstrip;
			tilewidth = deftilewidth;
			tilelength = deftilelength;
			g3opts = defg3opts;
			if (!tiffcp(in, out) || !TIFFWriteDirectory(out)) {
				(void) TIFFClose(in);
				(void) TIFFClose(out);
				return (1);
			}
			if (imageCursor) { /* seek next image directory */
				if (!nextSrcImage(in, &imageCursor)) break;
			}else
				if (!TIFFReadDirectory(in)) break;
		}
		(void) TIFFClose(in);
	}

	(void) TIFFClose(out);
	return (0);
}