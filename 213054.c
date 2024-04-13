static void *r_core_sleep_begin (RCore *core) {
	RCoreTask *task = r_core_task_self (&core->tasks);
	if (task) {
		r_core_task_sleep_begin (task);
	}
	return task;
}