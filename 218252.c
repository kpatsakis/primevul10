static void virtual_submission_tasklet(unsigned long data)
{
	struct virtual_engine * const ve = (struct virtual_engine *)data;
	const int prio = ve->base.execlists.queue_priority_hint;
	intel_engine_mask_t mask;
	unsigned int n;

	rcu_read_lock();
	mask = virtual_submission_mask(ve);
	rcu_read_unlock();
	if (unlikely(!mask))
		return;

	local_irq_disable();
	for (n = 0; READ_ONCE(ve->request) && n < ve->num_siblings; n++) {
		struct intel_engine_cs *sibling = ve->siblings[n];
		struct ve_node * const node = &ve->nodes[sibling->id];
		struct rb_node **parent, *rb;
		bool first;

		if (unlikely(!(mask & sibling->mask))) {
			if (!RB_EMPTY_NODE(&node->rb)) {
				spin_lock(&sibling->active.lock);
				rb_erase_cached(&node->rb,
						&sibling->execlists.virtual);
				RB_CLEAR_NODE(&node->rb);
				spin_unlock(&sibling->active.lock);
			}
			continue;
		}

		spin_lock(&sibling->active.lock);

		if (!RB_EMPTY_NODE(&node->rb)) {
			/*
			 * Cheat and avoid rebalancing the tree if we can
			 * reuse this node in situ.
			 */
			first = rb_first_cached(&sibling->execlists.virtual) ==
				&node->rb;
			if (prio == node->prio || (prio > node->prio && first))
				goto submit_engine;

			rb_erase_cached(&node->rb, &sibling->execlists.virtual);
		}

		rb = NULL;
		first = true;
		parent = &sibling->execlists.virtual.rb_root.rb_node;
		while (*parent) {
			struct ve_node *other;

			rb = *parent;
			other = rb_entry(rb, typeof(*other), rb);
			if (prio > other->prio) {
				parent = &rb->rb_left;
			} else {
				parent = &rb->rb_right;
				first = false;
			}
		}

		rb_link_node(&node->rb, rb, parent);
		rb_insert_color_cached(&node->rb,
				       &sibling->execlists.virtual,
				       first);

submit_engine:
		GEM_BUG_ON(RB_EMPTY_NODE(&node->rb));
		node->prio = prio;
		if (first && prio > sibling->execlists.queue_priority_hint) {
			sibling->execlists.queue_priority_hint = prio;
			tasklet_hi_schedule(&sibling->execlists.tasklet);
		}

		spin_unlock(&sibling->active.lock);
	}
	local_irq_enable();
}