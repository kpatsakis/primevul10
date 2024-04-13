void CopyToFingerprint(const tstring& feature, uint64* dst) {
  *dst = Fingerprint64(feature);
}