static const char *latm_dmx_probe_data(const u8 *data, u32 size, GF_FilterProbeScore *score)
{
	u32 nb_frames=0;
	u32 nb_skip=0;
	GF_M4ADecSpecInfo acfg;
	GF_BitStream *bs = gf_bs_new(data, size, GF_BITSTREAM_READ);
	while (1) {
		u32 nb_skipped = 0;
		if (!latm_dmx_sync_frame_bs(bs, &acfg, 0, NULL, &nb_skipped)) break;
		if (! GF_M4ASampleRates[acfg.base_sr_index]) {
			nb_frames = 0;
			break;
		}

		if (nb_skipped) {
			if (nb_skip) {
				nb_frames = 0;
				break;
			}
			nb_skip++;
		}
		nb_frames++;
	}
	gf_bs_del(bs);
	if (nb_frames>=2) {
		*score = nb_skip ? GF_FPROBE_MAYBE_SUPPORTED : GF_FPROBE_SUPPORTED;
		return "audio/aac+latm";
	}
	return NULL;
}