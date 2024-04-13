static void r_core_sleep_end (RCore *core, void *user) {
	RCoreTask *task = (RCoreTask *)user;
	if (task) {
		r_core_task_sleep_end (task);
	}
}