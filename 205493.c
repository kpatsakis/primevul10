JPEGFixupTagsSubsamplingReadByte(struct JPEGFixupTagsSubsamplingData* data, uint8* result)
{
	if (data->bufferbytesleft==0)
	{
		uint32 m;
		if (data->filebytesleft==0)
			return(0);
		if (!data->filepositioned)
		{
			TIFFSeekFile(data->tif,data->fileoffset,SEEK_SET);
			data->filepositioned=1;
		}
		m=data->buffersize;
		if ((uint64)m>data->filebytesleft)
			m=(uint32)data->filebytesleft;
		assert(m<0x80000000UL);
		if (TIFFReadFile(data->tif,data->buffer,(tmsize_t)m)!=(tmsize_t)m)
			return(0);
		data->buffercurrentbyte=data->buffer;
		data->bufferbytesleft=m;
		data->fileoffset+=m;
		data->filebytesleft-=m;
	}
	*result=*data->buffercurrentbyte;
	data->buffercurrentbyte++;
	data->bufferbytesleft--;
	return(1);
}