  static req::ptr<Key> GetHelper(const Variant& var, bool public_key,
                                 const char *passphrase) {
    req::ptr<Certificate> ocert;
    EVP_PKEY *key = nullptr;

    if (var.isResource()) {
      auto cert = dyn_cast_or_null<Certificate>(var);
      auto key = dyn_cast_or_null<Key>(var);
      if (!cert && !key) return nullptr;
      if (key) {
        bool is_priv = key->isPrivate();
        if (!public_key && !is_priv) {
          raise_warning("supplied key param is a public key");
          return nullptr;
        }
        if (public_key && is_priv) {
          raise_warning("Don't know how to get public key from "
                          "this private key");
          return nullptr;
        }
        return key;
      }
      ocert = cert;
    } else {
      /* it's an X509 file/cert of some kind, and we need to extract
         the data from that */
      if (public_key) {
        ocert = Certificate::Get(var);
        if (!ocert) {
          /* not a X509 certificate, try to retrieve public key */
          BIO *in = Certificate::ReadData(var);
          if (in == nullptr) return nullptr;
          key = PEM_read_bio_PUBKEY(in, nullptr,nullptr, nullptr);
          BIO_free(in);
        }
      } else {
        /* we want the private key */
        BIO *in = Certificate::ReadData(var);
        if (in == nullptr) return nullptr;
        key = PEM_read_bio_PrivateKey(in, nullptr,nullptr, (void*)passphrase);
        BIO_free(in);
      }
    }

    if (public_key && ocert && key == nullptr) {
      /* extract public key from X509 cert */
      key = (EVP_PKEY *)X509_get_pubkey(ocert->get());
    }

    if (key) {
      return req::make<Key>(key);
    }

    return nullptr;
  }