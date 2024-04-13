static int qdisc_graft(struct net_device *dev, struct Qdisc *parent,
		       u32 classid,
		       struct Qdisc *new, struct Qdisc **old)
{
	int err = 0;
	struct Qdisc *q = *old;


	if (parent == NULL) { 
		if (q && q->flags&TCQ_F_INGRESS) {
			*old = dev_graft_qdisc(dev, q);
		} else {
			*old = dev_graft_qdisc(dev, new);
		}
	} else {
		struct Qdisc_class_ops *cops = parent->ops->cl_ops;

		err = -EINVAL;

		if (cops) {
			unsigned long cl = cops->get(parent, classid);
			if (cl) {
				err = cops->graft(parent, cl, new, old);
				if (new)
					new->parent = classid;
				cops->put(parent, cl);
			}
		}
	}
	return err;
}