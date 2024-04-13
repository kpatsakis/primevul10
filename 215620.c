GF_Err gf_isom_enable_compression(GF_ISOFile *file, GF_ISOCompressMode compress_mode, Bool force_compress)
{
	if (!file) return GF_BAD_PARAM;
	file->compress_mode = compress_mode;
	file->force_compress = force_compress;
	return GF_OK;
}