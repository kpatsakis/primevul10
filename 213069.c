static int __disasm(void *_core, ut64 addr) {
	RCore *core = _core;
	ut64 prevaddr = core->offset;

	r_core_seek (core, addr, true);
	int len = r_core_print_disasm_instructions (core, 0, 1);
	r_core_seek (core, prevaddr, true);

	return len;
}