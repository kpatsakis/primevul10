static int setDecompDefaults(struct jpeg_decompress_struct *dinfo,
	int pixelFormat, int flags)
{
	int retval=0;

	switch(pixelFormat)
	{
		case TJPF_GRAY:
			dinfo->out_color_space=JCS_GRAYSCALE;  break;
		#if JCS_EXTENSIONS==1
		case TJPF_RGB:
			dinfo->out_color_space=JCS_EXT_RGB;  break;
		case TJPF_BGR:
			dinfo->out_color_space=JCS_EXT_BGR;  break;
		case TJPF_RGBX:
			dinfo->out_color_space=JCS_EXT_RGBX;  break;
		case TJPF_BGRX:
			dinfo->out_color_space=JCS_EXT_BGRX;  break;
		case TJPF_XRGB:
			dinfo->out_color_space=JCS_EXT_XRGB;  break;
		case TJPF_XBGR:
			dinfo->out_color_space=JCS_EXT_XBGR;  break;
		#if JCS_ALPHA_EXTENSIONS==1
		case TJPF_RGBA:
			dinfo->out_color_space=JCS_EXT_RGBA;  break;
		case TJPF_BGRA:
			dinfo->out_color_space=JCS_EXT_BGRA;  break;
		case TJPF_ARGB:
			dinfo->out_color_space=JCS_EXT_ARGB;  break;
		case TJPF_ABGR:
			dinfo->out_color_space=JCS_EXT_ABGR;  break;
		#endif
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
			dinfo->out_color_space=JCS_RGB;  break;
		#endif
		case TJPF_CMYK:
			dinfo->out_color_space=JCS_CMYK;  break;
		default:
			_throw("Unsupported pixel format");
	}

	if(flags&TJFLAG_FASTDCT) dinfo->dct_method=JDCT_FASTEST;

	bailout:
	return retval;
}