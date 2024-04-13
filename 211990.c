  bool generatePrivateKey() {
    assertx(priv_key == nullptr);

    if (priv_key_bits < MIN_KEY_LENGTH) {
      raise_warning("private key length is too short; it needs to be "
                      "at least %d bits, not %d",
                      MIN_KEY_LENGTH, priv_key_bits);
      return false;
    }

    char *randfile = CONF_get_string(req_config, section_name, "RANDFILE");
    int egdsocket, seeded;
    load_rand_file(randfile, &egdsocket, &seeded);

    bool ret = false;
    if ((priv_key = EVP_PKEY_new()) != nullptr) {
      switch (priv_key_type) {
      case OPENSSL_KEYTYPE_RSA:
        if (EVP_PKEY_assign_RSA
            (priv_key, RSA_generate_key(priv_key_bits, 0x10001, nullptr, nullptr))) {
          ret = true;
        }
        break;
#if !defined(NO_DSA) && defined(HAVE_DSA_DEFAULT_METHOD)
      case OPENSSL_KEYTYPE_DSA:
        {
          DSA *dsapar = DSA_generate_parameters(priv_key_bits, nullptr, 0, nullptr,
                                                nullptr, nullptr, nullptr);
          if (dsapar) {
            DSA_set_method(dsapar, DSA_get_default_method());
            if (DSA_generate_key(dsapar)) {
              if (EVP_PKEY_assign_DSA(priv_key, dsapar)) {
                ret = true;
              }
            } else {
              DSA_free(dsapar);
            }
          }
        }
        break;
#endif
#ifdef HAVE_EVP_PKEY_EC
      case OPENSSL_KEYTYPE_EC:
        {
          if (curve_name == NID_undef) {
            raise_warning("Missing configuration value: 'curve_name' not set");
            return false;
          }
          if (auto const eckey = EC_KEY_new_by_curve_name(curve_name)) {
            EC_KEY_set_asn1_flag(eckey, OPENSSL_EC_NAMED_CURVE);
            if (EC_KEY_generate_key(eckey) &&
                EVP_PKEY_assign_EC_KEY(priv_key, eckey)) {
              ret = true;
            } else {
              EC_KEY_free(eckey);
            }
          }
        }
        break;
#endif
      default:
        raise_warning("Unsupported private key type");
      }
    }

    write_rand_file(randfile, egdsocket, seeded);
    return ret;
  }