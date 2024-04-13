ecc_gostdsa_verify_itch (const struct ecc_curve *ecc)
{
  /* Largest storage need is for the ecc->mul call. */
  return 5*ecc->p.size + ecc->mul_itch;
}