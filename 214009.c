Bool mp4box_parse_args(int argc, char **argv)
{
	u32 i;
	/*parse our args*/
	for (i = 1; i < (u32)argc; i++) {
		char *arg = argv[i];
		/*input file(s)*/
		if ((arg[0] != '-') || !stricmp(arg, "--")) {
			char *arg_val = arg;
			if (!stricmp(arg, "--")) {
				CHECK_NEXT_ARG
				arg_val = argv[i + 1];
				i++;
			}
			if (argc < 3) {
				fprintf(stderr, "Error - only one input file found as argument, please check usage\n");
				return 2;
			}
			else if (inName) {
				if (dash_duration) {
					if (!nb_dash_inputs) {
						dash_inputs = set_dash_input(dash_inputs, inName, &nb_dash_inputs);
					}
					dash_inputs = set_dash_input(dash_inputs, arg_val, &nb_dash_inputs);
				}
				else {
					fprintf(stderr, "Error - 2 input names specified, please check usage\n");
					return 2;
				}
			}
			else {
				inName = arg_val;
			}
		}
		else if (!stricmp(arg, "-?")) {
			PrintUsage();
			return 1;
		}
		else if (!stricmp(arg, "-version")) {
			PrintVersion();
			return 1;
		}
		else if (!stricmp(arg, "-sdp")) print_sdp = 1;
        else if (!strcmp(argv[i], "-mem-track")) continue;
        else if (!strcmp(argv[i], "-mem-track-stack")) continue;
        else if (!strcmp(argv[i], "-p")) {
        	i++;
        	continue;
		}
        else if (!strncmp(argv[i], "-p=", 3)) continue;
		else if (!stricmp(arg, "-logs") || !strcmp(arg, "-log-file") || !strcmp(arg, "-lf")) {
			i++;
		}
		else if (!stricmp(arg, "-tracks")) get_nb_tracks = 1;
		else if (!stricmp(arg, "-info") || !stricmp(arg, "-infon")) {
			print_info = 1;
			if ((i + 1<(u32)argc) && (sscanf(argv[i + 1], "%u", &info_track_id) == 1)) {
				char szTk[20];
				sprintf(szTk, "%u", info_track_id);
				if (!strcmp(szTk, argv[i + 1])) i++;
				else info_track_id = 0;

				if (!stricmp(arg, "-infon")) print_info = 2;
			}
			else {
				info_track_id = 0;
			}
		}
		else if (!stricmp(arg, "-grab-ts")) {
			fprintf(stderr, "Deprecated option - use gpac application\n");
			return mp4box_cleanup(2);
		}
		else if (!stricmp(arg, "-atsc")) {
			fprintf(stderr, "Deprecated option - use gpac application\n");
			return mp4box_cleanup(2);
		}
#if !defined(GPAC_DISABLE_CORE_TOOLS)
		else if (!stricmp(arg, "-wget")) {
			CHECK_NEXT_ARG
			do_wget = argv[i + 1];
			i++;
		}
#endif
		/*******************************************************************************/
		else if (!stricmp(arg, "-dvbhdemux")) {
			dvbhdemux = GF_TRUE;
		}
		/********************************************************************************/
#ifndef GPAC_DISABLE_MEDIA_EXPORT
		else if (!stricmp(arg, "-raw")) {
			CHECK_NEXT_ARG
			track_dump_type = create_new_track_action(argv[i + 1], &tracks, &nb_track_act, GF_EXPORT_NATIVE);
			i++;
		}
		else if (!stricmp(arg, "-raw-layer")) {
			CHECK_NEXT_ARG
			track_dump_type = create_new_track_action(argv[i + 1], &tracks, &nb_track_act, GF_EXPORT_NATIVE | GF_EXPORT_SVC_LAYER);
			i++;
		}
		else if (!stricmp(arg, "-qcp")) {
			CHECK_NEXT_ARG
			track_dump_type = create_new_track_action(argv[i + 1], &tracks, &nb_track_act, GF_EXPORT_NATIVE | GF_EXPORT_USE_QCP);
			i++;
		}
		else if (!stricmp(arg, "-aviraw")) {
			CHECK_NEXT_ARG
			if (argv[i + 1] && !stricmp(argv[i + 1], "video")) trackID = 1;
			else if (argv[i + 1] && !stricmp(argv[i + 1], "audio")) {
				if (strlen(argv[i + 1]) == 5) trackID = 2;
				else trackID = 1 + atoi(argv[i + 1] + 5);
			} else {
				fprintf(stderr, "Usage: \"-aviraw video\" or \"-aviraw audio\"\n");
				return 2;
			}
			track_dump_type = GF_EXPORT_AVI_NATIVE;
			i++;
		}
		else if (!stricmp(arg, "-raws")) {
			CHECK_NEXT_ARG
			track_dump_type = create_new_track_action(argv[i + 1], &tracks, &nb_track_act, GF_EXPORT_RAW_SAMPLES);
			i++;
		}
		else if (!stricmp(arg, "-nhnt")) {
			CHECK_NEXT_ARG
			track_dump_type = create_new_track_action(argv[i + 1], &tracks, &nb_track_act, GF_EXPORT_NHNT);
			i++;
		}
		else if (!stricmp(arg, "-nhml")) {
			CHECK_NEXT_ARG
			track_dump_type = create_new_track_action(argv[i + 1], &tracks, &nb_track_act, GF_EXPORT_NHML);
			i++;
		}
		else if (!stricmp(arg, "-webvtt-raw")) {
			CHECK_NEXT_ARG
			track_dump_type = create_new_track_action(argv[i + 1], &tracks, &nb_track_act, GF_EXPORT_WEBVTT_META);
			i++;
		}
		else if (!stricmp(arg, "-six")) {
			CHECK_NEXT_ARG
			track_dump_type = create_new_track_action(argv[i + 1], &tracks, &nb_track_act, GF_EXPORT_SIX);
			i++;
		}
		else if (!stricmp(arg, "-avi")) {
			CHECK_NEXT_ARG
			track_dump_type = create_new_track_action(argv[i + 1], &tracks, &nb_track_act, GF_EXPORT_AVI);
			if (tracks[nb_track_act-1].trackID)
				i++;
		}
#endif /*GPAC_DISABLE_MEDIA_EXPORT*/
#if !defined(GPAC_DISABLE_STREAMING) && !defined(GPAC_DISABLE_SENG)
		else if (!stricmp(arg, "-rtp")) {
			fprintf(stderr, "Deprecated option - use gpac application\n");
			return mp4box_cleanup(2);
		}
		else if (!stricmp(arg, "-live")) {
			live_scene = GF_TRUE;
		}
#endif
		else if (!stricmp(arg, "-diod")) {
			dump_iod = GF_TRUE;
		}
#ifndef GPAC_DISABLE_VRML
		else if (!stricmp(arg, "-node")) {
			CHECK_NEXT_ARG
			PrintNode(argv[i + 1], 0);
			return 1;
		}
		else if (!stricmp(arg, "-xnode")) {
			CHECK_NEXT_ARG
			PrintNode(argv[i + 1], 1);
			return 1;
		}
		else if (!stricmp(arg, "-nodes") || !stricmp(arg, "-nodex")) {
			PrintBuiltInNodes(0, !stricmp(arg, "-nodex") ? GF_TRUE : GF_FALSE);
			return 1;
		}
		else if (!stricmp(arg, "-xnodes") || !stricmp(arg, "-xnodex")) {
			PrintBuiltInNodes(1, !stricmp(arg, "-xnodex") ? GF_TRUE : GF_FALSE);
			return 1;
		}
#endif
#ifndef GPAC_DISABLE_SVG
		else if (!stricmp(arg, "-snodes")) {
			PrintBuiltInNodes(2, GF_FALSE);
			return 1;
		}
#endif
		else if (!stricmp(arg, "-boxcov")) {
			gf_sys_set_args(argc, (const char **) argv);
            PrintBuiltInBoxes(GF_TRUE);
			return 1;
		} else if (!stricmp(arg, "-boxes")) {
			PrintBuiltInBoxes(GF_FALSE);
			return 1;
		}
		else if (!stricmp(arg, "-std")) dump_std = 2;
		else if (!stricmp(arg, "-stdb")) dump_std = 1;
		else if (!stricmp(arg, "-fstat")) fs_dump_flags |= 1;
		else if (!stricmp(arg, "-fgraph")) fs_dump_flags |= 1<<1;

#if !defined(GPAC_DISABLE_MEDIA_EXPORT) && !defined(GPAC_DISABLE_SCENE_DUMP)
		else if (!stricmp(arg, "-keep-ods")) no_odf_conf = GF_TRUE;
		else if (!stricmp(arg, "-bt")) dump_mode = GF_SM_DUMP_BT;
		else if (!stricmp(arg, "-xmt")) dump_mode = GF_SM_DUMP_XMTA;
		else if (!stricmp(arg, "-wrl")) dump_mode = GF_SM_DUMP_VRML;
		else if (!stricmp(arg, "-x3dv")) dump_mode = GF_SM_DUMP_X3D_VRML;
		else if (!stricmp(arg, "-x3d")) dump_mode = GF_SM_DUMP_X3D_XML;
		else if (!stricmp(arg, "-lsr")) dump_mode = GF_SM_DUMP_LASER;
		else if (!stricmp(arg, "-svg")) dump_mode = GF_SM_DUMP_SVG;
#endif /*defined(GPAC_DISABLE_MEDIA_EXPORT) && !defined(GPAC_DISABLE_SCENE_DUMP)*/

		else if (!stricmp(arg, "-stat")) stat_level = 1;
		else if (!stricmp(arg, "-stats")) stat_level = 2;
		else if (!stricmp(arg, "-statx")) stat_level = 3;
		else if (!stricmp(arg, "-diso")) dump_isom = 1;
		else if (!stricmp(arg, "-dxml")) dump_isom = 2;
		else if (!stricmp(arg, "-disox")) dump_isom = 3;
		else if (!stricmp(arg, "-mergevtt")) merge_vtt_cues = GF_TRUE;
		else if (!stricmp(arg, "-dump-cover")) dump_cart = 1;
		else if (!stricmp(arg, "-dump-chap")) dump_chap = 1;
		else if (!stricmp(arg, "-dump-chap-ogg")) dump_chap = 2;
		else if (!stricmp(arg, "-dump-chap-zoom")) dump_chap = 3;
		else if (!stricmp(arg, "-hash")) do_hash = GF_TRUE;
		else if (!strnicmp(arg, "-comp", 5)) {
			CHECK_NEXT_ARG

			if (strchr(arg, 'x')) comp_top_box_version = 1;
			else if (strchr(arg, 'f')) comp_top_box_version = 2;

			if (strchr(arg, 'l')) comp_lzma = GF_TRUE;

			compress_top_boxes = argv[i + 1];
			i++;
		}
		else if (!strnicmp(arg, "-topsize", 8)) {
			CHECK_NEXT_ARG
			size_top_box = 1;
			compress_top_boxes = argv[i + 1];
			i++;
		}
		else if (!strnicmp(arg, "-topcount", 8)) {
			CHECK_NEXT_ARG
			size_top_box = 2;
			compress_top_boxes = argv[i + 1];
			i++;
		}
		else if (!stricmp(arg, "-mpd-rip")) do_mpd_rip = GF_TRUE;
		else if (!strcmp(arg, "-init-seg")) {
			CHECK_NEXT_ARG
			use_init_seg = argv[i + 1];
			i += 1;
		}

#ifndef GPAC_DISABLE_CORE_TOOLS
		else if (!stricmp(arg, "-bin")) do_bin_xml = GF_TRUE;
#endif
		else if (!stricmp(arg, "-dump-udta")) {
			char *sep, *code;
			CHECK_NEXT_ARG
			sep = strchr(argv[i + 1], ':');
			if (sep) {
				sep[0] = 0;
				dump_udta_track = atoi(argv[i + 1]);
				sep[0] = ':';
				code = &sep[1];
			}
			else {
				code = argv[i + 1];
			}
			if (strlen(code) == 4) {
				dump_udta_type = GF_4CC(code[0], code[1], code[2], code[3]);
			} else if (strlen(code) == 8) {
				// hex representation on 8 chars
				u32 hex1, hex2, hex3, hex4;
				if (sscanf(code, "%02x%02x%02x%02x", &hex1, &hex2, &hex3, &hex4) != 4) {
					fprintf(stderr, "udta code is either a 4CC or 8 hex chars for non-printable 4CC\n");
					return mp4box_cleanup(1);
				}
				dump_udta_type = GF_4CC(hex1, hex2, hex3, hex4);
			} else {
				fprintf(stderr, "udta code is either a 4CC or 8 hex chars for non-printable 4CC\n");
				return mp4box_cleanup(1);
			}
			i++;
		}
		else if (!stricmp(arg, "-dmp4")) {
			dump_isom = 1;
			fprintf(stderr, "WARNING: \"-dmp4\" is deprecated - use \"-diso\" option\n");
		}
		else if (!stricmp(arg, "-drtp")) dump_rtp = 1;
		else if (!stricmp(arg, "-dts")) {
			dump_timestamps = 1;
			if (((i + 1<(u32)argc) && inName) || (i + 2<(u32)argc)) {
				if (isdigit(argv[i + 1][0])) {
					program_number = atoi(argv[i + 1]);
					i++;
				}
			}
		}
		else if (!stricmp(arg, "-dtsx")) {
			dump_timestamps = 2;
		}
		else if (!stricmp(arg, "-dtsc")) {
			dump_timestamps = 3;
		}
		else if (!stricmp(arg, "-dtsxc")) {
			dump_timestamps = 4;
		}
		else if (!strnicmp(arg, "-dnal", 5)) {
			CHECK_NEXT_ARG
			dump_nal = atoi(argv[i + 1]);
			if (arg[5] == 'c') dump_nal_type |= 1;
			else if (arg[5] == 'd') dump_nal_type |= 2;
			else if (arg[5] == 'x') dump_nal_type |= 2|1;
			i++;
		}
		else if (!strnicmp(arg, "-dsap", 5)) {
			CHECK_NEXT_ARG
			dump_saps = atoi(argv[i + 1]);
			if (!stricmp(arg, "-dsaps")) dump_saps_mode = 1;
			else if (!stricmp(arg, "-dsapc")) dump_saps_mode = 2;
			else if (!stricmp(arg, "-dsapd")) dump_saps_mode = 3;
			else if (!stricmp(arg, "-dsapp")) dump_saps_mode = 4;
			else dump_saps_mode = 0;
			i++;
		}
		else if (!stricmp(arg, "-dcr")) dump_cr = 1;
		else if (!stricmp(arg, "-ttxt") || !stricmp(arg, "-srt")) {
			if ((i + 1<(u32)argc) && (sscanf(argv[i + 1], "%u", &trackID) == 1)) {
				char szTk[20];
				sprintf(szTk, "%d", trackID);
				if (!strcmp(szTk, argv[i + 1])) i++;
				else trackID = 0;
			}
			else if ((i + 1<(u32)argc) && !strcmp(argv[i + 1], "*")) {
				trackID = (u32)-1;
				i++;
			}
			else {
				trackID = 0;
			}
#ifdef GPAC_DISABLE_ISOM_WRITE
			if (trackID) {
				fprintf(stderr, "Error: Read-Only version - subtitle conversion not available\n");
				return 2;
			}
#endif //GPAC_DISABLE_ISOM_WRITE
			if (!stricmp(arg, "-ttxt")) dump_ttxt = GF_TRUE;
			else dump_srt = GF_TRUE;
			import_subtitle = 1;
		}
		else if (!stricmp(arg, "-dm2ts")) {
			dump_m2ts = 1;
			if (((i + 1<(u32)argc) && inName) || (i + 2<(u32)argc)) {
				if (argv[i + 1][0] != '-') pes_dump = argv[i + 1];
				i++;
			}
		}

#ifndef GPAC_DISABLE_SWF_IMPORT
		/*SWF importer options*/
		else if (!stricmp(arg, "-global")) swf_flags |= GF_SM_SWF_STATIC_DICT;
		else if (!stricmp(arg, "-no-ctrl")) swf_flags &= ~GF_SM_SWF_SPLIT_TIMELINE;
		else if (!stricmp(arg, "-no-text")) swf_flags |= GF_SM_SWF_NO_TEXT;
		else if (!stricmp(arg, "-no-font")) swf_flags |= GF_SM_SWF_NO_FONT;
		else if (!stricmp(arg, "-no-line")) swf_flags |= GF_SM_SWF_NO_LINE;
		else if (!stricmp(arg, "-no-grad")) swf_flags |= GF_SM_SWF_NO_GRADIENT;
		else if (!stricmp(arg, "-quad")) swf_flags |= GF_SM_SWF_QUAD_CURVE;
		else if (!stricmp(arg, "-xlp")) swf_flags |= GF_SM_SWF_SCALABLE_LINE;
		else if (!stricmp(arg, "-ic2d")) swf_flags |= GF_SM_SWF_USE_IC2D;
		else if (!stricmp(arg, "-same-app")) swf_flags |= GF_SM_SWF_REUSE_APPEARANCE;
		else if (!stricmp(arg, "-flatten")) {
			CHECK_NEXT_ARG
			swf_flatten_angle = (Float)atof(argv[i + 1]);
			i++;
		}
#endif
#ifndef GPAC_DISABLE_ISOM_WRITE
		else if (!stricmp(arg, "-isma")) {
			conv_type = GF_ISOM_CONV_TYPE_ISMA;
			open_edit = GF_TRUE;
		}
		else if (!stricmp(arg, "-3gp")) {
			conv_type = GF_ISOM_CONV_TYPE_3GPP;
			open_edit = GF_TRUE;
		}
		else if (!stricmp(arg, "-ipod")) {
			conv_type = GF_ISOM_CONV_TYPE_IPOD;
			open_edit = GF_TRUE;
		}
		else if (!stricmp(arg, "-psp")) {
			conv_type = GF_ISOM_CONV_TYPE_PSP;
			open_edit = GF_TRUE;
		}
		else if (!stricmp(arg, "-ismax")) {
			conv_type = GF_ISOM_CONV_TYPE_ISMA_EX;
			open_edit = GF_TRUE;
		}

		else if (!stricmp(arg, "-no-sys") || !stricmp(arg, "-nosys")) {
			remove_sys_tracks = 1;
			open_edit = GF_TRUE;
		}
		else if (!stricmp(arg, "-no-iod")) {
			remove_root_od = 1;
			open_edit = GF_TRUE;
		}
		else if (!stricmp(arg, "-out")) {
			CHECK_NEXT_ARG outName = argv[i + 1];
			i++;
		}
		else if (!stricmp(arg, "-tmp")) {
			CHECK_NEXT_ARG tmpdir = argv[i + 1];
			i++;
		}
		else if (!stricmp(arg, "-co64")) {
			force_co64 = GF_TRUE;
			open_edit = GF_TRUE;
		}
		else if (!stricmp(arg, "-write-buffer")) {
			CHECK_NEXT_ARG
			fprintf(stderr, "\tWARNING: \"-write-buffer\" deprecated and will soon be removed, use -bs-cache-size=%s\n", argv[i + 1]);
			gf_opts_set_key("temp", "bs-cache-size", argv[i + 1]);
			i++;
		}
		else if (!stricmp(arg, "-cprt")) {
			CHECK_NEXT_ARG cprt = argv[i + 1];
			i++;
			if (!dash_duration) open_edit = GF_TRUE;
		}
		else if (!stricmp(arg, "-chap") || !stricmp(arg, "-chapqt")) {
			CHECK_NEXT_ARG
			chap_file = argv[i + 1];
			i++;
			open_edit = GF_TRUE;
			if (!stricmp(arg, "-chapqt")) chap_qt = GF_TRUE;
		}
		else if (!stricmp(arg, "-inter") || !stricmp(arg, "-old-inter")) {
			CHECK_NEXT_ARG
			interleaving_time = atof(argv[i + 1]) / 1000;
			if (!interleaving_time) do_flat = 2;
			open_edit = GF_TRUE;
			needSave = GF_TRUE;
			if (!stricmp(arg, "-old-inter")) old_interleave = 1;
			i++;
		}
		else if (!stricmp(arg, "-frag")) {
			CHECK_NEXT_ARG
			interleaving_time = atof(argv[i + 1]) / 1000;
			needSave = GF_TRUE;
			i++;
			Frag = GF_TRUE;
		}
		else if (!stricmp(arg, "-dash")) {
			CHECK_NEXT_ARG
			dash_duration = atof(argv[i + 1]) / 1000;
			if (dash_duration == 0.0) {
				fprintf(stderr, "\tERROR: \"-dash-dash_duration\": invalid parameter %s\n", argv[i + 1]);
				return 2;
			}
			i++;
		}
		else if (!stricmp(arg, "-dash-strict")) {
			CHECK_NEXT_ARG
			dash_duration = atof(argv[i + 1]) / 1000;
			if (dash_duration == 0.0) {
				fprintf(stderr, "\tERROR: \"-dash-dash_duration\": invalid parameter %s\n", argv[i + 1]);
				return 2;
			}
			GF_LOG(GF_LOG_WARNING, GF_LOG_DASH, ("[DASH] -dash-strict is deprecated, will behave like -dash\n"));
			i++;
		}
		else if (!stricmp(arg, "-subdur")) {
			CHECK_NEXT_ARG
			dash_subduration = atof(argv[i + 1]) / 1000;
			i++;
		}
		else if (!stricmp(arg, "-dash-scale")) {
			CHECK_NEXT_ARG
			dash_scale = atoi(argv[i + 1]);
			if (!dash_scale) {
				fprintf(stderr, "\tERROR: \"-dash-scale\": invalid parameter %s\n", argv[i + 1]);
				return 2;
			}
			i++;
		}
		else if (!stricmp(arg, "-dash-ts-prog")) {
			CHECK_NEXT_ARG
			program_number = atoi(argv[i + 1]);
			i++;
		}
		else if (!stricmp(arg, "-subsegs-per-sidx") || !stricmp(arg, "-frags-per-sidx")) {
			CHECK_NEXT_ARG
			subsegs_per_sidx = atoi(argv[i + 1]);
			i++;
		}
		else if (!stricmp(arg, "-segment-name")) {
			CHECK_NEXT_ARG
			seg_name = argv[i + 1];
			i++;
		}
		else if (!stricmp(arg, "-run-for")) {
			CHECK_NEXT_ARG
			run_for = atoi(argv[i + 1]);
			i++;
		}
		else if (!stricmp(arg, "-no-cache")) {
			no_cache = GF_TRUE;
		}
		else if (!stricmp(arg, "-no-loop")) {
			no_loop = GF_TRUE;
		}
		else if (!stricmp(arg, "-hlsc")) {
			hls_clock = GF_TRUE;
		}
		else if (!stricmp(arg, "-bound")) {
			dash_split_mode = GF_DASH_SPLIT_IN;
		}
		else if (!stricmp(arg, "-closest")) {
			dash_split_mode = GF_DASH_SPLIT_CLOSEST;
		}
		else if (!stricmp(arg, "-segment-ext")) {
			CHECK_NEXT_ARG
			seg_ext = argv[i + 1];
			i++;
		}
		else if (!stricmp(arg, "-init-segment-ext")) {
			CHECK_NEXT_ARG
			init_seg_ext = argv[i + 1];
			i++;
		}
		else if (!stricmp(arg, "-bs-switching")) {
			CHECK_NEXT_ARG
			if (!stricmp(argv[i + 1], "no") || !stricmp(argv[i + 1], "off")) bitstream_switching_mode = GF_DASH_BSMODE_NONE;
			else if (!stricmp(argv[i + 1], "merge"))  bitstream_switching_mode = GF_DASH_BSMODE_MERGED;
			else if (!stricmp(argv[i + 1], "multi"))  bitstream_switching_mode = GF_DASH_BSMODE_MULTIPLE_ENTRIES;
			else if (!stricmp(argv[i + 1], "single"))  bitstream_switching_mode = GF_DASH_BSMODE_SINGLE;
			else if (!stricmp(argv[i + 1], "inband"))  bitstream_switching_mode = GF_DASH_BSMODE_INBAND;
			else {
				fprintf(stderr, "\tWARNING: Unrecognized bitstream switchin mode \"%s\" - please check usage\n", argv[i + 1]);
				return 2;
			}
			i++;
		}
		else if (!stricmp(arg, "-dynamic")) {
			dash_mode = GF_DASH_DYNAMIC;
		}
		else if (!stricmp(arg, "-last-dynamic")) {
			dash_mode = GF_DASH_DYNAMIC_LAST;
		}
		else if (!stricmp(arg, "-frag-rt")) {
			frag_real_time = GF_TRUE;
		}
		else if (!stricmp(arg, "-start-date")) {
			dash_start_date = argv[i+1];
			i++;
		}
		else if (!strnicmp(arg, "-cp-location=", 13)) {
			if (strcmp(arg+13, "both")) cp_location_mode = GF_DASH_CPMODE_BOTH;
			else if (strcmp(arg+13, "as")) cp_location_mode = GF_DASH_CPMODE_ADAPTATION_SET;
			else if (strcmp(arg+13, "rep")) cp_location_mode = GF_DASH_CPMODE_REPRESENTATION;
			else {
				fprintf(stderr, "\tWARNING: Unrecognized ContentProtection loction mode \"%s\" - please check usage\n", argv[i + 13]);
				return 2;
			}
		}
		else if (!strnicmp(arg, "-dash-live", 10) || !strnicmp(arg, "-ddbg-live", 10)) {
			dash_mode = !strnicmp(arg, "-ddbg-live", 10) ? GF_DASH_DYNAMIC_DEBUG : GF_DASH_DYNAMIC;
			dash_live = 1;
			if (arg[10] == '=') {
				dash_ctx_file = arg + 11;
			}
			CHECK_NEXT_ARG
			dash_duration = atof(argv[i + 1]) / 1000;
			i++;
		}
		else if (!stricmp(arg, "-mpd-duration")) {
			CHECK_NEXT_ARG mpd_live_duration = atof(argv[i + 1]);
			i++;
		}
		else if (!stricmp(arg, "-mpd-refresh")) {
			CHECK_NEXT_ARG mpd_update_time = atof(argv[i + 1]);
			i++;
		}
		else if (!stricmp(arg, "-time-shift")) {
			CHECK_NEXT_ARG
			time_shift_depth = (u32)atoi(argv[i + 1]);
			i++;
		}
		else if (!stricmp(arg, "-min-buffer")) {
			CHECK_NEXT_ARG
			min_buffer = atoi(argv[i + 1]);
			min_buffer /= 1000;
			i++;
		}
		else if (!stricmp(arg, "-ast-offset")) {
			CHECK_NEXT_ARG
			ast_offset_ms = atoi(argv[i + 1]);
			i++;
		}
		else if (!stricmp(arg, "-moof-sn")) {
			CHECK_NEXT_ARG
			initial_moof_sn = (u32)atoi(argv[i + 1]);
			i++;
		}
		else if (!stricmp(arg, "-tfdt")) {
			CHECK_NEXT_ARG
			sscanf(argv[i + 1], LLU, &initial_tfdt);
			i++;
		}
		else if (!stricmp(arg, "-no-frags-default")) {
			no_fragments_defaults = 1;
		}
		else if (!stricmp(arg, "-single-traf")) {
			single_traf_per_moof = 1;
		}
        else if (!stricmp(arg, "-tfdt-traf")) {
            tfdt_per_traf = 1;
        }
		else if (!stricmp(arg, "-mpd-title")) {
			CHECK_NEXT_ARG dash_title = argv[i + 1];
			i++;
		}
		else if (!stricmp(arg, "-mpd-source")) {
			CHECK_NEXT_ARG dash_source = argv[i + 1];
			i++;
		}
		else if (!stricmp(arg, "-mpd-info-url")) {
			CHECK_NEXT_ARG dash_more_info = argv[i + 1];
			i++;
		}
		else if (!stricmp(arg, "-base-url")) {
			CHECK_NEXT_ARG
			dash_more_info = argv[i + 1];
			mpd_base_urls = gf_realloc(mpd_base_urls, (nb_mpd_base_urls + 1)*sizeof(char**));
			mpd_base_urls[nb_mpd_base_urls] = argv[i + 1];
			nb_mpd_base_urls++;
			i++;
		}
		else if (!stricmp(arg, "-dash-ctx")) {
			CHECK_NEXT_ARG
			dash_ctx_file = argv[i + 1];
			i++;
		}
		else if (!stricmp(arg, "-ssix")) {
			use_ssix = 1;
		}
		else if (!stricmp(arg, "-daisy-chain")) {
			daisy_chain_sidx = 1;
		}
		else if (!stricmp(arg, "-single-segment")) {
			single_segment = 1;
		}
		else if (!stricmp(arg, "-single-file")) {
			single_file = 1;
		}
		else if (!stricmp(arg, "-pssh-moof")) {
			pssh_mode = GF_DASH_PSSH_MOOF;
		}
		else if (!strnicmp(arg, "-pssh=", 6)) {
			if (!strcmp(arg+6, "f")) pssh_mode = GF_DASH_PSSH_MOOF;
			else if (!strcmp(arg+6, "v")) pssh_mode = GF_DASH_PSSH_MOOV;
			else if (!strcmp(arg+6, "m")) pssh_mode = GF_DASH_PSSH_MPD;
			else if (!strcmp(arg+6, "mf") || !strcmp(arg+6, "fm")) pssh_mode = GF_DASH_PSSH_MOOF_MPD;
			else if (!strcmp(arg+6, "mv") || !strcmp(arg+6, "vm")) pssh_mode = GF_DASH_PSSH_MOOV_MPD;
			else pssh_mode = GF_DASH_PSSH_MOOV;
		}
		else if (!stricmp(arg, "-sample-groups-traf")) {
			samplegroups_in_traf = 1;
		}
		else if (!stricmp(arg, "-mvex-after-traks")) {
			mvex_after_traks = GF_TRUE;
		}
		else if (!stricmp(arg, "-sdtp-traf")) {
			CHECK_NEXT_ARG
			if (!stricmp(argv[i + 1], "both")) sdtp_in_traf = 2;
			else if (!stricmp(argv[i + 1], "sdtp")) sdtp_in_traf = 1;
			else sdtp_in_traf = 0;
			i++;
		}
		else if (!stricmp(arg, "-dash-profile") || !stricmp(arg, "-profile")) {
			CHECK_NEXT_ARG
			if (!stricmp(argv[i + 1], "live") || !stricmp(argv[i + 1], "simple")) dash_profile = GF_DASH_PROFILE_LIVE;
			else if (!stricmp(argv[i + 1], "onDemand")) dash_profile = GF_DASH_PROFILE_ONDEMAND;
			else if (!stricmp(argv[i + 1], "hbbtv1.5:live")) {
				dash_profile = GF_DASH_PROFILE_HBBTV_1_5_ISOBMF_LIVE;
			}
			else if (!stricmp(argv[i + 1], "dashavc264:live")) {
				dash_profile = GF_DASH_PROFILE_AVC264_LIVE;
			}
			else if (!stricmp(argv[i + 1], "dashavc264:onDemand")) {
				dash_profile = GF_DASH_PROFILE_AVC264_ONDEMAND;
			}
			else if (!stricmp(argv[i + 1], "main")) dash_profile = GF_DASH_PROFILE_MAIN;
			else if (!stricmp(argv[i + 1], "full")) dash_profile = GF_DASH_PROFILE_FULL;
			else {
				fprintf(stderr, "\tWARNING: Unrecognized DASH profile \"%s\" - please check usage\n", argv[i + 1]);
				return 2;
			}
			i++;
		}
		else if (!stricmp(arg, "-profile-ext")) {
			CHECK_NEXT_ARG
			dash_profile_extension = argv[i + 1];
			i++;
		}
		else if (!strnicmp(arg, "-url-template", 13)) {
			use_url_template = 1;
			if ((arg[13] == '=') && arg[14]) {
				if (!strcmp(&arg[14], "simulate")) use_url_template = 2;
			}
		}
		else if (!stricmp(arg, "-segment-timeline")) {
			segment_timeline = 1;
		}
		else if (!stricmp(arg, "-mem-frags")) {
			memory_frags = 1;
		}
		else if (!stricmp(arg, "-segment-marker")) {
			char *m;
			CHECK_NEXT_ARG
			m = argv[i + 1];
			segment_marker = GF_4CC(m[0], m[1], m[2], m[3]);
			i++;
		}
		else if (!stricmp(arg, "-cues")) {
			CHECK_NEXT_ARG
			dash_cues = argv[i + 1];
			i++;
		}
		else if (!stricmp(arg, "-strict-cues")) {
			strict_cues = GF_TRUE;
		}
		else if (!stricmp(arg, "-insert-utc")) {
			insert_utc = GF_TRUE;
		}
#endif //GPAC_DISABLE_ISOM_WRITE
		else if (!stricmp(arg, "-udp-write")) {
			udp_dest = argv[i+1];
			i++;
		}
		else {
			u32 ret = mp4box_parse_args_continue(argc, argv, &i);
			if (ret) return ret;
		}
	}
	return 0;
}