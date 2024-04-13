static void init_window_mask(struct rar5* rar) {
	if (rar->cstate.window_size)
		rar->cstate.window_mask = rar->cstate.window_size - 1;
	else
		rar->cstate.window_mask = 0;
}