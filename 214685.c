}

static void revert_cache_file(char *item_path)
{
	char szPATH[GF_MAX_PATH];
	const char *url;
	GF_Config *cached;

	if (!strstr(item_path, "gpac_cache_")) {
		fprintf(stderr, "%s is not a gpac cache file\n", item_path);
		return;
	}
	if (!strncmp(item_path, "./", 2) || !strncmp(item_path, ".\\", 2))
			item_path += 2;

 	strcpy(szPATH, item_path);
	strcat(szPATH, ".txt");

	cached = gf_cfg_new(NULL, szPATH);
	url = gf_cfg_get_key(cached, "cache", "url");
	if (url) url = strstr(url, "://");
	if (url) {
		u32 i, len, dir_len=0, k=0;
		char *sep;
		char *dst_name;
		sep = strstr(item_path, "gpac_cache_");
		if (sep) {
			sep[0] = 0;
			dir_len = (u32) strlen(item_path);
			sep[0] = 'g';
		}
		url+=3;
		len = (u32) strlen(url);
		dst_name = gf_malloc(len+dir_len+1);
		memset(dst_name, 0, len+dir_len+1);

		strncpy(dst_name, item_path, dir_len);
		k=dir_len;
		for (i=0; i<len; i++) {
			dst_name[k] = url[i];
			if (dst_name[k]==':') dst_name[k]='_';
			else if (dst_name[k]=='/') {
				if (!gf_dir_exists(dst_name))
					gf_mkdir(dst_name);
			}
			k++;
		}
		if (gf_file_exists(item_path)) {
			gf_file_move(item_path, dst_name);
		}

		gf_free(dst_name);
	} else {
		fprintf(stderr, "Failed to reverse %s cache file\n", item_path);
	}
	gf_cfg_del(cached);