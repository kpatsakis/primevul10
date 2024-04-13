static ut64 getref (RCore *core, int n, char t, int type) {
	RAnalFunction *fcn = r_anal_get_fcn_in (core->anal, core->offset, 0);
	RListIter *iter;
	RAnalRef *r;
	RList *list;
	int i = 0;
	if (!fcn) {
		return UT64_MAX;
	}
#if FCN_OLD
	if (t == 'r') {
		list = r_anal_function_get_refs (fcn);
	} else {
		list = r_anal_function_get_xrefs (fcn);
	}
	r_list_foreach (list, iter, r) {
		if (r->type == type) {
			if (i == n) {
				ut64 addr = r->addr;
				r_list_free (list);
				return addr;
			}
			i++;
		}
	}
	r_list_free (list);
#else
#warning implement getref() using sdb
#endif
	return UT64_MAX;
}