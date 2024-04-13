static void execlists_preempt(struct timer_list *timer)
{
	execlists_kick(timer, preempt);
}