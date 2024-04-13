int RegisterSignalHandler(int signum,
                          void (*klinux_sigaction)(int, klinux_siginfo_t *,
                                                   void *),
                          const sigset_t mask, int flags) {
  int klinux_signum = TokLinuxSignalNumber(signum);
  if (klinux_signum < 0) {
    errno = EINVAL;
    return -1;
  }
  klinux_sigset_t klinux_mask;
  TokLinuxSigset(&mask, &klinux_mask);
  int ret;
  CHECK_OCALL(ocall_enc_untrusted_register_signal_handler(
      &ret, klinux_signum, reinterpret_cast<void *>(klinux_sigaction),
      reinterpret_cast<void *>(&klinux_mask), sizeof(klinux_mask),
      TokLinuxSignalFlag(flags)));
  return ret;
}