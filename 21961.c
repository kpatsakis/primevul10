static void stack_map_get_build_id_offset(struct bpf_stack_build_id *id_offs,
					  u64 *ips, u32 trace_nr, bool user)
{
	int i;
	struct vm_area_struct *vma;
	bool irq_work_busy = false;
	struct stack_map_irq_work *work = NULL;

	if (irqs_disabled()) {
		if (!IS_ENABLED(CONFIG_PREEMPT_RT)) {
			work = this_cpu_ptr(&up_read_work);
			if (irq_work_is_busy(&work->irq_work)) {
				/* cannot queue more up_read, fallback */
				irq_work_busy = true;
			}
		} else {
			/*
			 * PREEMPT_RT does not allow to trylock mmap sem in
			 * interrupt disabled context. Force the fallback code.
			 */
			irq_work_busy = true;
		}
	}

	/*
	 * We cannot do up_read() when the irq is disabled, because of
	 * risk to deadlock with rq_lock. To do build_id lookup when the
	 * irqs are disabled, we need to run up_read() in irq_work. We use
	 * a percpu variable to do the irq_work. If the irq_work is
	 * already used by another lookup, we fall back to report ips.
	 *
	 * Same fallback is used for kernel stack (!user) on a stackmap
	 * with build_id.
	 */
	if (!user || !current || !current->mm || irq_work_busy ||
	    !mmap_read_trylock(current->mm)) {
		/* cannot access current->mm, fall back to ips */
		for (i = 0; i < trace_nr; i++) {
			id_offs[i].status = BPF_STACK_BUILD_ID_IP;
			id_offs[i].ip = ips[i];
			memset(id_offs[i].build_id, 0, BUILD_ID_SIZE_MAX);
		}
		return;
	}

	for (i = 0; i < trace_nr; i++) {
		vma = find_vma(current->mm, ips[i]);
		if (!vma || build_id_parse(vma, id_offs[i].build_id, NULL)) {
			/* per entry fall back to ips */
			id_offs[i].status = BPF_STACK_BUILD_ID_IP;
			id_offs[i].ip = ips[i];
			memset(id_offs[i].build_id, 0, BUILD_ID_SIZE_MAX);
			continue;
		}
		id_offs[i].offset = (vma->vm_pgoff << PAGE_SHIFT) + ips[i]
			- vma->vm_start;
		id_offs[i].status = BPF_STACK_BUILD_ID_VALID;
	}

	if (!work) {
		mmap_read_unlock(current->mm);
	} else {
		work->mm = current->mm;

		/* The lock will be released once we're out of interrupt
		 * context. Tell lockdep that we've released it now so
		 * it doesn't complain that we forgot to release it.
		 */
		rwsem_release(&current->mm->mmap_lock.dep_map, _RET_IP_);
		irq_work_queue(&work->irq_work);
	}
}