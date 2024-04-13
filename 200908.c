	__acquires(dev->ingress_lock)
{
	spin_lock_bh(&dev->queue_lock);
	spin_lock(&dev->ingress_lock);
}