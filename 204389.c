_eddsa_verify (const struct ecc_curve *ecc,
	       const struct ecc_eddsa *eddsa,
	       const uint8_t *pub,
	       const mp_limb_t *A,
	       void *ctx,
	       size_t length,
	       const uint8_t *msg,
	       const uint8_t *signature,
	       mp_limb_t *scratch)
{
  size_t nbytes;
#define R scratch
#define sp (scratch + 2*ecc->p.size)
#define hp (scratch + 3*ecc->p.size)
#define P (scratch + 5*ecc->p.size)
#define scratch_out (scratch + 8*ecc->p.size)
#define S R
#define hash ((uint8_t *) P)

  nbytes = 1 + ecc->p.bit_size / 8;

  /* Could maybe save some storage by delaying the R and S operations,
     but it makes sense to check them for validity up front. */
  if (!_eddsa_decompress (ecc, R, signature, R+2*ecc->p.size))
    return 0;

  mpn_set_base256_le (sp, ecc->q.size, signature + nbytes, nbytes);
  /* Check that s < q */
  if (mpn_cmp (sp, ecc->q.m, ecc->q.size) >= 0)
    return 0;

  eddsa->dom (ctx);
  eddsa->update (ctx, nbytes, signature);
  eddsa->update (ctx, nbytes, pub);
  eddsa->update (ctx, length, msg);
  eddsa->digest (ctx, 2*nbytes, hash);
  _eddsa_hash (&ecc->q, hp, 2*nbytes, hash);

  /* Compute h A + R - s G, which should be the neutral point */
  ecc->mul (ecc, P, hp, A, scratch_out);
  ecc->add_hh (ecc, P, P, R, scratch_out);
  /* Move out of the way. */
  mpn_copyi (hp, sp, ecc->q.size);
  ecc->mul_g (ecc, S, hp, scratch_out);

  return equal_h (&ecc->p,
		   P, P + 2*ecc->p.size,
		   S, S + 2*ecc->p.size, scratch_out)
    && equal_h (&ecc->p,
		P + ecc->p.size, P + 2*ecc->p.size,
		S + ecc->p.size, S + 2*ecc->p.size, scratch_out);

#undef R
#undef sp
#undef hp
#undef P
#undef S
}