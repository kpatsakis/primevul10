struct task_struct *fork_idle(int cpu)
{
	struct task_struct *task;
	struct kernel_clone_args args = {
		.flags = CLONE_VM,
	};

	task = copy_process(&init_struct_pid, 0, cpu_to_node(cpu), &args);
	if (!IS_ERR(task)) {
		init_idle_pids(task);
		init_idle(task, cpu);
	}

	return task;
}