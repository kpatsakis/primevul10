void flush_signals(struct task_struct *t)
{
	unsigned long flags;

	spin_lock_irqsave(&t->sighand->siglock, flags);
	__flush_signals(t);
	spin_unlock_irqrestore(&t->sighand->siglock, flags);
}