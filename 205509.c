JPEGFixupTagsSubsamplingSec(struct JPEGFixupTagsSubsamplingData* data)
{
	static const char module[] = "JPEGFixupTagsSubsamplingSec";
	uint8 m;
	while (1)
	{
		while (1)
		{
			if (!JPEGFixupTagsSubsamplingReadByte(data,&m))
				return(0);
			if (m==255)
				break;
		}
		while (1)
		{
			if (!JPEGFixupTagsSubsamplingReadByte(data,&m))
				return(0);
			if (m!=255)
				break;
		}
		switch (m)
		{
			case JPEG_MARKER_SOI:
				/* this type of marker has no data and should be skipped */
				break;
			case JPEG_MARKER_COM:
			case JPEG_MARKER_APP0:
			case JPEG_MARKER_APP0+1:
			case JPEG_MARKER_APP0+2:
			case JPEG_MARKER_APP0+3:
			case JPEG_MARKER_APP0+4:
			case JPEG_MARKER_APP0+5:
			case JPEG_MARKER_APP0+6:
			case JPEG_MARKER_APP0+7:
			case JPEG_MARKER_APP0+8:
			case JPEG_MARKER_APP0+9:
			case JPEG_MARKER_APP0+10:
			case JPEG_MARKER_APP0+11:
			case JPEG_MARKER_APP0+12:
			case JPEG_MARKER_APP0+13:
			case JPEG_MARKER_APP0+14:
			case JPEG_MARKER_APP0+15:
			case JPEG_MARKER_DQT:
			case JPEG_MARKER_SOS:
			case JPEG_MARKER_DHT:
			case JPEG_MARKER_DRI:
				/* this type of marker has data, but it has no use to us and should be skipped */
				{
					uint16 n;
					if (!JPEGFixupTagsSubsamplingReadWord(data,&n))
						return(0);
					if (n<2)
						return(0);
					n-=2;
					if (n>0)
						JPEGFixupTagsSubsamplingSkip(data,n);
				}
				break;
			case JPEG_MARKER_SOF0: /* Baseline sequential Huffman */
			case JPEG_MARKER_SOF1: /* Extended sequential Huffman */
			case JPEG_MARKER_SOF2: /* Progressive Huffman: normally not allowed by TechNote, but that doesn't hurt supporting it */
			case JPEG_MARKER_SOF9: /* Extended sequential arithmetic */
			case JPEG_MARKER_SOF10: /* Progressive arithmetic: normally not allowed by TechNote, but that doesn't hurt supporting it */
				/* this marker contains the subsampling factors we're scanning for */
				{
					uint16 n;
					uint16 o;
					uint8 p;
					uint8 ph,pv;
					if (!JPEGFixupTagsSubsamplingReadWord(data,&n))
						return(0);
					if (n!=8+data->tif->tif_dir.td_samplesperpixel*3)
						return(0);
					JPEGFixupTagsSubsamplingSkip(data,7);
					if (!JPEGFixupTagsSubsamplingReadByte(data,&p))
						return(0);
					ph=(p>>4);
					pv=(p&15);
					JPEGFixupTagsSubsamplingSkip(data,1);
					for (o=1; o<data->tif->tif_dir.td_samplesperpixel; o++)
					{
						JPEGFixupTagsSubsamplingSkip(data,1);
						if (!JPEGFixupTagsSubsamplingReadByte(data,&p))
							return(0);
						if (p!=0x11)
						{
							TIFFWarningExt(data->tif->tif_clientdata,module,
							    "Subsampling values inside JPEG compressed data have no TIFF equivalent, auto-correction of TIFF subsampling values failed");
							return(1);
						}
						JPEGFixupTagsSubsamplingSkip(data,1);
					}
					if (((ph!=1)&&(ph!=2)&&(ph!=4))||((pv!=1)&&(pv!=2)&&(pv!=4)))
					{
						TIFFWarningExt(data->tif->tif_clientdata,module,
						    "Subsampling values inside JPEG compressed data have no TIFF equivalent, auto-correction of TIFF subsampling values failed");
						return(1);
					}
					if ((ph!=data->tif->tif_dir.td_ycbcrsubsampling[0])||(pv!=data->tif->tif_dir.td_ycbcrsubsampling[1]))
					{
						TIFFWarningExt(data->tif->tif_clientdata,module,
						    "Auto-corrected former TIFF subsampling values [%d,%d] to match subsampling values inside JPEG compressed data [%d,%d]",
						    (int)data->tif->tif_dir.td_ycbcrsubsampling[0],
						    (int)data->tif->tif_dir.td_ycbcrsubsampling[1],
						    (int)ph,(int)pv);
						data->tif->tif_dir.td_ycbcrsubsampling[0]=ph;
						data->tif->tif_dir.td_ycbcrsubsampling[1]=pv;
					}
				}
				return(1);
			default:
				return(0);
		}
	}
}