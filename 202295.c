int rtnl_lock_interruptible(void)
{
	return down_interruptible(&rtnl_sem);
}