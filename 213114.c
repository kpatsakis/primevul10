R_API bool r_core_init(RCore *core) {
	core->blocksize = R_CORE_BLOCKSIZE;
	core->block = (ut8 *)calloc (R_CORE_BLOCKSIZE + 1, 1);
	if (!core->block) {
		eprintf ("Cannot allocate %d byte(s)\n", R_CORE_BLOCKSIZE);
		/* XXX memory leak */
		return false;
	}
	r_core_setenv (core);
	core->ev = r_event_new (core);
	r_event_hook (core->ev, R_EVENT_ALL, cb_event_handler, NULL);
	core->max_cmd_depth = R_CONS_CMD_DEPTH + 1;
	core->sdb = sdb_new (NULL, "r2kv.sdb", 0); // XXX: path must be in home?
	core->lastsearch = NULL;
	core->cmdfilter = NULL;
	core->switch_file_view = 0;
	core->cmdremote = 0;
	core->incomment = false;
	core->config = NULL;
	core->http_up = false;
	core->use_tree_sitter_r2cmd = false;
	ZERO_FILL (core->root_cmd_descriptor);
	core->print = r_print_new ();
	core->ropchain = r_list_newf ((RListFree)free);
	r_core_bind (core, &(core->print->coreb));
	core->print->user = core;
	core->print->num = core->num;
	core->print->offname = r_core_print_offname;
	core->print->offsize = r_core_print_offsize;
	core->print->cb_printf = r_cons_printf;
#if __WINDOWS__
	core->print->cb_eprintf = win_eprintf;
#endif
	core->print->cb_color = r_cons_rainbow_get;
	core->print->write = mywrite;
	core->print->exists_var = exists_var;
	core->print->disasm = __disasm;
	core->print->colorfor = colorfor_cb;
	core->print->hasrefs = hasrefs_cb;
	core->print->get_comments = get_comments_cb;
	core->print->get_section_name = get_section_name;
	core->print->use_comments = false;
	core->rtr_n = 0;
	core->blocksize_max = R_CORE_BLOCKSIZE_MAX;
	r_core_task_scheduler_init (&core->tasks, core);
	core->watchers = r_list_new ();
	core->watchers->free = (RListFree)r_core_cmpwatch_free;
	core->scriptstack = r_list_new ();
	core->scriptstack->free = (RListFree)free;
	core->log = r_core_log_new ();
	core->times = R_NEW0 (RCoreTimes);
	core->vmode = false;
	core->printidx = 0;
	core->lastcmd = NULL;
	core->cmdlog = NULL;
	core->stkcmd = NULL;
	core->cmdqueue = NULL;
	core->cmdrepeat = true;
	core->yank_buf = r_buf_new ();
	core->num = r_num_new (&num_callback, &str_callback, core);
	core->egg = r_egg_new ();
	r_egg_setup (core->egg, R_SYS_ARCH, R_SYS_BITS, 0, R_SYS_OS);

	core->undos = r_list_newf ((RListFree)r_core_undo_free);
	core->fixedarch = false;
	core->fixedbits = false;

	/* initialize libraries */
	core->cons = r_cons_new ();
	if (core->cons->refcnt == 1) {
		core->cons = r_cons_singleton ();
		if (core->cons->line) {
			core->cons->line->user = core;
			core->cons->line->cb_editor = \
				(RLineEditorCb)&r_core_editor;
			core->cons->line->cb_fkey = core->cons->cb_fkey;
		}
#if __EMSCRIPTEN__
		core->cons->user_fgets = NULL;
#else
		core->cons->user_fgets = (void *)r_core_fgets;
#endif
		//r_line_singleton ()->user = (void *)core;
		r_line_hist_load (R2_HOME_HISTORY);
	}
	core->print->cons = core->cons;
	r_cons_bind (&core->print->consbind);

	// We save the old num ad user, in order to restore it after free
	core->lang = r_lang_new ();
	core->lang->cmd_str = (char *(*)(void *, const char *))r_core_cmd_str;
	core->lang->cmdf = (int (*)(void *, const char *, ...))r_core_cmdf;
	r_core_bind_cons (core);
	core->lang->cb_printf = r_cons_printf;
	r_lang_define (core->lang, "RCore", "core", core);
	r_lang_set_user_ptr (core->lang, core);
	core->assembler = r_asm_new ();
	core->assembler->num = core->num;
	r_asm_set_user_ptr (core->assembler, core);
	core->anal = r_anal_new ();
	core->gadgets = r_list_newf ((RListFree)r_core_gadget_free);
	core->anal->ev = core->ev;
	core->anal->log = r_core_anal_log;
	core->anal->read_at = r_core_anal_read_at;
	core->anal->flag_get = r_core_flag_get_by_spaces;
	core->anal->cb.on_fcn_new = on_fcn_new;
	core->anal->cb.on_fcn_delete = on_fcn_delete;
	core->anal->cb.on_fcn_rename = on_fcn_rename;
	core->print->sdb_types = core->anal->sdb_types;
	core->assembler->syscall = r_syscall_ref (core->anal->syscall); // BIND syscall anal/asm
	r_anal_set_user_ptr (core->anal, core);
	core->anal->cb_printf = (void *) r_cons_printf;
	core->parser = r_parse_new ();
	r_anal_bind (core->anal, &(core->parser->analb));
	core->parser->varlist = r_anal_var_list;
	/// XXX shouhld be using coreb
	r_parse_set_user_ptr (core->parser, core);
	core->bin = r_bin_new ();
	r_cons_bind (&core->bin->consb);
	// XXX we shuold use RConsBind instead of this hardcoded pointer
	core->bin->cb_printf = (PrintfCallback) r_cons_printf;
	r_bin_set_user_ptr (core->bin, core);
	core->io = r_io_new ();
	core->io->ff = 1;
	core->io->user = (void *)core;
	core->io->cb_core_cmd = core_cmd_callback;
	core->io->cb_core_cmdstr = core_cmdstr_callback;
	core->io->cb_core_post_write = core_post_write_callback;
	core->search = r_search_new (R_SEARCH_KEYWORD);
	r_io_undo_enable (core->io, 1, 0); // TODO: configurable via eval
	core->fs = r_fs_new ();
	core->flags = r_flag_new ();
	core->flags->cb_printf = r_cons_printf;
	core->graph = r_agraph_new (r_cons_canvas_new (1, 1));
	core->graph->need_reload_nodes = false;
	core->asmqjmps_size = R_CORE_ASMQJMPS_NUM;
	if (sizeof (ut64) * core->asmqjmps_size < core->asmqjmps_size) {
		core->asmqjmps_size = 0;
		core->asmqjmps = NULL;
	} else {
		core->asmqjmps = R_NEWS (ut64, core->asmqjmps_size);
	}

	r_bin_bind (core->bin, &(core->assembler->binb));
	r_bin_bind (core->bin, &(core->anal->binb));
	r_bin_bind (core->bin, &(core->anal->binb));

	r_io_bind (core->io, &(core->search->iob));
	r_io_bind (core->io, &(core->print->iob));
	r_io_bind (core->io, &(core->anal->iob));
	r_io_bind (core->io, &(core->fs->iob));
	r_cons_bind (&(core->fs->csb));
	r_core_bind (core, &(core->fs->cob));
	r_io_bind (core->io, &(core->bin->iob));
	r_flag_bind (core->flags, &(core->anal->flb));
	core->anal->flg_class_set = core_flg_class_set;
	core->anal->flg_class_get = core_flg_class_get;
	core->anal->flg_fcn_set = core_flg_fcn_set;
	r_anal_bind (core->anal, &(core->parser->analb));
	core->parser->flag_get = r_core_flag_get_by_spaces;
	core->parser->label_get = r_anal_fcn_label_at;

	r_core_bind (core, &(core->anal->coreb));

	core->file = NULL;
	core->files = r_list_newf ((RListFree)r_core_file_free);
	core->offset = 0LL;
	core->prompt_offset = 0LL;
	r_core_cmd_init (core);
	core->dbg = r_debug_new (true);

	r_io_bind (core->io, &(core->dbg->iob));
	r_io_bind (core->io, &(core->dbg->bp->iob));
	r_core_bind (core, &core->dbg->corebind);
	r_core_bind (core, &core->dbg->bp->corebind);
	core->dbg->anal = core->anal; // XXX: dupped instance.. can cause lost pointerz
	//r_debug_use (core->dbg, "native");
// XXX pushing uninitialized regstate results in trashed reg values
//	r_reg_arena_push (core->dbg->reg); // create a 2 level register state stack
//	core->dbg->anal->reg = core->anal->reg; // XXX: dupped instance.. can cause lost pointerz
	core->io->cb_printf = r_cons_printf;
	core->dbg->cb_printf = r_cons_printf;
	core->dbg->bp->cb_printf = r_cons_printf;
	core->dbg->ev = core->ev;
	// initialize config before any corebind
	r_core_config_init (core);

	r_core_loadlibs_init (core);
	//r_core_loadlibs (core);

	// TODO: get arch from r_bin or from native arch
	r_asm_use (core->assembler, R_SYS_ARCH);
	r_anal_use (core->anal, R_SYS_ARCH);
	if (R_SYS_BITS & R_SYS_BITS_64) {
		r_config_set_i (core->config, "asm.bits", 64);
	} else {
		if (R_SYS_BITS & R_SYS_BITS_32) {
			r_config_set_i (core->config, "asm.bits", 32);
		}
	}
	r_config_set (core->config, "asm.arch", R_SYS_ARCH);
	r_bp_use (core->dbg->bp, R_SYS_ARCH, core->anal->bits);
	update_sdb (core);
	{
		char *a = r_str_r2_prefix (R2_FLAGS);
		if (a) {
			char *file = r_str_newf ("%s/tags.r2", a);
			(void)r_core_run_script (core, file);
			free (file);
			free (a);
		}
	}
	r_core_anal_type_init (core);
	__init_autocomplete (core);
	return 0;
}