static rpmRC hdrblobVerifyLengths(rpmTagVal regionTag, uint32_t il, uint32_t dl,
				  char **emsg) {
    uint32_t il_max = HEADER_TAGS_MAX;
    uint32_t dl_max = HEADER_DATA_MAX;
    if (regionTag == RPMTAG_HEADERSIGNATURES) {
	il_max = 32;
	dl_max = 64 * 1024 * 1024;
    }
    if (hdrchkRange(il_max, il)) {
	rasprintf(emsg, _("hdr tags: BAD, no. of tags(%" PRIu32 ") out of range"), il);
	return RPMRC_FAIL;
    }
    if (hdrchkRange(dl_max, dl)) {
	rasprintf(emsg, _("hdr data: BAD, no. of bytes(%" PRIu32 ") out of range"), dl);
	return RPMRC_FAIL;
    }
    return RPMRC_OK;
}