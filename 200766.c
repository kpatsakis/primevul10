static ssize_t __splice_from_pipe(struct pipe_inode_info *pipe,
				  struct file *out, loff_t *ppos, size_t len,
				  unsigned int flags, splice_actor *actor)
{
	int ret, do_wakeup, err;
	struct splice_desc sd;

	ret = 0;
	do_wakeup = 0;

	sd.total_len = len;
	sd.flags = flags;
	sd.file = out;
	sd.pos = *ppos;

	for (;;) {
		if (pipe->nrbufs) {
			struct pipe_buffer *buf = pipe->bufs + pipe->curbuf;
			struct pipe_buf_operations *ops = buf->ops;

			sd.len = buf->len;
			if (sd.len > sd.total_len)
				sd.len = sd.total_len;

			err = actor(pipe, buf, &sd);
			if (err <= 0) {
				if (!ret && err != -ENODATA)
					ret = err;

				break;
			}

			ret += err;
			buf->offset += err;
			buf->len -= err;

			sd.len -= err;
			sd.pos += err;
			sd.total_len -= err;
			if (sd.len)
				continue;

			if (!buf->len) {
				buf->ops = NULL;
				ops->release(pipe, buf);
				pipe->curbuf = (pipe->curbuf + 1) & (PIPE_BUFFERS - 1);
				pipe->nrbufs--;
				if (pipe->inode)
					do_wakeup = 1;
			}

			if (!sd.total_len)
				break;
		}

		if (pipe->nrbufs)
			continue;
		if (!pipe->writers)
			break;
		if (!pipe->waiting_writers) {
			if (ret)
				break;
		}

		if (flags & SPLICE_F_NONBLOCK) {
			if (!ret)
				ret = -EAGAIN;
			break;
		}

		if (signal_pending(current)) {
			if (!ret)
				ret = -ERESTARTSYS;
			break;
		}

		if (do_wakeup) {
			smp_mb();
			if (waitqueue_active(&pipe->wait))
				wake_up_interruptible_sync(&pipe->wait);
			kill_fasync(&pipe->fasync_writers, SIGIO, POLL_OUT);
			do_wakeup = 0;
		}

		pipe_wait(pipe);
	}

	if (do_wakeup) {
		smp_mb();
		if (waitqueue_active(&pipe->wait))
			wake_up_interruptible(&pipe->wait);
		kill_fasync(&pipe->fasync_writers, SIGIO, POLL_OUT);
	}

	return ret;
}