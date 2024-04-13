ecc_ecdsa_sign_itch (const struct ecc_curve *ecc)
{
  /* Needs 3*ecc->p.size + scratch for ecc->mul_g. Currently same for
     ecc_mul_g. */
  assert (ecc->p.size + ecc->p.invert_itch <= 3*ecc->p.size + ecc->mul_g_itch);
  return ECC_ECDSA_SIGN_ITCH (ecc->p.size);
}