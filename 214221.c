int DeliverSignal(int linux_signum, int linux_sigcode) {
  int signum = FromkLinuxSignalNumber(linux_signum);
  if (signum < 0) {
    return 1;
  }
  siginfo_t info;
  info.si_signo = signum;
  info.si_code = linux_sigcode;
  SignalManager *signal_manager = SignalManager::GetInstance();
  const sigset_t mask = signal_manager->GetSignalMask();

  // If the signal is blocked and still passed into the enclave. The signal
  // masks inside the enclave is out of sync with the untrusted signal mask.
  if (sigismember(&mask, signum)) {
    return -1;
  }
  signal_manager->HandleSignal(signum, &info, /*ucontext=*/nullptr);
  return 0;
}