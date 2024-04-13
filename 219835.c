static int setCompDefaults(struct jpeg_compress_struct *cinfo,
	int pixelFormat, int subsamp, int jpegQual, int flags)
{
	int retval=0;
#ifndef NO_GETENV
	char *env=NULL;
#endif

	switch(pixelFormat)
	{
		case TJPF_GRAY:
			cinfo->in_color_space=JCS_GRAYSCALE;  break;
		#if JCS_EXTENSIONS==1
		case TJPF_RGB:
			cinfo->in_color_space=JCS_EXT_RGB;  break;
		case TJPF_BGR:
			cinfo->in_color_space=JCS_EXT_BGR;  break;
		case TJPF_RGBX:
		case TJPF_RGBA:
			cinfo->in_color_space=JCS_EXT_RGBX;  break;
		case TJPF_BGRX:
		case TJPF_BGRA:
			cinfo->in_color_space=JCS_EXT_BGRX;  break;
		case TJPF_XRGB:
		case TJPF_ARGB:
			cinfo->in_color_space=JCS_EXT_XRGB;  break;
		case TJPF_XBGR:
		case TJPF_ABGR:
			cinfo->in_color_space=JCS_EXT_XBGR;  break;
		#else
		case TJPF_RGB:
		case TJPF_BGR:
		case TJPF_RGBX:
		case TJPF_BGRX:
		case TJPF_XRGB:
		case TJPF_XBGR:
		case TJPF_RGBA:
		case TJPF_BGRA:
		case TJPF_ARGB:
		case TJPF_ABGR:
			cinfo->in_color_space=JCS_RGB;  pixelFormat=TJPF_RGB;
			break;
		#endif
		case TJPF_CMYK:
			cinfo->in_color_space=JCS_CMYK;  break;
	}

	cinfo->input_components=tjPixelSize[pixelFormat];
	jpeg_set_defaults(cinfo);

#ifndef NO_GETENV
	if((env=getenv("TJ_OPTIMIZE"))!=NULL && strlen(env)>0 && !strcmp(env, "1"))
		cinfo->optimize_coding=TRUE;
	if((env=getenv("TJ_ARITHMETIC"))!=NULL && strlen(env)>0 && !strcmp(env, "1"))
		cinfo->arith_code=TRUE;
	if((env=getenv("TJ_RESTART"))!=NULL && strlen(env)>0)
	{
		int temp=-1;  char tempc=0;
		if(sscanf(env, "%d%c", &temp, &tempc)>=1 && temp>=0 && temp<=65535)
		{
			if(toupper(tempc)=='B')
			{
				cinfo->restart_interval=temp;
				cinfo->restart_in_rows=0;
			}
			else
				cinfo->restart_in_rows=temp;
		}
	}
#endif

	if(jpegQual>=0)
	{
		jpeg_set_quality(cinfo, jpegQual, TRUE);
		if(jpegQual>=96 || flags&TJFLAG_ACCURATEDCT) cinfo->dct_method=JDCT_ISLOW;
		else cinfo->dct_method=JDCT_FASTEST;
	}
	if(subsamp==TJSAMP_GRAY)
		jpeg_set_colorspace(cinfo, JCS_GRAYSCALE);
	else if(pixelFormat==TJPF_CMYK)
		jpeg_set_colorspace(cinfo, JCS_YCCK);
	else jpeg_set_colorspace(cinfo, JCS_YCbCr);

#ifndef NO_GETENV
	if((env=getenv("TJ_PROGRESSIVE"))!=NULL && strlen(env)>0
		&& !strcmp(env, "1"))
		jpeg_simple_progression(cinfo);
#endif

	cinfo->comp_info[0].h_samp_factor=tjMCUWidth[subsamp]/8;
	cinfo->comp_info[1].h_samp_factor=1;
	cinfo->comp_info[2].h_samp_factor=1;
	if(cinfo->num_components>3)
		cinfo->comp_info[3].h_samp_factor=tjMCUWidth[subsamp]/8;
	cinfo->comp_info[0].v_samp_factor=tjMCUHeight[subsamp]/8;
	cinfo->comp_info[1].v_samp_factor=1;
	cinfo->comp_info[2].v_samp_factor=1;
	if(cinfo->num_components>3)
		cinfo->comp_info[3].v_samp_factor=tjMCUHeight[subsamp]/8;

	return retval;
}