static Bool latm_dmx_sync_frame_bs(GF_BitStream *bs, GF_M4ADecSpecInfo *acfg, u32 *nb_bytes, u8 *buffer, u32 *nb_skipped)
{
	u32 val, size;
	u64 pos, mux_size;
	if (nb_skipped) *nb_skipped = 0;
	if (!acfg) return 0;

	while (gf_bs_available(bs)>3) {
		val = gf_bs_read_u8(bs);
		if (val!=0x56) {
			if (nb_skipped) (*nb_skipped) ++;
			continue;
		}
		val = gf_bs_read_int(bs, 3);
		if (val != 0x07) {
			gf_bs_read_int(bs, 5);
			if (nb_skipped) (*nb_skipped) ++;
			continue;
		}
		mux_size = gf_bs_read_int(bs, 13);
		pos = gf_bs_get_position(bs);
		if (mux_size>gf_bs_available(bs) ) {
			gf_bs_seek(bs, pos-3);
			return GF_FALSE;
		}

		/*use same stream mux*/
		if (!gf_bs_read_int(bs, 1)) {
			Bool amux_version, amux_versionA;

			amux_version = (Bool)gf_bs_read_int(bs, 1);
			amux_versionA = GF_FALSE;
			if (amux_version) amux_versionA = (Bool)gf_bs_read_int(bs, 1);
			if (!amux_versionA) {
				u32 i, allStreamsSameTimeFraming, numProgram;
				if (amux_version) gf_latm_get_value(bs);

				allStreamsSameTimeFraming = gf_bs_read_int(bs, 1);
				/*numSubFrames = */gf_bs_read_int(bs, 6);
				numProgram = gf_bs_read_int(bs, 4);
				for (i=0; i<=numProgram; i++) {
					u32 j, num_lay;
					num_lay = gf_bs_read_int(bs, 3);
					for (j=0; j<=num_lay; j++) {
						u32 frameLengthType;
						Bool same_cfg = GF_FALSE;
						if (i || j) same_cfg = (Bool)gf_bs_read_int(bs, 1);

						if (!same_cfg) {
							if (amux_version==1) gf_latm_get_value(bs);
							gf_m4a_parse_config(bs, acfg, GF_FALSE);
						}
						frameLengthType = gf_bs_read_int(bs, 3);
						if (!frameLengthType) {
							/*latmBufferFullness = */gf_bs_read_int(bs, 8);
							if (!allStreamsSameTimeFraming) {
							}
						} else {
							/*not supported*/
						}
					}

				}
				/*other data present*/
				if (gf_bs_read_int(bs, 1)) {
//					u32 k = 0;
				}
				/*CRCcheck present*/
				if (gf_bs_read_int(bs, 1)) {
				}
			}
		}

		size = 0;
		while (1) {
			u32 tmp = gf_bs_read_int(bs, 8);
			size += tmp;
			if (tmp!=255) break;
		}
		if (gf_bs_available(bs) < size) {
			gf_bs_seek(bs, pos-3);
			return GF_FALSE;
		}

		if (nb_bytes) {
			*nb_bytes = (u32) size;
		}

		if (buffer) {
			gf_bs_read_data(bs, (char *) buffer, size);
		} else {
			while (size) {
				gf_bs_read_int(bs, 8);
				size--;
			}
		}

		/*parse amux*/
		gf_bs_seek(bs, pos + mux_size);

		if ((gf_bs_available(bs)>2) && gf_bs_peek_bits(bs, 11, 0) != 0x2B7) {
			gf_bs_seek(bs, pos + 1);
			if (nb_skipped) (*nb_skipped) ++;
			continue;
		}

		return GF_TRUE;
	}
	return GF_FALSE;
}