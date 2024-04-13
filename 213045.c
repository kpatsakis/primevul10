R_API void r_core_prompt_loop(RCore *r) {
	int ret;
	do {
		int err = r_core_prompt (r, false);
		if (err < 1) {
			// handle ^D
			r->num->value = 0; // r.num->value will be read by r_main_radare2() after calling this fcn
			break;
		}
		/* -1 means invalid command, -2 means quit prompt loop */
		if ((ret = r_core_prompt_exec (r)) == -2) {
			break;
		}
	} while (ret != R_CORE_CMD_EXIT);
}