static struct sigqueue *__sigqueue_alloc(struct task_struct *t, gfp_t flags,
					 int override_rlimit)
{
	struct sigqueue *q = NULL;
	struct user_struct *user;

	/*
	 * We won't get problems with the target's UID changing under us
	 * because changing it requires RCU be used, and if t != current, the
	 * caller must be holding the RCU readlock (by way of a spinlock) and
	 * we use RCU protection here
	 */
	user = get_uid(__task_cred(t)->user);
	atomic_inc(&user->sigpending);
	if (override_rlimit ||
	    atomic_read(&user->sigpending) <=
			t->signal->rlim[RLIMIT_SIGPENDING].rlim_cur)
		q = kmem_cache_alloc(sigqueue_cachep, flags);
	if (unlikely(q == NULL)) {
		atomic_dec(&user->sigpending);
		free_uid(user);
	} else {
		INIT_LIST_HEAD(&q->list);
		q->flags = 0;
		q->user = user;
	}

	return q;
}