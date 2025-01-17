void dump_isom_scene_stats(char *file, char *inName, Bool is_final_name, u32 stat_level)
{
	GF_Err e;
	FILE *dump;
	Bool close;
	u32 i, j, count;
	char szBuf[1024];
	GF_SceneManager *ctx;
	GF_SceneLoader load;
	GF_StatManager *sm;
	GF_List *sample_list;
	GF_SceneGraph *scene_graph;

	dump = NULL;
	sm = NULL;
	sample_list = NULL;

	close = 0;

	scene_graph = gf_sg_new();
	ctx = gf_sm_new(scene_graph);
	memset(&load, 0, sizeof(GF_SceneLoader));
	load.fileName = file;
	load.ctx = ctx;

	if (get_file_type_by_ext(file) == 1) {
		load.isom = gf_isom_open(file, GF_ISOM_OPEN_READ, NULL);
		if (!load.isom) {
			M4_LOG(GF_LOG_ERROR, ("Cannot open file: %s\n", gf_error_to_string(gf_isom_last_error(NULL))));
			gf_sm_del(ctx);
			gf_sg_del(scene_graph);
			return;
		}
	}

	e = gf_sm_load_init(&load);
	if (!e) e = gf_sm_load_run(&load);
	gf_sm_load_done(&load);
	if (e<0) goto exit;

	if (inName) {
		strcpy(szBuf, inName);
		if (!is_final_name) strcat(szBuf, "_stat.xml");
		dump = gf_fopen(szBuf, "wt");
		if (!dump) {
			M4_LOG(GF_LOG_ERROR, ("Failed to open %s for dumping\n", szBuf));
			return;
		}
		close = 1;
	} else {
		dump = stdout;
		close = 0;
	}

	fprintf(stderr, "Analysing Scene\n");

	fprintf(dump, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
	fprintf(dump, "<!-- Scene Graph Statistics Generated by MP4Box - GPAC ");
	if (! gf_sys_is_test_mode())
		fprintf(dump, "%s ",  gf_gpac_version());
	fprintf(dump, "-->\n");

	fprintf(dump, "<SceneStatistics file=\"%s\" DumpType=\"%s\">\n", gf_file_basename(file), (stat_level==1) ? "full scene" : ((stat_level==2) ? "AccessUnit based" : "SceneGraph after each AU"));

	sm = gf_sm_stats_new();

	/*stat level 1: complete scene stat*/
	if (stat_level == 1) {
		e = gf_sm_stats_for_scene(sm, ctx);
		if (!e) dump_stats(dump, gf_sm_stats_get(sm) );
		goto exit;
	}
	/*re_order all BIFS-AUs*/
	sample_list = gf_list_new();
	/*configure all systems streams we're dumping*/
	for (i=0; i<gf_list_count(ctx->streams); i++) {
		GF_StreamContext *sc = gf_list_get(ctx->streams, i);
		if (sc->streamType != GF_STREAM_SCENE) continue;
		for (j=0; j<gf_list_count(sc->AUs); j++) {
			GF_AUContext *au = gf_list_get(sc->AUs, j);
			ReorderAU(sample_list, au);
		}
	}

	count = gf_list_count(sample_list);
	for (i=0; i<count; i++) {
		GF_AUContext *au = gf_list_get(sample_list, i);

		for (j=0; j<gf_list_count(au->commands); j++) {
			GF_Command *com = gf_list_get(au->commands, j);
			/*stat level 2 - get command stats*/
			if (stat_level==2) {
				e = gf_sm_stats_for_command(sm, com);
				if (e) goto exit;
			}
			/*stat level 3 - apply command*/
			if (stat_level==3) gf_sg_command_apply(scene_graph, com, 0);
		}
		/*stat level 3: get graph stat*/
		if (stat_level==3) {
			e = gf_sm_stats_for_graph(sm, scene_graph);
			if (e) goto exit;
		}
		if (stat_level==2) {
			fprintf(dump, "<AUStatistics StreamID=\"%d\" AUTime=\""LLD"\">\n", au->owner->ESID, au->timing);
		} else {
			fprintf(dump, "<GraphStatistics StreamID=\"%d\" AUTime=\""LLD"\">\n", au->owner->ESID, au->timing);
		}
		/*dump stats*/
		dump_stats(dump, gf_sm_stats_get(sm) );
		/*reset stats*/
		gf_sm_stats_reset(sm);
		if (stat_level==2) {
			fprintf(dump, "</AUStatistics>\n");
		} else {
			fprintf(dump, "</GraphStatistics>\n");
		}

		gf_set_progress("Analysing AU", i+1, count);
	}


exit:
	if (sample_list) gf_list_del(sample_list);
	if (sm) gf_sm_stats_del(sm);
	gf_sm_del(ctx);
	gf_sg_del(scene_graph);
	if (load.isom) gf_isom_delete(load.isom);
	if (e) {
		M4_LOG(GF_LOG_ERROR, ("Stats error: %s\n", gf_error_to_string(e)));
	} else {
		fprintf(dump, "</SceneStatistics>\n");
	}
	if (dump && close) gf_fclose(dump);
	fprintf(stderr, "done\n");
}