static void autocomplete_breakpoints(RCore *core, RLineCompletion *completion, const char *str) {
	r_return_if_fail (str);
	RListIter *iter;
	RBreakpoint *bp = core->dbg->bp;
	RBreakpointItem *b;
	int n = strlen (str);
	r_list_foreach (bp->bps, iter, b) {
		char *addr = r_str_newf ("0x%"PFMT64x"", b->addr);
		if (!strncmp (addr, str, n)) {
			r_line_completion_push (completion, addr);
		}
		free (addr);
	}
}