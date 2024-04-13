static char *r_core_anal_hasrefs_to_depth(RCore *core, ut64 value, int depth) {
	r_return_val_if_fail (core, NULL);
	if (depth < 1 || value == UT64_MAX) {
		return NULL;
	}
	RStrBuf *s = r_strbuf_new (NULL);
	char *mapname = NULL;
	RFlagItem *fi = r_flag_get_i (core->flags, value);
	ut64 type = r_core_anal_address (core, value);
	if (value && value != UT64_MAX) {
		RDebugMap *map = r_debug_map_get (core->dbg, value);
		if (map && map->name && map->name[0]) {
			mapname = strdup (map->name);
		}
	}
	if (mapname) {
		r_strbuf_appendf (s, " (%s)", mapname);
		R_FREE (mapname);
	}
	int bits = core->assembler->bits;
	switch (bits) {
	case 16: // umf, not in sync with pxr
		{
			st16 v = (st16)(value & UT16_MAX);
			st16 h = UT16_MAX / 0x100;
			if (v > -h && v < h) {
				r_strbuf_appendf (s," %hd", v);
			}
		}
		break;
	case 32:
		{
			st32 v = (st32)(value & 0xffffffff);
			st32 h = UT32_MAX / 0x10000;
			if (v > -h && v < h) {
				r_strbuf_appendf (s," %d", v);
			}
		}
		break;
	case 64:
		{
			st64 v = (st64)(value);
			st64 h = UT64_MAX / 0x1000000;
			if (v > -h && v < h) {
				r_strbuf_appendf (s," %"PFMT64d, v);
			}
		}
		break;
	}
	RBinSection *sect = value? r_bin_get_section_at (r_bin_cur_object (core->bin), value, true): NULL;
	if(! ((type&R_ANAL_ADDR_TYPE_HEAP)||(type&R_ANAL_ADDR_TYPE_STACK)) ) {
		// Do not repeat "stack" or "heap" words unnecessarily.
		if (sect && sect->name[0]) {
			r_strbuf_appendf (s," (%s)", sect->name);
		}
	}
	if (fi) {
		RRegItem *r = r_reg_get (core->dbg->reg, fi->name, -1);
		if (!r) {
			r_strbuf_appendf (s, " %s", fi->name);
		}
	}
	RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, value, 0);
	if (fcn) {
		r_strbuf_appendf (s, " %s", fcn->name);
	}
	if (type) {
		const char *c = r_core_anal_optype_colorfor (core, value, true);
		const char *cend = (c && *c) ? Color_RESET: "";
		if (!c) {
			c = "";
		}
		if (type & R_ANAL_ADDR_TYPE_HEAP) {
			r_strbuf_appendf (s, " %sheap%s", c, cend);
		} else if (type & R_ANAL_ADDR_TYPE_STACK) {
			r_strbuf_appendf (s, " %sstack%s", c, cend);
		}
		if (type & R_ANAL_ADDR_TYPE_PROGRAM) {
			r_strbuf_appendf (s, " %sprogram%s", c, cend);
		}
		if (type & R_ANAL_ADDR_TYPE_LIBRARY) {
			r_strbuf_appendf (s, " %slibrary%s", c, cend);
		}
		if (type & R_ANAL_ADDR_TYPE_ASCII) {
			r_strbuf_appendf (s, " %sascii%s ('%c')", c, cend, value);
		}
		if (type & R_ANAL_ADDR_TYPE_SEQUENCE) {
			r_strbuf_appendf (s, " %ssequence%s", c, cend);
		}
		if (type & R_ANAL_ADDR_TYPE_READ) {
			r_strbuf_appendf (s, " %sR%s", c, cend);
		}
		if (type & R_ANAL_ADDR_TYPE_WRITE) {
			r_strbuf_appendf (s, " %sW%s", c, cend);
		}
		if (type & R_ANAL_ADDR_TYPE_EXEC) {
			RAsmOp op;
			ut8 buf[32];
			r_strbuf_appendf (s, " %sX%s", c, cend);
			/* instruction disassembly */
			r_io_read_at (core->io, value, buf, sizeof (buf));
			r_asm_set_pc (core->assembler, value);
			r_asm_disassemble (core->assembler, &op, buf, sizeof (buf));
			r_strbuf_appendf (s, " '%s'", r_asm_op_get_asm (&op));
			/* get library name */
			{ // NOTE: dup for mapname?
				RDebugMap *map;
				RListIter *iter;
				r_list_foreach (core->dbg->maps, iter, map) {
					if ((value >= map->addr) &&
						(value<map->addr_end)) {
						const char *lastslash = r_str_lchr (map->name, '/');
						r_strbuf_appendf (s, " '%s'", lastslash?
							lastslash+1:map->name);
						break;
					}
				}
			}
		} else if (type & R_ANAL_ADDR_TYPE_READ) {
			ut8 buf[32];
			ut32 *n32 = (ut32 *)buf;
			ut64 *n64 = (ut64*)buf;
			r_io_read_at (core->io, value, buf, sizeof (buf));
			ut64 n = (core->assembler->bits == 64)? *n64: *n32;
			r_strbuf_appendf (s, " 0x%"PFMT64x, n);
		}
	}
	{
		ut8 buf[128], widebuf[256];
		const char *c = r_config_get_i (core->config, "scr.color")? core->cons->context->pal.ai_ascii: "";
		const char *cend = (c && *c) ? Color_RESET: "";
		int len, r;
		if (r_io_read_at (core->io, value, buf, sizeof (buf))) {
			buf[sizeof (buf) - 1] = 0;
			switch (is_string (buf, sizeof(buf), &len)) {
			case 1:
				r_strbuf_appendf (s, " (%s%s%s)", c, buf, cend);
				break;
			case 2:
				r = r_utf8_encode_str ((const RRune *)buf, widebuf,
						       sizeof (widebuf) - 1);
				if (r == -1) {
					eprintf ("Something was wrong with refs\n");
				} else {
					r_strbuf_appendf (s, " (%s%s%s)", c, widebuf, cend);
				}
				break;
			}
		}

	}
	if ((type & R_ANAL_ADDR_TYPE_READ) && !(type & R_ANAL_ADDR_TYPE_EXEC) && depth) {
		// Try to telescope further, but only several levels deep.
		ut8 buf[32];
		ut32 *n32 = (ut32 *)buf;
		ut64 *n64 = (ut64*)buf;
		r_io_read_at (core->io, value, buf, sizeof (buf));
		ut64 n = (core->assembler->bits == 64)? *n64: *n32;
		if(n != value) {
			char* rrstr = r_core_anal_hasrefs_to_depth (core, n, depth-1);
			if (rrstr) {
				if (rrstr[0]) {
					r_strbuf_appendf (s, " --> %s", rrstr);
				}
				free (rrstr);
			}
		}
	}
	free (mapname);
	return r_strbuf_drain (s);
}