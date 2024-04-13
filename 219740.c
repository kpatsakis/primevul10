generate_hash_secret_salt(XML_Parser parser)
{
  unsigned long entropy;
  (void)parser;
#if defined(HAVE_ARC4RANDOM_BUF) || defined(__CloudABI__)
  (void)gather_time_entropy;
  arc4random_buf(&entropy, sizeof(entropy));
  return ENTROPY_DEBUG("arc4random_buf", entropy);
#else
  /* Try high quality providers first .. */
#ifdef _WIN32
  if (writeRandomBytes_RtlGenRandom((void *)&entropy, sizeof(entropy))) {
    return ENTROPY_DEBUG("RtlGenRandom", entropy);
  }
#elif defined(HAVE_GETRANDOM) || defined(HAVE_SYSCALL_GETRANDOM)
  if (writeRandomBytes_getrandom((void *)&entropy, sizeof(entropy))) {
    return ENTROPY_DEBUG("getrandom", entropy);
  }
#endif
  /* .. and self-made low quality for backup: */

  /* Process ID is 0 bits entropy if attacker has local access */
  entropy = gather_time_entropy() ^ getpid();

  /* Factors are 2^31-1 and 2^61-1 (Mersenne primes M31 and M61) */
  if (sizeof(unsigned long) == 4) {
    return ENTROPY_DEBUG("fallback(4)", entropy * 2147483647);
  } else {
    return ENTROPY_DEBUG("fallback(8)",
        entropy * (unsigned long)2305843009213693951);
  }
#endif
}