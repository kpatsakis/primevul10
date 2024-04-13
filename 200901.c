	__releases(dev->queue_lock)
{
	spin_unlock(&dev->ingress_lock);
	spin_unlock_bh(&dev->queue_lock);
}