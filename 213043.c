static ut64 bbBegin(RAnalFunction *fcn, ut64 addr) {
	RListIter *iter;
	RAnalBlock *bb;
	r_list_foreach (fcn->bbs, iter, bb) {
		if (R_BETWEEN (bb->addr, addr, bb->addr + bb->size - 1)) {
			return bb->addr;
		}
	}
	return UT64_MAX;
}