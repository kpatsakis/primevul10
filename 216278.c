void init_pdb_downloader(SPDBDownloaderOpt *opt, SPDBDownloader *pd) {
	pd->opt = R_NEW0 (SPDBDownloaderOpt);
	if (!pd->opt) {
		pd->download = 0;
		eprintf ("Cannot allocate memory for SPDBDownloaderOpt.\n");
		return;
	}
	pd->opt->dbg_file = strdup (opt->dbg_file);
	pd->opt->guid = strdup (opt->guid);
	pd->opt->symbol_server = strdup (opt->symbol_server);
	pd->opt->user_agent = strdup (opt->user_agent);
	pd->opt->symbol_store_path = strdup (opt->symbol_store_path);
	pd->opt->extract = opt->extract;
	pd->download = download;
}