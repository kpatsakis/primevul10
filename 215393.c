inline void klinux_sigaddset(klinux_sigset_t *klinux_set, int klinux_sig) {
  uint64_t sig = klinux_sig - 1;
  klinux_set->klinux_val[0] |= 1UL << sig;
}