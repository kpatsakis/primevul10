R_API RAnalOp *r_core_op_anal(RCore *core, ut64 addr) {
	ut8 buf[64];
	RAnalOp *op = R_NEW (RAnalOp);
	r_io_read_at (core->io, addr, buf, sizeof (buf));
	r_anal_op (core->anal, op, addr, buf, sizeof (buf), R_ANAL_OP_MASK_ALL);
	return op;
}