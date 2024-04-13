R_API RBuffer *r_core_syscall (RCore *core, const char *name, const char *args) {
	RBuffer *b = NULL;
	char code[1024];
	int num;

	//arch check
	if (strcmp (core->anal->cur->arch, "x86")) {
		eprintf ("architecture not yet supported!\n");
		return 0;
	}

	num = r_syscall_get_num (core->anal->syscall, name);

	//bits check
	switch (core->assembler->bits) {
	case 32:
		if (strcmp (name, "setup") && !num ) {
			eprintf ("syscall not found!\n");
			return 0;
		}
		break;
	case 64:
		if (strcmp (name, "read") && !num ) {
			eprintf ("syscall not found!\n");
			return 0;
		}
		break;
	default:
		eprintf ("syscall not found!\n");
		return 0;
	}

	snprintf (code, sizeof (code),
		"sc@syscall(%d);\n"
		"main@global(0) { sc(%s);\n"
		":int3\n" /// XXX USE trap
		"}\n", num, args);
	r_egg_reset (core->egg);
	// TODO: setup arch/bits/os?
	r_egg_load (core->egg, code, 0);

	if (!r_egg_compile (core->egg)) {
		eprintf ("Cannot compile.\n");
	}
	if (!r_egg_assemble (core->egg)) {
		eprintf ("r_egg_assemble: invalid assembly\n");
	}
	if ((b = r_egg_get_bin (core->egg))) {
#if 0
		if (b->length > 0) {
			for (i = 0; i < b->length; i++) {
				r_cons_printf ("%02x", b->buf[i]);
			}
			r_cons_printf ("\n");
		}
#endif
	}
	return b;
}