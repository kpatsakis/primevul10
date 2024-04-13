void ethereum_signing_abort(void) {
  if (ethereum_signing) {
    memzero(privkey, sizeof(privkey));
    layoutHome();
    ethereum_signing = false;
  }
}