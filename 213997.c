GF_DashSegmenterInput *set_dash_input(GF_DashSegmenterInput *dash_inputs, char *name, u32 *nb_dash_inputs)
{
	GF_DashSegmenterInput *di;
	char *other_opts = NULL;
	char *sep = gf_url_colon_suffix(name);

	dash_inputs = gf_realloc(dash_inputs, sizeof(GF_DashSegmenterInput) * (*nb_dash_inputs + 1) );
	memset(&dash_inputs[*nb_dash_inputs], 0, sizeof(GF_DashSegmenterInput) );
	di = &dash_inputs[*nb_dash_inputs];
	(*nb_dash_inputs)++;

	if (sep) {
		char *opts, *first_opt;
		opts = first_opt = sep;
		while (opts) {
			sep = gf_url_colon_suffix(opts);
			while (sep) {
				/* this is a real separator if it is followed by a keyword we are looking for */
				if (!strnicmp(sep, ":id=", 4) ||
				        !strnicmp(sep, ":dur=", 5) ||
				        !strnicmp(sep, ":period=", 8) ||
				        !strnicmp(sep, ":BaseURL=", 9) ||
				        !strnicmp(sep, ":bandwidth=", 11) ||
				        !strnicmp(sep, ":role=", 6) ||
				        !strnicmp(sep, ":desc", 5) ||
				        !strnicmp(sep, ":sscale", 7) ||
				        !strnicmp(sep, ":duration=", 10) ||
				        !strnicmp(sep, ":period_duration=", 10) ||
				        !strnicmp(sep, ":pdur=", 6) ||
				        !strnicmp(sep, ":xlink=", 7) ||
				        !strnicmp(sep, ":asID=", 6) ||
				        !strnicmp(sep, ":sn=", 4) ||
				        !strnicmp(sep, ":tpl=", 5) ||
				        !strnicmp(sep, ":hls=", 5) ||
				        !strnicmp(sep, ":trackID=", 9) ||
				        !strnicmp(sep, ":@@", 3)
				        ) {
					break;
				} else {
					char *nsep = gf_url_colon_suffix(sep+1);
					if (nsep) nsep[0] = 0;

					gf_dynstrcat(&other_opts, sep, ":");

					if (nsep) nsep[0] = ':';

					sep = strchr(sep+1, ':');
				}
			}
			if (sep && !strncmp(sep, "://", 3) && strnicmp(sep, ":@@", 3)) sep = gf_url_colon_suffix(sep+3);
			if (sep) sep[0] = 0;

			if (!strnicmp(opts, "id=", 3)) {
				di->representationID = gf_strdup(opts+3);
				/*we allow the same repID to be set to force muxed representations*/
			}
			else if (!strnicmp(opts, "dur=", 4)) di->media_duration = (Double)atof(opts+4);
			else if (!strnicmp(opts, "period=", 7)) di->periodID = gf_strdup(opts+7);
			else if (!strnicmp(opts, "BaseURL=", 8)) {
				di->baseURL = (char **)gf_realloc(di->baseURL, (di->nb_baseURL+1)*sizeof(char *));
				di->baseURL[di->nb_baseURL] = gf_strdup(opts+8);
				di->nb_baseURL++;
			} else if (!strnicmp(opts, "bandwidth=", 10)) di->bandwidth = atoi(opts+10);
			else if (!strnicmp(opts, "role=", 5)) {
				di->roles = gf_realloc(di->roles, sizeof (char *) * (di->nb_roles+1));
				di->roles[di->nb_roles] = gf_strdup(opts+5);
				di->nb_roles++;
			} else if (!strnicmp(opts, "desc", 4)) {
				u32 *nb_descs=NULL;
				char ***descs=NULL;
				u32 opt_offset=0;
				u32 len;
				if (!strnicmp(opts, "desc_p=", 7)) {
					nb_descs = &di->nb_p_descs;
					descs = &di->p_descs;
					opt_offset = 7;
				} else if (!strnicmp(opts, "desc_as=", 8)) {
					nb_descs = &di->nb_as_descs;
					descs = &di->as_descs;
					opt_offset = 8;
				} else if (!strnicmp(opts, "desc_as_c=", 8)) {
					nb_descs = &di->nb_as_c_descs;
					descs = &di->as_c_descs;
					opt_offset = 10;
				} else if (!strnicmp(opts, "desc_rep=", 8)) {
					nb_descs = &di->nb_rep_descs;
					descs = &di->rep_descs;
					opt_offset = 9;
				}
				if (opt_offset) {
					(*nb_descs)++;
					opts += opt_offset;
					len = (u32) strlen(opts);
					(*descs) = (char **)gf_realloc((*descs), (*nb_descs)*sizeof(char *));
					(*descs)[(*nb_descs)-1] = (char *)gf_malloc((len+1)*sizeof(char));
					strncpy((*descs)[(*nb_descs)-1], opts, len);
					(*descs)[(*nb_descs)-1][len] = 0;
				}

			}
			else if (!strnicmp(opts, "xlink=", 6)) di->xlink = gf_strdup(opts+6);
			else if (!strnicmp(opts, "sscale", 6)) di->sscale = GF_TRUE;
			else if (!strnicmp(opts, "pdur=", 5)) di->period_duration = (Double) atof(opts+5);
			else if (!strnicmp(opts, "period_duration=", 16)) di->period_duration = (Double) atof(opts+16);
			else if (!strnicmp(opts, "duration=", 9)) di->dash_duration = (Double) atof(opts+9);
			else if (!strnicmp(opts, "asID=", 5)) di->asID = atoi(opts+5);
			else if (!strnicmp(opts, "sn=", 3)) di->startNumber = atoi(opts+3);
			else if (!strnicmp(opts, "tpl=", 4)) di->seg_template = gf_strdup(opts+4);
			else if (!strnicmp(opts, "hls=", 4)) di->hls_pl = gf_strdup(opts+4);
			else if (!strnicmp(opts, "trackID=", 8)) di->track_id = atoi(opts+8);
			else if (!strnicmp(opts, "@@", 2)) {
				di->filter_chain = gf_strdup(opts+2);
				if (sep) sep[0] = ':';
				sep = NULL;
			}

			if (!sep) break;
			sep[0] = ':';
			opts = sep+1;
		}
		first_opt[0] = '\0';
	}
	di->file_name = name;
	di->source_opts = other_opts;

	if (!di->representationID) {
		char szRep[100];
		sprintf(szRep, "%d", *nb_dash_inputs);
		di->representationID = gf_strdup(szRep);
	}

	return dash_inputs;
}