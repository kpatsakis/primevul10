static void __delayed_free_task(struct rcu_head *rhp)
{
	struct task_struct *tsk = container_of(rhp, struct task_struct, rcu);

	free_task(tsk);
}