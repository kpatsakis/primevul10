_eddsa_verify_itch (const struct ecc_curve *ecc)
{
  assert (_eddsa_decompress_itch (ecc) <= ecc->mul_itch);
  return 8*ecc->p.size + ecc->mul_itch;
}