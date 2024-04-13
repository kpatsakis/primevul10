static bool r_core_anal_read_at(struct r_anal_t *anal, ut64 addr, ut8 *buf, int len) {
	return r_io_read_at (anal->iob.io, addr, buf, len);
}