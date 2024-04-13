
static int io_arm_poll_handler(struct io_kiocb *req)
{
	const struct io_op_def *def = &io_op_defs[req->opcode];
	struct io_ring_ctx *ctx = req->ctx;
	struct async_poll *apoll;
	struct io_poll_table ipt;
	__poll_t ret, mask = EPOLLONESHOT | POLLERR | POLLPRI;
	int rw;

	if (!req->file || !file_can_poll(req->file))
		return IO_APOLL_ABORTED;
	if (req->flags & REQ_F_POLLED)
		return IO_APOLL_ABORTED;
	if (!def->pollin && !def->pollout)
		return IO_APOLL_ABORTED;

	if (def->pollin) {
		rw = READ;
		mask |= POLLIN | POLLRDNORM;

		/* If reading from MSG_ERRQUEUE using recvmsg, ignore POLLIN */
		if ((req->opcode == IORING_OP_RECVMSG) &&
		    (req->sr_msg.msg_flags & MSG_ERRQUEUE))
			mask &= ~POLLIN;
	} else {
		rw = WRITE;
		mask |= POLLOUT | POLLWRNORM;
	}

	/* if we can't nonblock try, then no point in arming a poll handler */
	if (!io_file_supports_async(req, rw))
		return IO_APOLL_ABORTED;

	apoll = kmalloc(sizeof(*apoll), GFP_ATOMIC);
	if (unlikely(!apoll))
		return IO_APOLL_ABORTED;
	apoll->double_poll = NULL;
	req->apoll = apoll;
	req->flags |= REQ_F_POLLED;
	ipt.pt._qproc = io_async_queue_proc;

	ret = __io_arm_poll_handler(req, &apoll->poll, &ipt, mask,
					io_async_wake);
	if (ret || ipt.error) {
		io_poll_remove_double(req);
		spin_unlock_irq(&ctx->completion_lock);
		if (ret)
			return IO_APOLL_READY;
		return IO_APOLL_ABORTED;
	}
	spin_unlock_irq(&ctx->completion_lock);
	trace_io_uring_poll_arm(ctx, req, req->opcode, req->user_data,
				mask, apoll->poll.events);
	return IO_APOLL_OK;