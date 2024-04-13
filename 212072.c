  bool isPrivate() {
    assertx(m_key);
    switch (EVP_PKEY_id(m_key)) {
#ifndef NO_RSA
    case EVP_PKEY_RSA:
    case EVP_PKEY_RSA2:
      {
        const auto rsa = EVP_PKEY_get0_RSA(m_key);
        assertx(rsa);
        const BIGNUM *p, *q;
        RSA_get0_factors(rsa, &p, &q);
        if (!p || !q) {
          return false;
        }
        break;
      }
#endif
#ifndef NO_DSA
    case EVP_PKEY_DSA:
    case EVP_PKEY_DSA1:
    case EVP_PKEY_DSA2:
    case EVP_PKEY_DSA3:
    case EVP_PKEY_DSA4:
      {
        const auto dsa = EVP_PKEY_get0_DSA(m_key);
        assertx(dsa);
        const BIGNUM *p, *q, *g, *pub_key, *priv_key;
        DSA_get0_pqg(dsa, &p, &q, &g);
        if (!p || !q || !g) {
          return false;
        }
        DSA_get0_key(dsa, &pub_key, &priv_key);
        if (!priv_key) {
          return false;
        }
        break;
      }
#endif
#ifndef NO_DH
    case EVP_PKEY_DH:
      {
        const auto dh = EVP_PKEY_get0_DH(m_key);
        assertx(dh);
        const BIGNUM *p, *q, *g, *pub_key, *priv_key;
        DH_get0_pqg(dh, &p, &q, &g);
        if (!p) {
          return false;
        }
        DH_get0_key(dh, &pub_key, &priv_key);
        if (!priv_key) {
          return false;
        }
        break;
       }
#endif
#ifdef HAVE_EVP_PKEY_EC
    case EVP_PKEY_EC:
      {
        const auto ec_key = EVP_PKEY_get0_EC_KEY(m_key);
        assertx(ec_key);
        if (EC_KEY_get0_private_key(ec_key) == nullptr) {
          return false;
        }
        break;
      }
#endif
    default:
      raise_warning("key type not supported in this PHP build!");
      break;
    }
    return true;
  }