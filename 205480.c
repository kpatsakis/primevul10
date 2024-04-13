JPEGFixupTagsSubsamplingReadWord(struct JPEGFixupTagsSubsamplingData* data, uint16* result)
{
	uint8 ma;
	uint8 mb;
	if (!JPEGFixupTagsSubsamplingReadByte(data,&ma))
		return(0);
	if (!JPEGFixupTagsSubsamplingReadByte(data,&mb))
		return(0);
	*result=(ma<<8)|mb;
	return(1);
}