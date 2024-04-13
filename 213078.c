R_API char *r_core_op_str(RCore *core, ut64 addr) {
	RAsmOp op = {0};
	ut8 buf[64];
	r_asm_set_pc (core->assembler, addr);
	r_io_read_at (core->io, addr, buf, sizeof (buf));
	int ret = r_asm_disassemble (core->assembler, &op, buf, sizeof (buf));
	char *str = (ret > 0)? strdup (r_strbuf_get (&op.buf_asm)): NULL;
	r_asm_op_fini (&op);
	return str;
}