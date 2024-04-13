u32 mp4box_parse_args_continue(int argc, char **argv, u32 *current_index)
{
	u32 i = *current_index;
	/*parse our args*/
	{
		char *arg = argv[i];
		if (!stricmp(arg, "-itags")) {
			CHECK_NEXT_ARG
			itunes_tags = argv[i + 1];
			i++;
			open_edit = GF_TRUE;
		}
#ifndef GPAC_DISABLE_ISOM_HINTING
		else if (!stricmp(arg, "-hint")) {
			open_edit = GF_TRUE;
			HintIt = 1;
		}
		else if (!stricmp(arg, "-unhint")) {
			open_edit = GF_TRUE;
			remove_hint = 1;
		}
		else if (!stricmp(arg, "-copy")) HintCopy = 1;
		else if (!stricmp(arg, "-no-offset")) hint_no_offset = GF_TRUE;
		else if (!stricmp(arg, "-tight")) {
			FullInter = 1;
			open_edit = GF_TRUE;
			needSave = GF_TRUE;
		}
		else if (!stricmp(arg, "-ocr")) force_ocr = 1;
		else if (!stricmp(arg, "-latm")) hint_flags |= GP_RTP_PCK_USE_LATM_AAC;
		else if (!stricmp(arg, "-rap") || !stricmp(arg, "-refonly")) {
			if ((i + 1 < (u32)argc) && (argv[i + 1][0] != '-')) {
				if (sscanf(argv[i + 1], "%d", &trackID) == 1) {
					tracks = gf_realloc(tracks, sizeof(TrackAction) * (nb_track_act + 1));
					memset(&tracks[nb_track_act], 0, sizeof(TrackAction));
					tracks[nb_track_act].act_type = !stricmp(arg, "-rap") ? TRAC_ACTION_REM_NON_RAP : TRAC_ACTION_REM_NON_REFS;
					tracks[nb_track_act].trackID = trackID;
					nb_track_act++;
					i++;
					open_edit = GF_TRUE;
				}
			}
			hint_flags |= GP_RTP_PCK_SIGNAL_RAP;
			seg_at_rap = 1;
		}
		else if (!stricmp(arg, "-frag-rap")) {
			frag_at_rap = 1;
		}
		else if (!stricmp(arg, "-mfra")) {
			use_mfra = GF_TRUE;
		}
		else if (!stricmp(arg, "-ts")) hint_flags |= GP_RTP_PCK_SIGNAL_TS;
		else if (!stricmp(arg, "-size")) hint_flags |= GP_RTP_PCK_SIGNAL_SIZE;
		else if (!stricmp(arg, "-idx")) hint_flags |= GP_RTP_PCK_SIGNAL_AU_IDX;
		else if (!stricmp(arg, "-static")) hint_flags |= GP_RTP_PCK_USE_STATIC_ID;
		else if (!stricmp(arg, "-multi")) {
			hint_flags |= GP_RTP_PCK_USE_MULTI;
			if ((i + 1 < (u32)argc) && (sscanf(argv[i + 1], "%u", &max_ptime) == 1)) {
				char szPt[20];
				sprintf(szPt, "%u", max_ptime);
				if (!strcmp(szPt, argv[i + 1])) i++;
				else max_ptime = 0;
			}
		}
#endif
		else if (!stricmp(arg, "-mpeg4")) {
#ifndef GPAC_DISABLE_ISOM_HINTING
			hint_flags |= GP_RTP_PCK_FORCE_MPEG4;
#endif
#ifndef GPAC_DISABLE_MEDIA_IMPORT
			import_flags |= GF_IMPORT_FORCE_MPEG4;
#endif
		}
#ifndef GPAC_DISABLE_ISOM_HINTING
		else if (!stricmp(arg, "-mtu")) {
			CHECK_NEXT_ARG
			MTUSize = atoi(argv[i + 1]);
			i++;
		}
		else if (!stricmp(arg, "-cardur")) {
			CHECK_NEXT_ARG
			car_dur = atoi(argv[i + 1]);
			i++;
		}
		else if (!stricmp(arg, "-rate")) {
			CHECK_NEXT_ARG
			rtp_rate = atoi(argv[i + 1]);
			i++;
		}
#ifndef GPAC_DISABLE_SENG
		else if (!stricmp(arg, "-add-sdp") || !stricmp(arg, "-sdp_ex")) {
			char *id;
			CHECK_NEXT_ARG
			sdp_lines = gf_realloc(sdp_lines, sizeof(SDPLine) * (nb_sdp_ex + 1));

			id = strchr(argv[i + 1], ':');
			if (id) {
				id[0] = 0;
				if (sscanf(argv[i + 1], "%u", &sdp_lines[0].trackID) == 1) {
					id[0] = ':';
					sdp_lines[nb_sdp_ex].line = id + 1;
				}
				else {
					id[0] = ':';
					sdp_lines[nb_sdp_ex].line = argv[i + 1];
					sdp_lines[nb_sdp_ex].trackID = 0;
				}
			}
			else {
				sdp_lines[nb_sdp_ex].line = argv[i + 1];
				sdp_lines[nb_sdp_ex].trackID = 0;
			}
			open_edit = GF_TRUE;
			nb_sdp_ex++;
			i++;
		}
#endif /*GPAC_DISABLE_SENG*/
#endif /*GPAC_DISABLE_ISOM_HINTING*/

		else if (!stricmp(arg, "-single")) {
#ifndef GPAC_DISABLE_MEDIA_EXPORT
			CHECK_NEXT_ARG
			track_dump_type = GF_EXPORT_MP4;
			tracks = gf_realloc(tracks, sizeof(TrackAction) * (nb_track_act + 1));
			memset(&tracks[nb_track_act], 0, sizeof(TrackAction));
			tracks[nb_track_act].act_type = TRAC_ACTION_RAW_EXTRACT;
			tracks[nb_track_act].trackID = atoi(argv[i + 1]);
			tracks[nb_track_act].dump_type = GF_EXPORT_MP4;
			nb_track_act++;
			i++;
#endif
		}
		else if (!stricmp(arg, "-iod")) regular_iod = 1;
		else if (!stricmp(arg, "-flat")) {
			open_edit = GF_TRUE;
			do_flat = 1;
		}
		else if (!stricmp(arg, "-keep-utc")) keep_utc = GF_TRUE;
		else if (!stricmp(arg, "-new")) force_new = 1;
		else if (!stricmp(arg, "-newfs")) {
			force_new = 2;
			interleaving_time = 0.5;
			do_flat = 1;
		}
		else if (!stricmp(arg, "-timescale")) {
			CHECK_NEXT_ARG
			timescale = atoi(argv[i + 1]);
			open_edit = GF_TRUE;
			i++;
		}
		else if (!stricmp(arg, "-udta")) {
			CHECK_NEXT_ARG
			create_new_track_action(argv[i + 1], &tracks, &nb_track_act, 0);
			tracks[nb_track_act - 1].act_type = TRAC_ACTION_SET_UDTA;
			open_edit = GF_TRUE;
			i++;
		}
		else if (!stricmp(arg, "-add") || !stricmp(arg, "-import") || !stricmp(arg, "-convert")) {
			CHECK_NEXT_ARG
			if (!stricmp(arg, "-import")) fprintf(stderr, "\tWARNING: \"-import\" is deprecated - use \"-add\"\n");
			else if (!stricmp(arg, "-convert")) fprintf(stderr, "\tWARNING: \"-convert\" is deprecated - use \"-add\"\n");
			nb_add++;
			i++;
		}
		else if (!stricmp(arg, "-cat") || !stricmp(arg, "-catx") || !stricmp(arg, "-catpl")) {
			CHECK_NEXT_ARG
			nb_cat++;
			i++;
		}
		else if (!stricmp(arg, "-time")) {
			struct tm time;
			CHECK_NEXT_ARG
			memset(&time, 0, sizeof(struct tm));
			sscanf(argv[i + 1], "%d/%d/%d-%d:%d:%d", &time.tm_mday, &time.tm_mon, &time.tm_year, &time.tm_hour, &time.tm_min, &time.tm_sec);
			time.tm_isdst = 0;
			time.tm_year -= 1900;
			time.tm_mon -= 1;
			open_edit = GF_TRUE;
			movie_time = 2082758400;
			movie_time += mktime(&time);
			i++;
		}
		else if (!stricmp(arg, "-force-cat")) force_cat = 1;
		else if (!stricmp(arg, "-align-cat")) align_cat = 1;
		else if (!stricmp(arg, "-unalign-cat")) align_cat = 0;
		else if (!stricmp(arg, "-raw-cat")) {
			CHECK_NEXT_ARG
			raw_cat = argv[i + 1];
			i++;
		}
		else if (!stricmp(arg, "-rem") || !stricmp(arg, "-disable") || !stricmp(arg, "-enable")) {
			CHECK_NEXT_ARG
			tracks = gf_realloc(tracks, sizeof(TrackAction) * (nb_track_act + 1));
			memset(&tracks[nb_track_act], 0, sizeof(TrackAction));
			if (!stricmp(arg, "-enable")) tracks[nb_track_act].act_type = TRAC_ACTION_ENABLE;
			else if (!stricmp(arg, "-disable")) tracks[nb_track_act].act_type = TRAC_ACTION_DISABLE;
			else tracks[nb_track_act].act_type = TRAC_ACTION_REM_TRACK;
			tracks[nb_track_act].trackID = atoi(argv[i + 1]);
			open_edit = GF_TRUE;
			nb_track_act++;
			i++;
		}
		else if (!stricmp(arg, "-set-track-id") || !stricmp(arg, "-swap-track-id")) {
			char *sep;
			CHECK_NEXT_ARG
			tracks = gf_realloc(tracks, sizeof(TrackAction) * (nb_track_act + 1));
			memset(&tracks[nb_track_act], 0, sizeof(TrackAction));
			tracks[nb_track_act].act_type = !stricmp(arg, "-set-track-id") ? TRAC_ACTION_SET_ID : TRAC_ACTION_SWAP_ID;
			sep = strchr(argv[i + 1], ':');
			if (!sep) {
				fprintf(stderr, "Bad format for -set-track-id - expecting \"id1:id2\" got \"%s\"\n", argv[i + 1]);
				return 2;
			}
			*sep = 0;
			tracks[nb_track_act].trackID = atoi(argv[i + 1]);
			*sep = ':';
			sep++;
			tracks[nb_track_act].newTrackID = atoi(sep);
			open_edit = GF_TRUE;
			nb_track_act++;
			i++;
		}
		else if (!stricmp(arg, "-par")) {
			char szTK[20], *ext;
			CHECK_NEXT_ARG
			tracks = gf_realloc(tracks, sizeof(TrackAction) * (nb_track_act + 1));
			memset(&tracks[nb_track_act], 0, sizeof(TrackAction));

			tracks[nb_track_act].act_type = TRAC_ACTION_SET_PAR;
			assert(strlen(argv[i + 1]) + 1 <= sizeof(szTK));
			strncpy(szTK, argv[i + 1], sizeof(szTK));
			ext = strchr(szTK, '=');
			if (!ext) {
				fprintf(stderr, "Bad format for track par - expecting tkID=none or tkID=PAR_NUM:PAR_DEN got %s\n", argv[i + 1]);
				return 2;
			}
			if (!stricmp(ext + 1, "none")) {
				tracks[nb_track_act].par_num = tracks[nb_track_act].par_den = 0;
			}
			else if (!stricmp(ext + 1, "auto")) {
				tracks[nb_track_act].par_num = tracks[nb_track_act].par_den = -1;
				tracks[nb_track_act].force_par = 1;
			}
			else if (!stricmp(ext + 1, "force")) {
				tracks[nb_track_act].par_num = tracks[nb_track_act].par_den = 1;
				tracks[nb_track_act].force_par = 1;
			}
			else {
				if (ext[1]=='w') {
					tracks[nb_track_act].rewrite_bs = 1;
					ext++;
				}
				sscanf(ext + 1, "%d", &tracks[nb_track_act].par_num);
				ext = strchr(ext + 1, ':');
				if (!ext) {
					fprintf(stderr, "Bad format for track par - expecting tkID=PAR_NUM:PAR_DEN got %s\n", argv[i + 1]);
					return 2;
				}
				sscanf(ext + 1, "%d", &tracks[nb_track_act].par_den);
			}
			ext[0] = 0;
			tracks[nb_track_act].trackID = atoi(szTK);
			open_edit = GF_TRUE;
			nb_track_act++;
			i++;
		}
		else if (!stricmp(arg, "-clap")) {
			char szTK[200], *ext;
			TrackAction *tka;
			CHECK_NEXT_ARG
			tracks = gf_realloc(tracks, sizeof(TrackAction) * (nb_track_act + 1));
			memset(&tracks[nb_track_act], 0, sizeof(TrackAction));

			tracks[nb_track_act].act_type = TRAC_ACTION_SET_CLAP;
			assert(strlen(argv[i + 1]) + 1 <= sizeof(szTK));
			strncpy(szTK, argv[i + 1], sizeof(szTK));
			ext = strchr(szTK, '=');
			if (!ext) {
				fprintf(stderr, "Bad format for track clap - expecting tkID=none or tkID=Wn,Wd,Hn,Hd,HOn,HOd,VOn,VOd got %s\n", argv[i + 1]);
				return 2;
			}
			tka = &tracks[nb_track_act];
			if (!stricmp(ext + 1, "none")) {
				tka->clap_wnum= tka->clap_wden = tka->clap_hnum = tka->clap_hden = tka->clap_honum = tka->clap_hoden = tka->clap_vonum = tka->clap_voden = 0;
			} else {
				if (sscanf(ext + 1, "%d,%d,%d,%d,%d,%d,%d,%d", &tka->clap_wnum, &tka->clap_wden, &tka->clap_hnum, &tka->clap_hden, &tka->clap_honum, &tka->clap_hoden, &tka->clap_vonum, &tka->clap_voden) != 8) {

					fprintf(stderr, "Bad format for track clap - expecting tkID=none or tkID=Wn,Wd,Hn,Hd,HOn,HOd,VOn,VOd got %s\n", argv[i + 1]);
					return 2;
				}
			}
			ext[0] = 0;
			tracks[nb_track_act].trackID = atoi(szTK);
			open_edit = GF_TRUE;
			nb_track_act++;
			i++;
		}
		else if (!stricmp(arg, "-mx")) {
			char szTK[200], *ext;
			TrackAction *tka;
			CHECK_NEXT_ARG
			tracks = gf_realloc(tracks, sizeof(TrackAction) * (nb_track_act + 1));
			memset(&tracks[nb_track_act], 0, sizeof(TrackAction));

			tracks[nb_track_act].act_type = TRAC_ACTION_SET_MX;
			assert(strlen(argv[i + 1]) + 1 <= sizeof(szTK));
			strncpy(szTK, argv[i + 1], sizeof(szTK));
			ext = strchr(szTK, '=');
			if (!ext) {
				fprintf(stderr, "Bad format for track matrix - expecting ID=none or ID=M1:M2:M3:M4:M5:M6:M7:M8:M9 got %s\n", argv[i + 1]);
				return 2;
			}
			tka = &tracks[nb_track_act];
			if (!stricmp(ext + 1, "none")) {
				memset(tka->mx, 0, sizeof(s32)*9);
			} else {
				s32 res;
				if (strstr(ext+1, "0x")) {
					res = sscanf(ext + 1, "0x%d:0x%d:0x%d:0x%d:0x%d:0x%d:0x%d:0x%d:0x%d", &tka->mx[0], &tka->mx[1], &tka->mx[2], &tka->mx[3], &tka->mx[4], &tka->mx[5], &tka->mx[6], &tka->mx[7], &tka->mx[8]);
				} else {
					res = sscanf(ext + 1, "%d:%d:%d:%d:%d:%d:%d:%d:%d", &tka->mx[0], &tka->mx[1], &tka->mx[2], &tka->mx[3], &tka->mx[4], &tka->mx[5], &tka->mx[6], &tka->mx[7], &tka->mx[8]);
				}
				if (res != 9) {
					fprintf(stderr, "Bad format for track matrix - expecting ID=none or ID=M1:M2:M3:M4:M5:M6:M7:M8:M9 got %s\n", argv[i + 1]);
					return 2;
				}
			}
			ext[0] = 0;
			tracks[nb_track_act].trackID = atoi(szTK);
			open_edit = GF_TRUE;
			nb_track_act++;
			i++;
		}
		else if (!stricmp(arg, "-hdr")) {
			CHECK_NEXT_ARG
			high_dynamc_range_filename = argv[i + 1];
			i++;
		}
		else if (!stricmp(arg, "-bo")) {
			freeze_box_order = GF_TRUE;
		}
		else if (!stricmp(arg, "-patch")) {
			CHECK_NEXT_ARG
			box_patch_filename = argv[i + 1];
			char *sep = strchr(box_patch_filename, '=');
			if (sep) {
				sep[0] = 0;
				box_patch_trackID = atoi(box_patch_filename);
				sep[0] = '=';
				box_patch_filename = sep+1;
			}
 			open_edit = GF_TRUE;
			i++;
		}
		else if (!stricmp(arg, "-lang")) {
			char szTK[20], *ext;
			CHECK_NEXT_ARG
			tracks = gf_realloc(tracks, sizeof(TrackAction) * (nb_track_act + 1));
			memset(&tracks[nb_track_act], 0, sizeof(TrackAction));

			tracks[nb_track_act].act_type = TRAC_ACTION_SET_LANGUAGE;
			tracks[nb_track_act].trackID = 0;
			strncpy(szTK, argv[i + 1], sizeof(szTK)-1);
			szTK[sizeof(szTK)-1] = 0;
			ext = strchr(szTK, '=');
			if (!strnicmp(argv[i + 1], "all=", 4)) {
				strncpy(tracks[nb_track_act].lang, argv[i + 1] + 4, 10);
			}
			else if (!ext) {
				strncpy(tracks[nb_track_act].lang, argv[i + 1], 10);
			}
			else {
				strncpy(tracks[nb_track_act].lang, ext + 1, 10);
				ext[0] = 0;
				tracks[nb_track_act].trackID = atoi(szTK);
				ext[0] = '=';
			}
			open_edit = GF_TRUE;
			nb_track_act++;
			i++;
		}
		else if (!stricmp(arg, "-kind") || !stricmp(arg, "-kind-rem")) {
			char szTK[200], *ext;
			char *scheme_start = NULL;
			Bool has_track_id = GF_FALSE;
			CHECK_NEXT_ARG
			tracks = gf_realloc(tracks, sizeof(TrackAction) * (nb_track_act + 1));
			memset(&tracks[nb_track_act], 0, sizeof(TrackAction));

			if (!stricmp(arg, "-kind")) {
				tracks[nb_track_act].act_type = TRAC_ACTION_SET_KIND;
			}
			else {
				tracks[nb_track_act].act_type = TRAC_ACTION_REM_KIND;
			}
			tracks[nb_track_act].trackID = 0;
			if (!strnicmp(argv[i + 1], "all=", 4)) {
				scheme_start = argv[i + 1] + 4;
				has_track_id = GF_TRUE;
			}
			if (!scheme_start) {
				if (strlen(argv[i + 1]) > 200) {
					GF_LOG(GF_LOG_WARNING, GF_LOG_ALL, ("Warning: track kind parameter is too long!"));
				}
				strncpy(szTK, argv[i + 1], 200);
				ext = strchr(szTK, '=');
				if (ext && !has_track_id) {
					ext[0] = 0;
					has_track_id = (sscanf(szTK, "%d", &tracks[nb_track_act].trackID) == 1 ? GF_TRUE : GF_FALSE);
					if (has_track_id) {
						scheme_start = ext + 1;
					}
					else {
						scheme_start = szTK;
					}
					ext[0] = '=';
				}
				else {
					scheme_start = szTK;
				}
			}
			ext = strchr(scheme_start, '=');
			if (!ext) {
				tracks[nb_track_act].kind_scheme = gf_strdup(scheme_start);
			}
			else {
				ext[0] = 0;
				tracks[nb_track_act].kind_scheme = gf_strdup(scheme_start);
				ext[0] = '=';
				tracks[nb_track_act].kind_value = gf_strdup(ext + 1);
			}
			open_edit = GF_TRUE;
			nb_track_act++;
			i++;
		}
		else if (!stricmp(arg, "-delay")) {
			char szTK[20], *ext;
			CHECK_NEXT_ARG
			tracks = gf_realloc(tracks, sizeof(TrackAction) * (nb_track_act + 1));
			memset(&tracks[nb_track_act], 0, sizeof(TrackAction));

			strncpy(szTK, argv[i + 1], sizeof(szTK)-1);
			szTK[sizeof(szTK)-1] = 0;
			ext = strchr(szTK, '=');
			if (!ext) {
				fprintf(stderr, "Bad format for track delay - expecting tkID=DLAY got %s\n", argv[i + 1]);
				return 2;
			}
			tracks[nb_track_act].act_type = TRAC_ACTION_SET_DELAY;
			tracks[nb_track_act].delay_ms = atoi(ext + 1);
			ext[0] = 0;
			tracks[nb_track_act].trackID = atoi(szTK);
			open_edit = GF_TRUE;
			nb_track_act++;
			i++;
		}
		else if (!stricmp(arg, "-ref")) {
			char *szTK, *ext;
			CHECK_NEXT_ARG
			tracks = gf_realloc(tracks, sizeof(TrackAction) * (nb_track_act + 1));
			memset(&tracks[nb_track_act], 0, sizeof(TrackAction));

			szTK = argv[i + 1];
			ext = strchr(szTK, ':');
			if (!ext) {
				fprintf(stderr, "Bad format for track reference - expecting tkID:XXXX:refID got %s\n", argv[i + 1]);
				return 2;
			}
			tracks[nb_track_act].act_type = TRAC_ACTION_REFERENCE;
			ext[0] = 0;
			tracks[nb_track_act].trackID = atoi(szTK);
			ext[0] = ':';
			szTK = ext + 1;
			ext = strchr(szTK, ':');
			if (!ext) {
				fprintf(stderr, "Bad format for track reference - expecting tkID:XXXX:refID got %s\n", argv[i + 1]);
				return 2;
			}
			ext[0] = 0;
			strncpy(tracks[nb_track_act].lang, szTK, 10);
			ext[0] = ':';
			tracks[nb_track_act].delay_ms = (s32)atoi(ext + 1);
			open_edit = GF_TRUE;
			nb_track_act++;
			i++;
		}
		else if (!stricmp(arg, "-name")) {
			char szTK[GF_MAX_PATH], *ext;
			CHECK_NEXT_ARG
			tracks = gf_realloc(tracks, sizeof(TrackAction) * (nb_track_act + 1));
			memset(&tracks[nb_track_act], 0, sizeof(TrackAction));

			strncpy(szTK, argv[i + 1], sizeof(szTK)-1);
			szTK[sizeof(szTK)-1] = 0;
			ext = strchr(szTK, '=');
			if (!ext) {
				fprintf(stderr, "Bad format for track name - expecting tkID=name got %s\n", argv[i + 1]);
				return 2;
			}
			tracks[nb_track_act].act_type = TRAC_ACTION_SET_HANDLER_NAME;
			tracks[nb_track_act].hdl_name = strchr(argv[i + 1], '=') + 1;
			ext[0] = 0;
			tracks[nb_track_act].trackID = atoi(szTK);
			ext[0] = '=';
			open_edit = GF_TRUE;
			nb_track_act++;
			i++;
		}
#if !defined(GPAC_DISABLE_MEDIA_EXPORT) && !defined(GPAC_DISABLE_MEDIA_IMPORT)
		else if (!stricmp(arg, "-dref")) import_flags |= GF_IMPORT_USE_DATAREF;
		else if (!stricmp(arg, "-no-drop") || !stricmp(arg, "-nodrop")) import_flags |= GF_IMPORT_NO_FRAME_DROP;
		else if (!stricmp(arg, "-packed")) import_flags |= GF_IMPORT_FORCE_PACKED;
		else if (!stricmp(arg, "-sbr")) import_flags |= GF_IMPORT_SBR_IMPLICIT;
		else if (!stricmp(arg, "-sbrx")) import_flags |= GF_IMPORT_SBR_EXPLICIT;
		else if (!stricmp(arg, "-ps")) import_flags |= GF_IMPORT_PS_IMPLICIT;
		else if (!stricmp(arg, "-psx")) import_flags |= GF_IMPORT_PS_EXPLICIT;
		else if (!stricmp(arg, "-ovsbr")) import_flags |= GF_IMPORT_OVSBR;
		else if (!stricmp(arg, "-fps")) {
			CHECK_NEXT_ARG
			if (!strcmp(argv[i + 1], "auto")) { fprintf(stderr, "Warning, fps=auto option is deprecated\n"); }
			else if (strchr(argv[i + 1], '-')) {
				u32 ticks, dts_inc;
				sscanf(argv[i + 1], "%u-%u", &ticks, &dts_inc);
				if (!dts_inc) dts_inc = 1;
				import_fps.num = ticks;
				import_fps.den = dts_inc;
			} else {
				import_fps.num = (s32) (1000 * atof(argv[i + 1]));
				import_fps.den = 1000;
			}
			i++;
		}
		else if (!stricmp(arg, "-agg")) {
			CHECK_NEXT_ARG agg_samples = atoi(argv[i + 1]);
			i++;
		}
#endif /*!defined(GPAC_DISABLE_MEDIA_EXPORT) && !defined(GPAC_DISABLE_MEDIA_IMPORT*/
		else if (!stricmp(arg, "-keep-sys") || !stricmp(arg, "-keepsys")) keep_sys_tracks = 1;
		else if (!stricmp(arg, "-ms")) {
			CHECK_NEXT_ARG mediaSource = argv[i + 1];
			i++;
		}
		else if (!stricmp(arg, "-mp4")) {
			encode = GF_TRUE;
			open_edit = GF_TRUE;
		}
		else if (!stricmp(arg, "-saf")) {
			do_saf = GF_TRUE;
		}
		else if (!stricmp(arg, "-sclog")) {
			do_scene_log = GF_TRUE;
		}
#ifndef GPAC_DISABLE_MPD
		else if (!stricmp(arg, "-mpd")) {
			do_mpd = GF_TRUE;
			CHECK_NEXT_ARG
			inName = argv[i + 1];
			i++;
		}
#endif

#ifndef GPAC_DISABLE_SCENE_ENCODER
		else if (!stricmp(arg, "-def")) smenc_opts.flags |= GF_SM_ENCODE_USE_NAMES;
		else if (!stricmp(arg, "-sync")) {
			CHECK_NEXT_ARG
			smenc_opts.flags |= GF_SM_ENCODE_RAP_INBAND;
			smenc_opts.rap_freq = atoi(argv[i + 1]);
			i++;
		}
		else if (!stricmp(arg, "-shadow")) {
			CHECK_NEXT_ARG
			smenc_opts.flags &= ~GF_SM_ENCODE_RAP_INBAND;
			smenc_opts.flags |= GF_SM_ENCODE_RAP_SHADOW;
			smenc_opts.rap_freq = atoi(argv[i + 1]);
			i++;
		}
		else if (!stricmp(arg, "-carousel")) {
			CHECK_NEXT_ARG
			smenc_opts.flags &= ~(GF_SM_ENCODE_RAP_INBAND | GF_SM_ENCODE_RAP_SHADOW);
			smenc_opts.rap_freq = atoi(argv[i + 1]);
			i++;
		}
		/*LASeR options*/
		else if (!stricmp(arg, "-resolution")) {
			CHECK_NEXT_ARG
			smenc_opts.resolution = atoi(argv[i + 1]);
			i++;
		}
#ifndef GPAC_DISABLE_SCENE_STATS
		else if (!stricmp(arg, "-auto-quant")) {
			CHECK_NEXT_ARG
			smenc_opts.resolution = atoi(argv[i + 1]);
			smenc_opts.auto_quant = 1;
			i++;
		}
#endif
		else if (!stricmp(arg, "-coord-bits")) {
			CHECK_NEXT_ARG
			smenc_opts.coord_bits = atoi(argv[i + 1]);
			i++;
		}
		else if (!stricmp(arg, "-scale-bits")) {
			CHECK_NEXT_ARG
			smenc_opts.scale_bits = atoi(argv[i + 1]);
			i++;
		}
		else if (!stricmp(arg, "-global-quant")) {
			CHECK_NEXT_ARG
			smenc_opts.resolution = atoi(argv[i + 1]);
			smenc_opts.auto_quant = 2;
			i++;
		}
		/*chunk encoding*/
		else if (!stricmp(arg, "-ctx-out") || !stricmp(arg, "-outctx")) {
			CHECK_NEXT_ARG
			output_ctx = argv[i + 1];
			i++;
		}
		else if (!stricmp(arg, "-ctx-in") || !stricmp(arg, "-inctx")) {
			CHECK_NEXT_ARG
			chunk_mode = GF_TRUE;
			input_ctx = argv[i + 1];
			i++;
		}
#endif /*GPAC_DISABLE_SCENE_ENCODER*/

#ifndef GPAC_DISABLE_ISOM_WRITE
		else if (!strcmp(arg, "-crypt")) {
			CHECK_NEXT_ARG
			crypt = 1;
			drm_file = argv[i + 1];
			open_edit = GF_TRUE;
			i += 1;
		}
		else if (!strcmp(arg, "-decrypt")) {
			CHECK_NEXT_ARG
			crypt = 2;
			if (get_file_type_by_ext(argv[i + 1]) != 1) {
				drm_file = argv[i + 1];
				i += 1;
			}
			open_edit = GF_TRUE;
		}
		else if (!stricmp(arg, "-set-kms")) {
			char szTK[20], *ext;
			CHECK_NEXT_ARG
			tracks = gf_realloc(tracks, sizeof(TrackAction) * (nb_track_act + 1));
			memset(&tracks[nb_track_act], 0, sizeof(TrackAction));

			strncpy(szTK, argv[i + 1], 19);
			ext = strchr(szTK, '=');
			tracks[nb_track_act].act_type = TRAC_ACTION_SET_KMS_URI;
			tracks[nb_track_act].trackID = 0;
			if (!strnicmp(argv[i + 1], "all=", 4)) {
				tracks[nb_track_act].kms = argv[i + 1] + 4;
			}
			else if (!ext) {
				tracks[nb_track_act].kms = argv[i + 1];
			}
			else {
				tracks[nb_track_act].kms = ext + 1;
				ext[0] = 0;
				tracks[nb_track_act].trackID = atoi(szTK);
				ext[0] = '=';
			}
			open_edit = GF_TRUE;
			nb_track_act++;
			i++;
		}
		else if (!stricmp(arg, "-split")) {
			CHECK_NEXT_ARG
			split_duration = atof(argv[i + 1]);
			if (split_duration < 0) split_duration = 0;
			i++;
			split_size = 0;
		}
		else if (!stricmp(arg, "-split-rap") || !stricmp(arg, "-splitr")) {
			CHECK_NEXT_ARG
			split_duration = -1;
			split_size = -1;
		}
		else if (!stricmp(arg, "-split-size") || !stricmp(arg, "-splits")) {
			CHECK_NEXT_ARG
			split_size = (u32)atoi(argv[i + 1]);
			i++;
			split_duration = 0;
		}
		else if (!stricmp(arg, "-split-chunk") || !stricmp(arg, "-splitx") || !stricmp(arg, "-splitz")) {
			CHECK_NEXT_ARG
			if (!strstr(argv[i + 1], ":")) {
				fprintf(stderr, "Chunk extraction usage: \"-splitx start:end\" expressed in seconds\n");
				return 2;
			}
			if (strstr(argv[i + 1], "end")) {
				if (strstr(argv[i + 1], "end-")) {
					Double dur_end=0;
					sscanf(argv[i + 1], "%lf:end-%lf", &split_start, &dur_end);
					split_duration = -2 - dur_end;
				} else {
					sscanf(argv[i + 1], "%lf:end", &split_start);
					split_duration = -2;
				}
			}
			else {
				if (strchr(argv[i + 1], '-')) {
					split_range_str = argv[i + 1];
				} else {
					sscanf(argv[i + 1], "%lf:%lf", &split_start, &split_duration);
					split_duration -= split_start;
				}
			}
			split_size = 0;
			if (!stricmp(arg, "-splitz")) adjust_split_end = 1;
			i++;
		}
		/*meta*/
		else if (!stricmp(arg, "-set-meta")) {
			metas = gf_realloc(metas, sizeof(MetaAction) * (nb_meta_act + 1));
			parse_meta_args(&metas[nb_meta_act], META_ACTION_SET_TYPE, argv[i + 1]);
			nb_meta_act++;
			open_edit = GF_TRUE;
			i++;
		}
		else if (!stricmp(arg, "-add-item")) {
			metas = gf_realloc(metas, sizeof(MetaAction) * (nb_meta_act + 1));
			parse_meta_args(&metas[nb_meta_act], META_ACTION_ADD_ITEM, argv[i + 1]);
			nb_meta_act++;
			open_edit = GF_TRUE;
			i++;
		}
		else if (!stricmp(arg, "-add-image")) {
			metas = gf_realloc(metas, sizeof(MetaAction) * (nb_meta_act + 1));
			parse_meta_args(&metas[nb_meta_act], META_ACTION_ADD_IMAGE_ITEM, argv[i + 1]);
			nb_meta_act++;
			open_edit = GF_TRUE;
			i++;
		}
		else if (!stricmp(arg, "-rem-item")) {
			metas = gf_realloc(metas, sizeof(MetaAction) * (nb_meta_act + 1));
			parse_meta_args(&metas[nb_meta_act], META_ACTION_REM_ITEM, argv[i + 1]);
			nb_meta_act++;
			open_edit = GF_TRUE;
			i++;
		}
		else if (!stricmp(arg, "-set-primary")) {
			metas = gf_realloc(metas, sizeof(MetaAction) * (nb_meta_act + 1));
			parse_meta_args(&metas[nb_meta_act], META_ACTION_SET_PRIMARY_ITEM, argv[i + 1]);
			nb_meta_act++;
			open_edit = GF_TRUE;
			i++;
		}
		else if (!stricmp(arg, "-set-xml")) {
			metas = gf_realloc(metas, sizeof(MetaAction) * (nb_meta_act + 1));
			parse_meta_args(&metas[nb_meta_act], META_ACTION_SET_XML, argv[i + 1]);
			nb_meta_act++;
			open_edit = GF_TRUE;
			i++;
		}
		else if (!stricmp(arg, "-rem-xml")) {
			metas = gf_realloc(metas, sizeof(MetaAction) * (nb_meta_act + 1));
			if (parse_meta_args(&metas[nb_meta_act], META_ACTION_REM_XML, argv[i + 1])) i++;
			nb_meta_act++;
			open_edit = GF_TRUE;
		}
		else if (!stricmp(arg, "-dump-xml")) {
			metas = gf_realloc(metas, sizeof(MetaAction) * (nb_meta_act + 1));
			parse_meta_args(&metas[nb_meta_act], META_ACTION_DUMP_XML, argv[i + 1]);
			nb_meta_act++;
			i++;
		}
		else if (!stricmp(arg, "-dump-item")) {
			metas = gf_realloc(metas, sizeof(MetaAction) * (nb_meta_act + 1));
			parse_meta_args(&metas[nb_meta_act], META_ACTION_DUMP_ITEM, argv[i + 1]);
			nb_meta_act++;
			i++;
		}
		else if (!stricmp(arg, "-group-add") || !stricmp(arg, "-group-rem-track") || !stricmp(arg, "-group-rem") ) {
			TSELActionType act_type;
			if (!stricmp(arg, "-group-rem")) {
				act_type = TSEL_ACTION_REMOVE_ALL_TSEL_IN_GROUP;
			}
			else if (!stricmp(arg, "-group-rem-track")) {
				act_type = TSEL_ACTION_REMOVE_TSEL;
			}
			else {
				act_type = TSEL_ACTION_SET_PARAM;
			}
			if (parse_tsel_args(&tsel_acts, argv[i + 1], &nb_tsel_acts, act_type) == 0) {
				fprintf(stderr, "Invalid group syntax - check usage\n");
				return 2;
			}
			open_edit = GF_TRUE;
			i++;
		}
		else if (!stricmp(arg, "-group-clean")) {
			clean_groups = 1;
			open_edit = GF_TRUE;
		}
		else if (!stricmp(arg, "-group-single")) {
			single_group = 1;
		}
		else if (!stricmp(arg, "-package")) {
			CHECK_NEXT_ARG
			pack_file = argv[i + 1];
			i++;
		}
		else if (!stricmp(arg, "-zmov")) {
			compress_moov = GF_TRUE;
		}
		else if (!stricmp(arg, "-mgt")) {
			CHECK_NEXT_ARG
			pack_file = argv[i + 1];
			pack_wgt = GF_TRUE;
			i++;
		}
		else if (!stricmp(arg, "-brand")) {
			char *b = argv[i + 1];
			CHECK_NEXT_ARG
			major_brand = GF_4CC(b[0], b[1], b[2], b[3]);
			open_edit = GF_TRUE;
			if (b[4] == ':') {
				if (!strncmp(b+5, "0x", 2))
					sscanf(b+5, "0x%x", &minor_version);
				else
					minor_version = atoi(b + 5);
			}
			i++;
		}
		else if (!stricmp(arg, "-ab")) {
			char *b = argv[i + 1];
			CHECK_NEXT_ARG
			brand_add = (u32*)gf_realloc(brand_add, sizeof(u32) * (nb_alt_brand_add + 1));
			brand_add[nb_alt_brand_add] = GF_4CC(b[0], b[1], b[2], b[3]);
			nb_alt_brand_add++;
			open_edit = GF_TRUE;
			i++;
		}
		else if (!stricmp(arg, "-rb")) {
			char *b = argv[i + 1];
			CHECK_NEXT_ARG
			brand_rem = (u32*)gf_realloc(brand_rem, sizeof(u32) * (nb_alt_brand_rem + 1));
			brand_rem[nb_alt_brand_rem] = GF_4CC(b[0], b[1], b[2], b[3]);
			nb_alt_brand_rem++;
			open_edit = GF_TRUE;
			i++;
		}
#endif
		else if (!stricmp(arg, "-languages")) {
			PrintLanguages();
			return 1;
		}
		else if (!stricmp(arg, "-h")) {
			gf_sys_set_args(argc, (const char**) argv);

			if (i + 1 == (u32)argc) PrintUsage();
			else if (!strcmp(argv[i + 1], "general")) PrintGeneralUsage();
			else if (!strcmp(argv[i + 1], "extract")) PrintExtractUsage();
			else if (!strcmp(argv[i + 1], "dash")) PrintDASHUsage();
			else if (!strcmp(argv[i + 1], "dump")) PrintDumpUsage();
			else if (!strcmp(argv[i + 1], "import")) PrintImportUsage();
			else if (!strcmp(argv[i + 1], "format")) fprintf(stderr, "deprectaed, see [filters documentation](Filters)\n");
			else if (!strcmp(argv[i + 1], "hint")) PrintHintUsage();
			else if (!strcmp(argv[i + 1], "encode")) PrintEncodeUsage();
			else if (!strcmp(argv[i + 1], "crypt")) PrintEncryptUsage();
			else if (!strcmp(argv[i + 1], "meta")) PrintMetaUsage();
			else if (!strcmp(argv[i + 1], "swf")) PrintSWFUsage();
#if !defined(GPAC_DISABLE_STREAMING) && !defined(GPAC_DISABLE_SENG)
			else if (!strcmp(argv[i + 1], "rtp")) fprintf(stderr, "RTP streaming deprecated in MP4Box, use gpac application\n");
			else if (!strcmp(argv[i + 1], "live")) PrintLiveUsage();
#endif
			else if (!strcmp(argv[i + 1], "core")) PrintCoreUsage();
			else if (!strcmp(argv[i + 1], "all")) {
				PrintGeneralUsage();
				PrintExtractUsage();
				PrintDASHUsage();
				PrintDumpUsage();
				PrintImportUsage();
				PrintHintUsage();
				PrintEncodeUsage();
				PrintEncryptUsage();
				PrintMetaUsage();
				PrintSWFUsage();
#if !defined(GPAC_DISABLE_STREAMING) && !defined(GPAC_DISABLE_SENG)
				PrintLiveUsage();
#endif
				PrintCoreUsage();
			} else if (!strcmp(argv[i + 1], "opts")) {
				PrintHelp("@", GF_FALSE, GF_FALSE);
			} else {
				PrintHelp(argv[i+1], GF_FALSE, GF_FALSE);
			}
			return 1;
		}
		else if (!stricmp(arg, "-hx")) {
			if (i + 1 == (u32)argc) PrintUsage();
			else PrintHelp(argv[i+1], GF_TRUE, GF_FALSE);
			return 1;
		}
		else if (!strcmp(arg, "-genmd")) {
			help_flags = GF_PRINTARG_MD | GF_PRINTARG_IS_APP;
			helpout = gf_fopen("mp4box-gen-opts.md", "w");

	 		fprintf(helpout, "[**HOME**](Home) » [**MP4Box**](MP4Box) » General");
	 		fprintf(helpout, "<!-- automatically generated - do not edit, patch gpac/applications/mp4box/main.c -->\n");
			fprintf(helpout, "# Syntax\n");
			gf_sys_format_help(helpout, help_flags, "MP4Box [option] input [option] [other_dash_inputs]\n"
				"  \n"
			);
			PrintGeneralUsage();
			PrintEncryptUsage();
			fprintf(helpout, "# Help Options\n");
			while (m4b_usage_args[i].name) {
				GF_GPACArg *g_arg = &m4b_usage_args[i];
				i++;
				gf_sys_print_arg(helpout, help_flags, g_arg, "mp4box-general");
			}

			gf_fclose(helpout);

			helpout = gf_fopen("mp4box-import-opts.md", "w");
	 		fprintf(helpout, "[**HOME**](Home) » [**MP4Box**](MP4Box) » Media Import");
	 		fprintf(helpout, "<!-- automatically generated - do not edit, patch gpac/applications/mp4box/main.c -->\n");
			PrintImportUsage();
			gf_fclose(helpout);

			helpout = gf_fopen("mp4box-dash-opts.md", "w");
	 		fprintf(helpout, "[**HOME**](Home) » [**MP4Box**](MP4Box) » Media DASH");
	 		fprintf(helpout, "<!-- automatically generated - do not edit, patch gpac/applications/mp4box/main.c -->\n");
			PrintDASHUsage();
			gf_fclose(helpout);

			helpout = gf_fopen("mp4box-dump-opts.md", "w");
	 		fprintf(helpout, "[**HOME**](Home) » [**MP4Box**](MP4Box) » Media Dump and Export");
	 		fprintf(helpout, "<!-- automatically generated - do not edit, patch gpac/applications/mp4box/main.c -->\n");
			PrintExtractUsage();
			PrintDumpUsage();
			gf_fclose(helpout);

			helpout = gf_fopen("mp4box-meta-opts.md", "w");
	 		fprintf(helpout, "[**HOME**](Home) » [**MP4Box**](MP4Box) » Meta and HEIF/IFF");
	 		fprintf(helpout, "<!-- automatically generated - do not edit, patch gpac/applications/mp4box/main.c -->\n");
			PrintMetaUsage();
			gf_fclose(helpout);


			helpout = gf_fopen("mp4box-scene-opts.md", "w");
	 		fprintf(helpout, "[**HOME**](Home) » [**MP4Box**](MP4Box) » Scene Description");
	 		fprintf(helpout, "<!-- automatically generated - do not edit, patch gpac/applications/mp4box/main.c -->\n");
			PrintEncodeUsage();
#if !defined(GPAC_DISABLE_STREAMING) && !defined(GPAC_DISABLE_SENG)
			PrintLiveUsage();
#endif
			PrintSWFUsage();
			gf_fclose(helpout);

			helpout = gf_fopen("mp4box-other-opts.md", "w");
	 		fprintf(helpout, "[**HOME**](Home) » [**MP4Box**](MP4Box) » Other Features");
	 		fprintf(helpout, "<!-- automatically generated - do not edit, patch gpac/applications/mp4box/main.c -->\n");
			PrintHintUsage();
			gf_fclose(helpout);

			gf_sys_close();
			return 1;
		} else if (!strcmp(arg, "-genman")) {
			help_flags = GF_PRINTARG_MAN;
			helpout = gf_fopen("mp4box.1", "w");


	 		fprintf(helpout, ".TH MP4Box 1 2019 MP4Box GPAC\n");
			fprintf(helpout, ".\n.SH NAME\n.LP\nMP4Box \\- GPAC command-line media packager\n.SH SYNOPSIS\n.LP\n.B MP4Box\n.RI [options] \\ [file] \\ [options]\n.br\n.\n");

			PrintGeneralUsage();
			PrintExtractUsage();
			PrintDASHUsage();
			PrintDumpUsage();
			PrintImportUsage();
			PrintHintUsage();
			PrintEncodeUsage();
			PrintEncryptUsage();
			PrintMetaUsage();
			PrintSWFUsage();
#if !defined(GPAC_DISABLE_STREAMING) && !defined(GPAC_DISABLE_SENG)
			PrintLiveUsage();
#endif

			fprintf(helpout, ".SH EXAMPLES\n.TP\nBasic and advanced examples are available at https://wiki.gpac.io/MP4Box-Introduction\n");
			fprintf(helpout, ".SH MORE\n.LP\nAuthors: GPAC developers, see git repo history (-log)\n"
			".br\nFor bug reports, feature requests, more information and source code, visit http://github.com/gpac/gpac\n"
			".br\nbuild: %s\n"
			".br\nCopyright: %s\n.br\n"
			".SH SEE ALSO\n"
			".LP\ngpac(1), MP4Client(1)\n", gf_gpac_version(), gf_gpac_copyright());

			gf_fclose(helpout);
			gf_sys_close();
			return 1;
		}

		else if (!stricmp(arg, "-v")) verbose++;
		else if (!stricmp(arg, "-tag-list")) {
			fprintf(stderr, "Supported iTunes tag modifiers:\n");
			for (i = 0; i < nb_itunes_tags; i++) {
				fprintf(stderr, "\t%s\t%s\n", itags[i].name, itags[i].comment);
			}
			return 1;
		}
		else if (!live_scene) {
			u32 res = gf_sys_is_gpac_arg(arg);
			if (res==0) {
				PrintHelp(arg, GF_FALSE, GF_TRUE);
				return 2;
			} else if (res==2) {
				i++;
			}
		}
	}
	*current_index = i;
	return 0;
}