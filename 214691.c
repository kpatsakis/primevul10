}

void dump_mpeg2_ts(char *mpeg2ts_file, char *out_name, Bool prog_num)
{
	u8 data[188];
	GF_M2TS_Dump dumper;

	u32 size;
	u64 fsize, fdone;
	GF_M2TS_Demuxer *ts;
	FILE *src;

	if (!prog_num && !out_name) {
		fprintf(stderr, "No program number nor output filename specified. No timestamp file will be generated.");
	}

	src = gf_fopen(mpeg2ts_file, "rb");
	if (!src) {
		fprintf(stderr, "Cannot open %s: no such file\n", mpeg2ts_file);
		return;
	}
	ts = gf_m2ts_demux_new();
	ts->on_event = on_m2ts_dump_event;
	ts->notify_pes_timing = 1;
	memset(&dumper, 0, sizeof(GF_M2TS_Dump));
	ts->user = &dumper;
	dumper.prog_number = prog_num;

	/*PES dumping*/
	if (out_name) {
		char *pid = strrchr(out_name, '#');
		if (pid) {
			dumper.dump_pid = atoi(pid+1);
			pid[0] = 0;
			sprintf(dumper.dump, "%s_%d.raw", out_name, dumper.dump_pid);
			dumper.pes_out = gf_fopen(dumper.dump, "wb");
#if 0
			sprintf(dumper.nhml, "%s_%d.nhml", pes_out_name, dumper.dump_pid);
			dumper.pes_out_nhml = gf_fopen(dumper.nhml, "wt");
			sprintf(dumper.info, "%s_%d.info", pes_out_name, dumper.dump_pid);
			dumper.pes_out_info = gf_fopen(dumper.info, "wb");
#endif
			pid[0] = '#';
		}
	}

	gf_fseek(src, 0, SEEK_END);
	fsize = gf_ftell(src);
	gf_fseek(src, 0, SEEK_SET);

	/* first loop to process all packets between two PAT, and assume all signaling was found between these 2 PATs */
	while (!feof(src)) {
		size = (u32) gf_fread(data, 188, src);
		if (size<188) break;

		gf_m2ts_process_data(ts, data, size);
		if (dumper.has_seen_pat) break;
	}
	dumper.has_seen_pat = GF_TRUE;

	if (!prog_num) {
		GF_M2TS_Program *p = gf_list_get(ts->programs, 0);
		if (p) prog_num = p->number;
		fprintf(stderr, "No program number specified, defaulting to first program\n");
	}

	if (!prog_num && !out_name) {
		fprintf(stderr, "No program number nor output filename specified. No timestamp file will be generated\n");
	}

	if (prog_num) {
		sprintf(dumper.timestamps_info_name, "%s_prog_%d_timestamps.txt", mpeg2ts_file, prog_num/*, mpeg2ts_file*/);
		dumper.timestamps_info_file = gf_fopen(dumper.timestamps_info_name, "wt");
		if (!dumper.timestamps_info_file) {
			fprintf(stderr, "Cannot open file %s\n", dumper.timestamps_info_name);
			return;
		}
		fprintf(dumper.timestamps_info_file, "PCK#\tPID\tPCR\tDTS\tPTS\tRAP\tDiscontinuity\tDTS-PCR Diff\n");
	}

	gf_m2ts_reset_parsers(ts);
	gf_fseek(src, 0, SEEK_SET);
	fdone = 0;

	while (!feof(src)) {
		size = (u32) gf_fread(data, 188, src);
		if (size<188) break;

		gf_m2ts_process_data(ts, data, size);

		fdone += size;
		gf_set_progress("MPEG-2 TS Parsing", fdone, fsize);
	}

	gf_fclose(src);
	gf_m2ts_demux_del(ts);
	if (dumper.pes_out) gf_fclose(dumper.pes_out);
#if 0
	if (dumper.pes_out_nhml) {
		if (dumper.is_info_dumped) fprintf(dumper.pes_out_nhml, "</NHNTStream>\n");
		gf_fclose(dumper.pes_out_nhml);
		gf_fclose(dumper.pes_out_info);
	}
#endif