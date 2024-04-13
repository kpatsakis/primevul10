static ut64 num_callback(RNum *userptr, const char *str, int *ok) {
	RCore *core = (RCore *)userptr; // XXX ?
	RAnalFunction *fcn;
	char *ptr, *bptr, *out = NULL;
	RFlagItem *flag;
	RBinSection *s;
	RAnalOp op;
	ut64 ret = 0;

	if (ok) {
		*ok = false;
	}
	switch (*str) {
	case '.':
		if (str[1] == '.') {
			if (ok) {
				*ok = true;
			}
			return r_num_tail (core->num, core->offset, str + 2);
		}
		if (core->num->nc.curr_tok == '+') {
			ut64 off = core->num->nc.number_value.n;
			if (!off) {
				off = core->offset;
			}
			RAnalFunction *fcn = r_anal_get_function_at (core->anal, off);
			if (fcn) {
				if (ok) {
					*ok = true;
				}
				ut64 dst = r_anal_fcn_label_get (core->anal, fcn, str + 1);
				if (dst == UT64_MAX) {
					dst = fcn->addr;
				}
				st64 delta = dst - off;
				if (delta < 0) {
					core->num->nc.curr_tok = '-';
					delta = off - dst;
				}
				return delta;
			}
		}
		break;
	case '[':
{
		ut64 n = 0LL;
		int refsz = core->assembler->bits / 8;
		const char *p = NULL;
		if (strlen (str) > 5) {
			p = strchr (str + 5, ':');
		}
		if (p) {
			refsz = atoi (str + 1);
			str = p;
		}
		// push state
		if (str[0] && str[1]) {
			const char *q;
			char *o = strdup (str + 1);
			if (o) {
				q = r_num_calc_index (core->num, NULL);
				if (q) {
					if (r_str_replace_char (o, ']', 0)>0) {
						n = r_num_math (core->num, o);
						if (core->num->nc.errors) {
							return 0;
						}
						r_num_calc_index (core->num, q);
					}
				}
				free (o);
			}
		} else {
			return 0;
		}
		// pop state
		if (ok) {
			*ok = 1;
		}
		ut8 buf[sizeof (ut64)] = R_EMPTY;
		(void)r_io_read_at (core->io, n, buf, R_MIN (sizeof (buf), refsz));
		switch (refsz) {
		case 8:
			return r_read_ble64 (buf, core->print->big_endian);
		case 4:
			return r_read_ble32 (buf, core->print->big_endian);
		case 2:
			return r_read_ble16 (buf, core->print->big_endian);
		case 1:
			return r_read_ble8 (buf);
		default:
			eprintf ("Invalid reference size: %d (%s)\n", refsz, str);
			return 0LL;
		}
}
		break;
	case '$':
		if (ok) {
			*ok = 1;
		}
		// TODO: group analop-dependant vars after a char, so i can filter
		r_anal_op (core->anal, &op, core->offset, core->block, core->blocksize, R_ANAL_OP_MASK_BASIC);
		r_anal_op_fini (&op); // we don't need strings or pointers, just values, which are not nullified in fini
		switch (str[1]) {
		case '.': // can use pc, sp, a0, a1, ...
			return r_debug_reg_get (core->dbg, str + 2);
		case 'k': // $k{kv}
			if (str[2] != '{') {
				eprintf ("Expected '{' after 'k'.\n");
				break;
			}
			bptr = strdup (str + 3);
			ptr = strchr (bptr, '}');
			if (!ptr) {
				// invalid json
				free (bptr);
				break;
			}
			*ptr = '\0';
			ret = 0LL;
			out = sdb_querys (core->sdb, NULL, 0, bptr);
			if (out && *out) {
				if (strstr (out, "$k{")) {
					eprintf ("Recursivity is not permitted here\n");
				} else {
					ret = r_num_math (core->num, out);
				}
			}
			free (bptr);
			free (out);
			return ret;
			break;
		case '{': // ${ev} eval var
			bptr = strdup (str + 2);
			ptr = strchr (bptr, '}');
			if (ptr) {
				ptr[0] = '\0';
				ut64 ret = r_config_get_i (core->config, bptr);
				free (bptr);
				return ret;
			}
			// take flag here
			free (bptr);
			break;
		case 'c': // $c console width
			return r_cons_get_size (NULL);
		case 'r': // $r
			if (str[2] == '{') {
				bptr = strdup (str + 3);
				ptr = strchr (bptr, '}');
				if (!ptr) {
					free (bptr);
					break;
				}
				*ptr = 0;
				if (r_config_get_i (core->config, "cfg.debug")) {
					if (r_debug_reg_sync (core->dbg, R_REG_TYPE_GPR, false)) {
						RRegItem *r = r_reg_get (core->dbg->reg, bptr, -1);
						if (r) {
							free (bptr);
							return r_reg_get_value (core->dbg->reg, r);
						}
					}
				} else {
					RRegItem *r = r_reg_get (core->anal->reg, bptr, -1);
					if (r) {
						free (bptr);
						return r_reg_get_value (core->anal->reg, r);
					}
				}
				free (bptr);
				return 0; // UT64_MAX;
			} else {
				int rows;
				(void)r_cons_get_size (&rows);
				return rows;
			}
			break;
		case 'e': // $e
			if (str[2] == '{') { // $e{flag} flag off + size
				char *flagName = strdup (str + 3);
				int flagLength = strlen (flagName);
				if (flagLength > 0) {
					flagName[flagLength - 1] = 0;
				}
				RFlagItem *flag = r_flag_get (core->flags, flagName);
				free (flagName);
				if (flag) {
					return flag->offset + flag->size;
				}
				return UT64_MAX;
			}
			return r_anal_op_is_eob (&op);
		case 'j': // $j jump address
			return op.jump;
		case 'p': // $p
			return r_sys_getpid ();
		case 'P': // $P
			return core->dbg->pid > 0 ? core->dbg->pid : 0;
		case 'f': // $f jump fail address
			if (str[2] == 'l') { // $fl flag length
				RFlagItem *fi = r_flag_get_i (core->flags, core->offset);
				if (fi) {
					return fi->size;
				}
				return 0;
			}
			return op.fail;
		case 'm': // $m memref
			return op.ptr;
		case 'B': // $B base address
		case 'M': { // $M map address
				ut64 lower = UT64_MAX;
				ut64 size = 0LL;
				RIOMap *map = r_io_map_get (core->io, core->offset);
				if (map) {
					lower = r_itv_begin (map->itv);
					size = r_itv_size (map->itv);
				}

				if (str[1] == 'B') {
					/* clear lower bits of the lowest map address to define the base address */
					const int clear_bits = 16;
					lower >>= clear_bits;
					lower <<= clear_bits;
				}
				if (str[2] == 'M') {
					return size;
				}
				return (lower == UT64_MAX)? 0LL: lower;
			}
			break;
		case 'v': // $v immediate value
			return op.val;
		case 'l': // $l opcode length
			return op.size;
		case 'b': // $b
			return core->blocksize;
		case 's': // $s file size
			if (str[2] == '{') { // $s{flag} flag size
				bptr = strdup (str + 3);
				ptr = strchr (bptr, '}');
				if (!ptr) {
					// invalid json
					free (bptr);
					break;
				}
				*ptr = '\0';
				RFlagItem *flag = r_flag_get (core->flags, bptr);
				ret = flag? flag->size: 0LL; // flag
				free (bptr);
				free (out);
				return ret;
			} else if (core->file) {
				return r_io_fd_size (core->io, core->file->fd);
			}
			return 0LL;
		case 'w': // $w word size
			return r_config_get_i (core->config, "asm.bits") / 8;
		case 'S': // $S section offset
			{
				RBinObject *bo = r_bin_cur_object (core->bin);
				if (bo && (s = r_bin_get_section_at (bo, core->offset, true))) {
					return (str[2] == 'S'? s->size: s->vaddr);
				}
			}
			return 0LL;
		case 'D': // $D
			if (str[2] == 'B') { // $DD
				return r_debug_get_baddr (core->dbg, NULL);
			} else if (IS_DIGIT (str[2])) {
				return getref (core, atoi (str + 2), 'r', R_ANAL_REF_TYPE_DATA);
			} else {
				RDebugMap *map;
				RListIter *iter;
				r_list_foreach (core->dbg->maps, iter, map) {
					if (core->offset >= map->addr && core->offset < map->addr_end) {
						return (str[2] == 'D')? map->size: map->addr;
					}
				}
			}
			return 0LL; // maybe // return UT64_MAX;
		case '?': // $?
			return core->num->value; // rc;
		case '$': // $$ offset
			return str[2] == '$' ? core->prompt_offset : core->offset;
		case 'o': { // $o
			RBinSection *s = r_bin_get_section_at (r_bin_cur_object (core->bin), core->offset, true);
			return s ? core->offset - s->vaddr + s->paddr : core->offset;
			break;
		}
		case 'O': // $O
			  if (core->print->cur_enabled) {
				  return core->offset + core->print->cur;
			  }
			  return core->offset;
		case 'C': // $C nth call
			return getref (core, atoi (str + 2), 'r', R_ANAL_REF_TYPE_CALL);
		case 'J': // $J nth jump
			return getref (core, atoi (str + 2), 'r', R_ANAL_REF_TYPE_CODE);
		case 'X': // $X nth xref
			return getref (core, atoi (str + 2), 'x', R_ANAL_REF_TYPE_CALL);
		case 'F': // $F function size
			fcn = r_anal_get_fcn_in (core->anal, core->offset, 0);
			if (fcn) {
				switch (str[2]) {
				/* function bounds (uppercase) */
				case 'B': return fcn->addr; // begin
				case 'E': return r_anal_function_max_addr (fcn); // end
				case 'S': return (str[3]=='S') ? r_anal_function_realsize (fcn) : r_anal_function_linear_size (fcn);
				case 'I': return fcn->ninstr;
				/* basic blocks (lowercase) */
				case 'b': return bbBegin (fcn, core->offset);
				case 'e': return bbBegin (fcn, core->offset) + bbSize (fcn, core->offset);
				case 'i': return bbInstructions (fcn, core->offset);
				case 's': return bbSize (fcn, core->offset);
				case 'j': return bbJump (fcn, core->offset); // jump
				case 'f': return bbFail (fcn, core->offset); // fail
				}
				return fcn->addr;
			}
			return 0;
		}
		break;
	default:
		if (*str >= 'A') {
			// NOTE: functions override flags
			RAnalFunction *fcn = r_anal_get_function_byname (core->anal, str);
			if (fcn) {
				if (ok) {
					*ok = true;
				}
				return fcn->addr;
			}
#if 0
			ut64 addr = r_anal_fcn_label_get (core->anal, core->offset, str);
			if (addr != 0) {
				ret = addr;
			} else {
				...
			}
#endif
			if ((flag = r_flag_get (core->flags, str))) {
				ret = flag->offset;
				if (ok) {
					*ok = true;
				}
				return ret;
			}

			// check for reg alias
			struct r_reg_item_t *r = r_reg_get (core->dbg->reg, str, -1);
			if (!r) {
				int role = r_reg_get_name_idx (str);
				if (role != -1) {
					const char *alias = r_reg_get_name (core->dbg->reg, role);
					if (alias) {
						r = r_reg_get (core->dbg->reg, alias, -1);
						if (r) {
							if (ok) {
								*ok = true;
							}
							ret = r_reg_get_value (core->dbg->reg, r);
							return ret;
						}
					}
				}
			} else {
				if (ok) {
					*ok = true;
				}
				ret = r_reg_get_value (core->dbg->reg, r);
				return ret;
			}
		}
		break;
	}

	return ret;
}