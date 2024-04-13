JPEGFixupTagsSubsamplingSkip(struct JPEGFixupTagsSubsamplingData* data, uint16 skiplength)
{
	if ((uint32)skiplength<=data->bufferbytesleft)
	{
		data->buffercurrentbyte+=skiplength;
		data->bufferbytesleft-=skiplength;
	}
	else
	{
		uint16 m;
		m=(uint16)(skiplength-data->bufferbytesleft);
		if (m<=data->filebytesleft)
		{
			data->bufferbytesleft=0;
			data->fileoffset+=m;
			data->filebytesleft-=m;
			data->filepositioned=0;
		}
		else
		{
			data->bufferbytesleft=0;
			data->filebytesleft=0;
		}
	}
}