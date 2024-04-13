static inline void __wake_up_parent(struct task_struct *p,
				    struct task_struct *parent)
{
	wake_up_interruptible_sync(&parent->signal->wait_chldexit);
}