const char* rdpgfx_get_codec_id_string(UINT16 codecId)
{
	switch (codecId)
	{
		case RDPGFX_CODECID_UNCOMPRESSED:
			return "RDPGFX_CODECID_UNCOMPRESSED";

		case RDPGFX_CODECID_CAVIDEO:
			return "RDPGFX_CODECID_CAVIDEO";

		case RDPGFX_CODECID_CLEARCODEC:
			return "RDPGFX_CODECID_CLEARCODEC";

		case RDPGFX_CODECID_PLANAR:
			return "RDPGFX_CODECID_PLANAR";

		case RDPGFX_CODECID_AVC420:
			return "RDPGFX_CODECID_AVC420";

		case RDPGFX_CODECID_AVC444:
			return "RDPGFX_CODECID_AVC444";

		case RDPGFX_CODECID_AVC444v2:
			return "RDPGFX_CODECID_AVC444v2";

		case RDPGFX_CODECID_ALPHA:
			return "RDPGFX_CODECID_ALPHA";

		case RDPGFX_CODECID_CAPROGRESSIVE:
			return "RDPGFX_CODECID_CAPROGRESSIVE";

		case RDPGFX_CODECID_CAPROGRESSIVE_V2:
			return "RDPGFX_CODECID_CAPROGRESSIVE_V2";
	}

	return "RDPGFX_CODECID_UNKNOWN";
}