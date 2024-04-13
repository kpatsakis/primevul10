int RegisterSignalHandler(int signum,
                          void (*klinux_sigaction)(int, klinux_siginfo_t *,
                                                   void *),
                          const sigset_t mask, int flags) {
  // Never pass SA_SIGINFO through this layer. It's always appended to the flags
  // over in the untrusted layer.
  flags &= ~SA_SIGINFO;

  flags &= SA_NODEFER | SA_RESETHAND;

  absl::optional<int> klinux_signum = TokLinuxSignalNumber(signum);
  absl::optional<int> klinux_flags = TokLinuxSignalFlag(flags);
  if (!klinux_signum || !klinux_flags) {
    errno = EINVAL;
    return -1;
  }
  klinux_sigset_t klinux_mask;
  TokLinuxSigset(&mask, &klinux_mask);
  int ret;
  CHECK_OCALL(ocall_enc_untrusted_register_signal_handler(
      &ret, *klinux_signum, reinterpret_cast<void *>(klinux_sigaction),
      reinterpret_cast<void *>(&klinux_mask), sizeof(klinux_mask),
      *klinux_flags));
  return ret;
}