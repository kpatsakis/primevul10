static int rose_connect(struct socket *sock, struct sockaddr *uaddr, int addr_len, int flags)
{
	struct sock *sk = sock->sk;
	struct rose_sock *rose = rose_sk(sk);
	struct sockaddr_rose *addr = (struct sockaddr_rose *)uaddr;
	unsigned char cause, diagnostic;
	struct net_device *dev;
	ax25_uid_assoc *user;
	int n, err = 0;

	if (addr_len != sizeof(struct sockaddr_rose) && addr_len != sizeof(struct full_sockaddr_rose))
		return -EINVAL;

	if (addr->srose_family != AF_ROSE)
		return -EINVAL;

	if (addr_len == sizeof(struct sockaddr_rose) && addr->srose_ndigis > 1)
		return -EINVAL;

	if (addr->srose_ndigis > ROSE_MAX_DIGIS)
		return -EINVAL;

	/* Source + Destination digis should not exceed ROSE_MAX_DIGIS */
	if ((rose->source_ndigis + addr->srose_ndigis) > ROSE_MAX_DIGIS)
		return -EINVAL;

	lock_sock(sk);

	if (sk->sk_state == TCP_ESTABLISHED && sock->state == SS_CONNECTING) {
		/* Connect completed during a ERESTARTSYS event */
		sock->state = SS_CONNECTED;
		goto out_release;
	}

	if (sk->sk_state == TCP_CLOSE && sock->state == SS_CONNECTING) {
		sock->state = SS_UNCONNECTED;
		err = -ECONNREFUSED;
		goto out_release;
	}

	if (sk->sk_state == TCP_ESTABLISHED) {
		/* No reconnect on a seqpacket socket */
		err = -EISCONN;
		goto out_release;
	}

	sk->sk_state   = TCP_CLOSE;
	sock->state = SS_UNCONNECTED;

	rose->neighbour = rose_get_neigh(&addr->srose_addr, &cause,
					 &diagnostic, 0);
	if (!rose->neighbour) {
		err = -ENETUNREACH;
		goto out_release;
	}

	rose->lci = rose_new_lci(rose->neighbour);
	if (!rose->lci) {
		err = -ENETUNREACH;
		goto out_release;
	}

	if (sock_flag(sk, SOCK_ZAPPED)) {	/* Must bind first - autobinding in this may or may not work */
		sock_reset_flag(sk, SOCK_ZAPPED);

		if ((dev = rose_dev_first()) == NULL) {
			err = -ENETUNREACH;
			goto out_release;
		}

		user = ax25_findbyuid(current_euid());
		if (!user) {
			err = -EINVAL;
			goto out_release;
		}

		memcpy(&rose->source_addr, dev->dev_addr, ROSE_ADDR_LEN);
		rose->source_call = user->call;
		rose->device      = dev;
		ax25_uid_put(user);

		rose_insert_socket(sk);		/* Finish the bind */
	}
rose_try_next_neigh:
	rose->dest_addr   = addr->srose_addr;
	rose->dest_call   = addr->srose_call;
	rose->rand        = ((long)rose & 0xFFFF) + rose->lci;
	rose->dest_ndigis = addr->srose_ndigis;

	if (addr_len == sizeof(struct full_sockaddr_rose)) {
		struct full_sockaddr_rose *full_addr = (struct full_sockaddr_rose *)uaddr;
		for (n = 0 ; n < addr->srose_ndigis ; n++)
			rose->dest_digis[n] = full_addr->srose_digis[n];
	} else {
		if (rose->dest_ndigis == 1) {
			rose->dest_digis[0] = addr->srose_digi;
		}
	}

	/* Move to connecting socket, start sending Connect Requests */
	sock->state   = SS_CONNECTING;
	sk->sk_state     = TCP_SYN_SENT;

	rose->state = ROSE_STATE_1;

	rose->neighbour->use++;

	rose_write_internal(sk, ROSE_CALL_REQUEST);
	rose_start_heartbeat(sk);
	rose_start_t1timer(sk);

	/* Now the loop */
	if (sk->sk_state != TCP_ESTABLISHED && (flags & O_NONBLOCK)) {
		err = -EINPROGRESS;
		goto out_release;
	}

	/*
	 * A Connect Ack with Choke or timeout or failed routing will go to
	 * closed.
	 */
	if (sk->sk_state == TCP_SYN_SENT) {
		DEFINE_WAIT(wait);

		for (;;) {
			prepare_to_wait(sk->sk_sleep, &wait,
					TASK_INTERRUPTIBLE);
			if (sk->sk_state != TCP_SYN_SENT)
				break;
			if (!signal_pending(current)) {
				release_sock(sk);
				schedule();
				lock_sock(sk);
				continue;
			}
			err = -ERESTARTSYS;
			break;
		}
		finish_wait(sk->sk_sleep, &wait);

		if (err)
			goto out_release;
	}

	if (sk->sk_state != TCP_ESTABLISHED) {
	/* Try next neighbour */
		rose->neighbour = rose_get_neigh(&addr->srose_addr, &cause, &diagnostic, 0);
		if (rose->neighbour)
			goto rose_try_next_neigh;

		/* No more neighbours */
		sock->state = SS_UNCONNECTED;
		err = sock_error(sk);	/* Always set at this point */
		goto out_release;
	}

	sock->state = SS_CONNECTED;

out_release:
	release_sock(sk);

	return err;
}