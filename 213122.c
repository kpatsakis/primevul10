static ut64 bbSize(RAnalFunction *fcn, ut64 addr) {
	RListIter *iter;
	RAnalBlock *bb;
	r_list_foreach (fcn->bbs, iter, bb) {
		if (R_BETWEEN (bb->addr, addr, bb->addr + bb->size - 1)) {
			return bb->size;
		}
	}
	return 0;
}