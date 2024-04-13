static int rose_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg)
{
	struct sock *sk = sock->sk;
	struct rose_sock *rose = rose_sk(sk);
	void __user *argp = (void __user *)arg;

	switch (cmd) {
	case TIOCOUTQ: {
		long amount;

		amount = sk->sk_sndbuf - sk_wmem_alloc_get(sk);
		if (amount < 0)
			amount = 0;
		return put_user(amount, (unsigned int __user *) argp);
	}

	case TIOCINQ: {
		struct sk_buff *skb;
		long amount = 0L;
		/* These two are safe on a single CPU system as only user tasks fiddle here */
		if ((skb = skb_peek(&sk->sk_receive_queue)) != NULL)
			amount = skb->len;
		return put_user(amount, (unsigned int __user *) argp);
	}

	case SIOCGSTAMP:
		return sock_get_timestamp(sk, (struct timeval __user *) argp);

	case SIOCGSTAMPNS:
		return sock_get_timestampns(sk, (struct timespec __user *) argp);

	case SIOCGIFADDR:
	case SIOCSIFADDR:
	case SIOCGIFDSTADDR:
	case SIOCSIFDSTADDR:
	case SIOCGIFBRDADDR:
	case SIOCSIFBRDADDR:
	case SIOCGIFNETMASK:
	case SIOCSIFNETMASK:
	case SIOCGIFMETRIC:
	case SIOCSIFMETRIC:
		return -EINVAL;

	case SIOCADDRT:
	case SIOCDELRT:
	case SIOCRSCLRRT:
		if (!capable(CAP_NET_ADMIN))
			return -EPERM;
		return rose_rt_ioctl(cmd, argp);

	case SIOCRSGCAUSE: {
		struct rose_cause_struct rose_cause;
		rose_cause.cause      = rose->cause;
		rose_cause.diagnostic = rose->diagnostic;
		return copy_to_user(argp, &rose_cause, sizeof(struct rose_cause_struct)) ? -EFAULT : 0;
	}

	case SIOCRSSCAUSE: {
		struct rose_cause_struct rose_cause;
		if (copy_from_user(&rose_cause, argp, sizeof(struct rose_cause_struct)))
			return -EFAULT;
		rose->cause      = rose_cause.cause;
		rose->diagnostic = rose_cause.diagnostic;
		return 0;
	}

	case SIOCRSSL2CALL:
		if (!capable(CAP_NET_ADMIN)) return -EPERM;
		if (ax25cmp(&rose_callsign, &null_ax25_address) != 0)
			ax25_listen_release(&rose_callsign, NULL);
		if (copy_from_user(&rose_callsign, argp, sizeof(ax25_address)))
			return -EFAULT;
		if (ax25cmp(&rose_callsign, &null_ax25_address) != 0)
			return ax25_listen_register(&rose_callsign, NULL);

		return 0;

	case SIOCRSGL2CALL:
		return copy_to_user(argp, &rose_callsign, sizeof(ax25_address)) ? -EFAULT : 0;

	case SIOCRSACCEPT:
		if (rose->state == ROSE_STATE_5) {
			rose_write_internal(sk, ROSE_CALL_ACCEPTED);
			rose_start_idletimer(sk);
			rose->condition = 0x00;
			rose->vs        = 0;
			rose->va        = 0;
			rose->vr        = 0;
			rose->vl        = 0;
			rose->state     = ROSE_STATE_3;
		}
		return 0;

	default:
		return -ENOIOCTLCMD;
	}

	return 0;
}