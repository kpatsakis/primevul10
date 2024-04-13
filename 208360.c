static void get_random_vector(unsigned char *vector)
{
#ifdef linux
	int fd = open("/dev/urandom",O_RDONLY); /* Linux: get *real* random numbers */
	int total = 0;
	if (fd >= 0) {
		while (total < AUTH_VECTOR_LEN) {
			int bytes = read(fd, vector + total, AUTH_VECTOR_LEN - total);
			if (bytes <= 0)
				break;			/* oops! Error */
			total += bytes;
		}
		close(fd);
	}

	if (total != AUTH_VECTOR_LEN)
#endif
	{				/* do this *always* on other platforms */
		MD5_CTX my_md5;
		struct timeval tv;
		struct timezone tz;
		static unsigned int session = 0; /* make the number harder to guess */

		/* Use the time of day with the best resolution the system can
	 	   give us -- often close to microsecond accuracy. */
		gettimeofday(&tv,&tz);

		if (session == 0) {
			session = getppid();	/* (possibly) hard to guess information */
		}

		tv.tv_sec ^= getpid() * session++;

		/* Hash things to get maybe cryptographically strong pseudo-random numbers */
		MD5Init(&my_md5);
		MD5Update(&my_md5, (unsigned char *) &tv, sizeof(tv));
		MD5Update(&my_md5, (unsigned char *) &tz, sizeof(tz));
		MD5Final(vector, &my_md5);				/* set the final vector */
	}
}