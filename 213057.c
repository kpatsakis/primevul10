static ut64 bbInstructions(RAnalFunction *fcn, ut64 addr) {
	RListIter *iter;
	RAnalBlock *bb;
	r_list_foreach (fcn->bbs, iter, bb) {
		if (R_BETWEEN (bb->addr, addr, bb->addr + bb->size - 1)) {
			return bb->ninstr;
		}
	}
	return UT64_MAX;
}