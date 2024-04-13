static inline bool io_run_task_work(void)
{
	if (current->task_works) {
		__set_current_state(TASK_RUNNING);
		task_work_run();
		return true;
	}

	return false;
}