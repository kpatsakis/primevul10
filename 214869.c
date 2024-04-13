static char* is_unsafe_cipher(ndpi_cipher_weakness c) {
  switch(c) {
  case ndpi_cipher_insecure:
    return("INSECURE");
    break;

  case ndpi_cipher_weak:
    return("WEAK");
    break;

  default:
    return("OK");
  }
}