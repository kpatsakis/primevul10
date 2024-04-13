static void execlists_timeslice(struct timer_list *timer)
{
	execlists_kick(timer, timer);
}