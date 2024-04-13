}

GF_Err rip_mpd(const char *mpd_src, const char *output_dir)
{
	GF_DownloadSession *sess;
	u32 i, connect_time, reply_time, download_time, req_hdr_size, rsp_hdr_size;
	GF_Err e;
	GF_DOMParser *mpd_parser=NULL;
	GF_MPD *mpd=NULL;
	GF_MPD_Period *period;
	GF_MPD_AdaptationSet *as;
	GF_MPD_Representation *rep;
	char szName[GF_MAX_PATH];
	GF_DownloadManager *dm;

	if (output_dir) {
		char *sep;
		strcpy(szName, output_dir);
		sep = gf_file_basename(szName);
		if (sep) sep[0] = 0;
		gf_opts_set_key("temp", "cache", szName);
	} else {
		gf_opts_set_key("temp", "cache", ".");
	}
	gf_opts_set_key("temp", "clean-cache", "true");
	dm = gf_dm_new(NULL);

/*
	char *name = strrchr(mpd_src, '/');
	if (!name) name = strrchr(mpd_src, '\\');
	if (!name) name = "manifest.mpd";
	else name ++;

	if (strchr(name, '?') || strchr(name, '&')) name = "manifest.mpd";
*/

	fprintf(stderr, "Downloading %s\n", mpd_src);
	sess = get_file(mpd_src, dm, &e);
	if (!sess) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_APP, ("Error downloading MPD file %s: %s\n", mpd_src, gf_error_to_string(e) ));
		goto err_exit;
	}
	strcpy(szName, gf_dm_sess_get_cache_name(sess) );
	gf_dm_sess_get_header_sizes_and_times(sess, &req_hdr_size, &rsp_hdr_size, &connect_time, &reply_time, &download_time);
	gf_dm_sess_del(sess);

	if (e) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_APP, ("Error fetching MPD file %s: %s\n", mpd_src, gf_error_to_string(e)));
		goto err_exit;
	}
	else {
		GF_LOG(GF_LOG_INFO, GF_LOG_APP, ("Fetched file %s\n", mpd_src));
	}
	GF_LOG(GF_LOG_DEBUG, GF_LOG_APP, ("GET Header size %d - Reply header size %d\n", req_hdr_size, rsp_hdr_size));
	GF_LOG(GF_LOG_DEBUG, GF_LOG_APP, ("GET time: Connect Time %d - Reply Time %d - Download Time %d\n", connect_time, reply_time, download_time));

	mpd_parser = gf_xml_dom_new();
	e = gf_xml_dom_parse(mpd_parser, szName, NULL, NULL);

	if (e != GF_OK) {
		gf_xml_dom_del(mpd_parser);
		GF_LOG(GF_LOG_ERROR, GF_LOG_APP, ("Error parsing MPD %s : %s\n", mpd_src, gf_error_to_string(e)));
		return e;
	}
	mpd = gf_mpd_new();
	e = gf_mpd_init_from_dom(gf_xml_dom_get_root(mpd_parser), mpd, mpd_src);
	gf_xml_dom_del(mpd_parser);
	mpd_parser=NULL;
	if (e) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_APP, ("Error initializing MPD %s : %s\n", mpd_src, gf_error_to_string(e)));
		goto err_exit;
	}
	else {
		GF_LOG(GF_LOG_DEBUG, GF_LOG_APP, ("MPD %s initialized: %s\n", szName, gf_error_to_string(e)));
	}

	revert_cache_file(szName);
	if (mpd->type==GF_MPD_TYPE_DYNAMIC) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_APP, ("MPD rip is not supported on live sources\n"));
		e = GF_NOT_SUPPORTED;
		goto err_exit;
	}

	i=0;
	while ((period = (GF_MPD_Period *) gf_list_enum(mpd->periods, &i))) {
		char *initTemplate = NULL;
		Bool segment_base = GF_FALSE;
		u32 j=0;

		if (period->segment_base) segment_base=GF_TRUE;

		if (period->segment_template && period->segment_template->initialization) {
			initTemplate = period->segment_template->initialization;
		}

		while ((as = gf_list_enum(period->adaptation_sets, &j))) {
			u32 k=0;
			if (!initTemplate && as->segment_template && as->segment_template->initialization) {
				initTemplate = as->segment_template->initialization;
			}
			if (as->segment_base) segment_base=GF_TRUE;

			while ((rep = gf_list_enum(as->representations, &k))) {
				u64 out_range_start, out_range_end, segment_duration;
				Bool is_in_base_url;
				char *seg_url;
				u32 seg_idx=0;
				if (rep->segment_template && rep->segment_template->initialization) {
					initTemplate = rep->segment_template->initialization;
				} else if (k>1) {
					initTemplate = NULL;
				}
				if (rep->segment_base) segment_base=GF_TRUE;

				e = gf_mpd_resolve_url(mpd, rep, as, period, mpd_src, 0, GF_MPD_RESOLVE_URL_INIT, 0, 0, &seg_url, &out_range_start, &out_range_end, &segment_duration, &is_in_base_url, NULL, NULL, NULL);
				if (e) {
					GF_LOG(GF_LOG_ERROR, GF_LOG_APP, ("Error resolving init segment name : %s\n", gf_error_to_string(e)));
					continue;
				}
				//not a byte range, replace URL
				if (segment_base) {

				} else if (out_range_start || out_range_end || !seg_url) {
					GF_LOG(GF_LOG_ERROR, GF_LOG_APP, ("byte range rip not yet implemented\n"));
					if (seg_url) gf_free(seg_url);
					e = GF_NOT_SUPPORTED;
					goto err_exit;
				}

				fprintf(stderr, "Downloading %s\n", seg_url);
				sess = get_file(seg_url, dm, &e);
				if (e) {
					GF_LOG(GF_LOG_ERROR, GF_LOG_APP, ("Error downloading init segment %s from MPD %s : %s\n", seg_url, mpd_src, gf_error_to_string(e)));
					goto err_exit;
				}
				revert_cache_file((char *) gf_dm_sess_get_cache_name(sess) );
				gf_free(seg_url);
				gf_dm_sess_del(sess);

				if (segment_base) continue;

				while (1) {
					e = gf_mpd_resolve_url(mpd, rep, as, period, mpd_src, 0, GF_MPD_RESOLVE_URL_MEDIA, seg_idx, 0, &seg_url, &out_range_start, &out_range_end, &segment_duration, NULL, NULL, NULL, NULL);
					if (e) {
						if (e<0) {
							GF_LOG(GF_LOG_ERROR, GF_LOG_APP, ("Error resolving segment name : %s\n", gf_error_to_string(e)));
						}
						break;
					}

					seg_idx++;

					if (out_range_start || out_range_end || !seg_url) {
						GF_LOG(GF_LOG_ERROR, GF_LOG_APP, ("byte range rip not yet implemented\n"));
						if (seg_url) gf_free(seg_url);
						break;
					}
					fprintf(stderr, "Downloading %s\n", seg_url);
					sess = get_file(seg_url, dm, &e);
					if (e) {
						gf_free(seg_url);
						if (e != GF_URL_ERROR) {
							GF_LOG(GF_LOG_ERROR, GF_LOG_APP, ("Error downloading segment %s: %s\n", seg_url, gf_error_to_string(e)));
						} else {
							//todo, properly detect end of dash representation
							e = GF_OK;
						}
						break;
					}
					revert_cache_file((char *) gf_dm_sess_get_cache_name(sess) );
					gf_free(seg_url);
					gf_dm_sess_del(sess);
				}
			}
		}
	}

err_exit:
	if (mpd) gf_mpd_del(mpd);
	gf_dm_del(dm);