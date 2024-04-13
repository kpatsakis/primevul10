int sqlcipher_codec_pragma(sqlite3* db, int iDb, Parse *pParse, const char *zLeft, const char *zRight) {
  struct Db *pDb = &db->aDb[iDb];
  codec_ctx *ctx = NULL;
  int rc;

  if(pDb->pBt) {
    ctx = (codec_ctx*) sqlite3PagerGetCodec(pDb->pBt->pBt->pPager);
  }

  CODEC_TRACE("sqlcipher_codec_pragma: entered db=%p iDb=%d pParse=%p zLeft=%s zRight=%s ctx=%p\n", db, iDb, pParse, zLeft, zRight, ctx);
  
#ifdef SQLCIPHER_EXT
  if( sqlite3StrICmp(zLeft, "cipher_license")==0 && zRight ){
    char *license_result = sqlite3_mprintf("%d", sqlcipher_license_key(zRight));
    codec_vdbe_return_string(pParse, "cipher_license", license_result, P4_DYNAMIC);
  } else
    if( sqlite3StrICmp(zLeft, "cipher_license")==0 && !zRight ){
      if(ctx) {
        char *license_result = sqlite3_mprintf("%d", ctx
                                               ? sqlcipher_license_key_status(ctx->provider)
                                               : SQLITE_ERROR);
        codec_vdbe_return_string(pParse, "cipher_license", license_result, P4_DYNAMIC);
      }
  } else
#endif
#ifdef SQLCIPHER_TEST
  if( sqlite3StrICmp(zLeft,"cipher_fail_next_encrypt")==0 ){
    if( zRight ) {
      cipher_fail_next_encrypt = sqlite3GetBoolean(zRight,1);
    } else {
      char *fail = sqlite3_mprintf("%d", cipher_fail_next_encrypt);
      codec_vdbe_return_string(pParse, "cipher_fail_next_encrypt", fail, P4_DYNAMIC);
    }
  }else
  if( sqlite3StrICmp(zLeft,"cipher_fail_next_decrypt")==0 ){
    if( zRight ) {
      cipher_fail_next_decrypt = sqlite3GetBoolean(zRight,1);
    } else {
      char *fail = sqlite3_mprintf("%d", cipher_fail_next_decrypt);
      codec_vdbe_return_string(pParse, "cipher_fail_next_decrypt", fail, P4_DYNAMIC);
    }
  }else
#endif
  if( sqlite3StrICmp(zLeft, "cipher_fips_status")== 0 && !zRight ){
    if(ctx) {
      char *fips_mode_status = sqlite3_mprintf("%d", sqlcipher_codec_fips_status(ctx));
      codec_vdbe_return_string(pParse, "cipher_fips_status", fips_mode_status, P4_DYNAMIC);
    }
  } else
  if( sqlite3StrICmp(zLeft, "cipher_store_pass")==0 && zRight ) {
    if(ctx) {
      char *deprecation = "PRAGMA cipher_store_pass is deprecated, please remove from use";
      sqlcipher_codec_set_store_pass(ctx, sqlite3GetBoolean(zRight, 1));
      codec_vdbe_return_string(pParse, "cipher_store_pass", deprecation, P4_TRANSIENT);
      sqlite3_log(SQLITE_WARNING, deprecation);
    }
  } else
  if( sqlite3StrICmp(zLeft, "cipher_store_pass")==0 && !zRight ) {
    if(ctx){
      char *store_pass_value = sqlite3_mprintf("%d", sqlcipher_codec_get_store_pass(ctx));
      codec_vdbe_return_string(pParse, "cipher_store_pass", store_pass_value, P4_DYNAMIC);
    }
  }
  if( sqlite3StrICmp(zLeft, "cipher_profile")== 0 && zRight ){
      char *profile_status = sqlite3_mprintf("%d", sqlcipher_cipher_profile(db, zRight));
      codec_vdbe_return_string(pParse, "cipher_profile", profile_status, P4_DYNAMIC);
  } else
  if( sqlite3StrICmp(zLeft, "cipher_add_random")==0 && zRight ){
    if(ctx) {
      char *add_random_status = sqlite3_mprintf("%d", sqlcipher_codec_add_random(ctx, zRight, sqlite3Strlen30(zRight)));
      codec_vdbe_return_string(pParse, "cipher_add_random", add_random_status, P4_DYNAMIC);
    }
  } else
  if( sqlite3StrICmp(zLeft, "cipher_migrate")==0 && !zRight ){
    if(ctx){
      char *migrate_status = sqlite3_mprintf("%d", sqlcipher_codec_ctx_migrate(ctx));
      codec_vdbe_return_string(pParse, "cipher_migrate", migrate_status, P4_DYNAMIC);
    }
  } else
  if( sqlite3StrICmp(zLeft, "cipher_provider")==0 && !zRight ){
    if(ctx) { codec_vdbe_return_string(pParse, "cipher_provider",
                                              sqlcipher_codec_get_cipher_provider(ctx), P4_TRANSIENT);
    }
  } else
  if( sqlite3StrICmp(zLeft, "cipher_provider_version")==0 && !zRight){
    if(ctx) { codec_vdbe_return_string(pParse, "cipher_provider_version",
                                              sqlcipher_codec_get_provider_version(ctx), P4_TRANSIENT);
    }
  } else
  if( sqlite3StrICmp(zLeft, "cipher_version")==0 && !zRight ){
    codec_vdbe_return_string(pParse, "cipher_version", sqlcipher_version(), P4_DYNAMIC);
  }else
  if( sqlite3StrICmp(zLeft, "cipher")==0 ){
    if(ctx) {
      if( zRight ) {
        const char* message = "PRAGMA cipher is no longer supported.";
        codec_vdbe_return_string(pParse, "cipher", message, P4_TRANSIENT);
        sqlite3_log(SQLITE_WARNING, message);
      }else {
        codec_vdbe_return_string(pParse, "cipher", sqlcipher_codec_ctx_get_cipher(ctx), P4_TRANSIENT); 
      }
    }
  }else
  if( sqlite3StrICmp(zLeft, "rekey_cipher")==0 && zRight ){
    const char* message = "PRAGMA rekey_cipher is no longer supported.";
    codec_vdbe_return_string(pParse, "rekey_cipher", message, P4_TRANSIENT);
    sqlite3_log(SQLITE_WARNING, message);
  }else
  if( sqlite3StrICmp(zLeft,"cipher_default_kdf_iter")==0 ){
    if( zRight ) {
      sqlcipher_set_default_kdf_iter(atoi(zRight)); /* change default KDF iterations */
    } else {
      char *kdf_iter = sqlite3_mprintf("%d", sqlcipher_get_default_kdf_iter());
      codec_vdbe_return_string(pParse, "cipher_default_kdf_iter", kdf_iter, P4_DYNAMIC);
    }
  }else
  if( sqlite3StrICmp(zLeft, "kdf_iter")==0 ){
    if(ctx) {
      if( zRight ) {
        sqlcipher_codec_ctx_set_kdf_iter(ctx, atoi(zRight)); /* change of RW PBKDF2 iteration */
      } else {
        char *kdf_iter = sqlite3_mprintf("%d", sqlcipher_codec_ctx_get_kdf_iter(ctx));
        codec_vdbe_return_string(pParse, "kdf_iter", kdf_iter, P4_DYNAMIC);
      }
    }
  }else
  if( sqlite3StrICmp(zLeft, "fast_kdf_iter")==0){
    if(ctx) {
      if( zRight ) {
        char *deprecation = "PRAGMA fast_kdf_iter is deprecated, please remove from use";
        sqlcipher_codec_ctx_set_fast_kdf_iter(ctx, atoi(zRight)); /* change of RW PBKDF2 iteration */
        codec_vdbe_return_string(pParse, "fast_kdf_iter", deprecation, P4_TRANSIENT);
        sqlite3_log(SQLITE_WARNING, deprecation);
      } else {
        char *fast_kdf_iter = sqlite3_mprintf("%d", sqlcipher_codec_ctx_get_fast_kdf_iter(ctx));
        codec_vdbe_return_string(pParse, "fast_kdf_iter", fast_kdf_iter, P4_DYNAMIC);
      }
    }
  }else
  if( sqlite3StrICmp(zLeft, "rekey_kdf_iter")==0 && zRight ){
    const char* message = "PRAGMA rekey_kdf_iter is no longer supported.";
    codec_vdbe_return_string(pParse, "rekey_kdf_iter", message, P4_TRANSIENT);
    sqlite3_log(SQLITE_WARNING, message);
  }else
  if( sqlite3StrICmp(zLeft,"cipher_page_size")==0 ){
    if(ctx) {
      if( zRight ) {
        int size = atoi(zRight);
        rc = sqlcipher_codec_ctx_set_pagesize(ctx, size);
        if(rc != SQLITE_OK) sqlcipher_codec_ctx_set_error(ctx, rc);
        rc = codec_set_btree_to_codec_pagesize(db, pDb, ctx);
        if(rc != SQLITE_OK) sqlcipher_codec_ctx_set_error(ctx, rc);
      } else {
        char * page_size = sqlite3_mprintf("%d", sqlcipher_codec_ctx_get_pagesize(ctx));
        codec_vdbe_return_string(pParse, "cipher_page_size", page_size, P4_DYNAMIC);
      }
    }
  }else
  if( sqlite3StrICmp(zLeft,"cipher_default_page_size")==0 ){
    if( zRight ) {
      sqlcipher_set_default_pagesize(atoi(zRight));
    } else {
      char *default_page_size = sqlite3_mprintf("%d", sqlcipher_get_default_pagesize());
      codec_vdbe_return_string(pParse, "cipher_default_page_size", default_page_size, P4_DYNAMIC);
    }
  }else
  if( sqlite3StrICmp(zLeft,"cipher_default_use_hmac")==0 ){
    if( zRight ) {
      sqlcipher_set_default_use_hmac(sqlite3GetBoolean(zRight,1));
    } else {
      char *default_use_hmac = sqlite3_mprintf("%d", sqlcipher_get_default_use_hmac());
      codec_vdbe_return_string(pParse, "cipher_default_use_hmac", default_use_hmac, P4_DYNAMIC);
    }
  }else
  if( sqlite3StrICmp(zLeft,"cipher_use_hmac")==0 ){
    if(ctx) {
      if( zRight ) {
        rc = sqlcipher_codec_ctx_set_use_hmac(ctx, sqlite3GetBoolean(zRight,1));
        if(rc != SQLITE_OK) sqlcipher_codec_ctx_set_error(ctx, rc);
        /* since the use of hmac has changed, the page size may also change */
        rc = codec_set_btree_to_codec_pagesize(db, pDb, ctx);
        if(rc != SQLITE_OK) sqlcipher_codec_ctx_set_error(ctx, rc);
      } else {
        char *hmac_flag = sqlite3_mprintf("%d", sqlcipher_codec_ctx_get_use_hmac(ctx));
        codec_vdbe_return_string(pParse, "cipher_use_hmac", hmac_flag, P4_DYNAMIC);
      }
    }
  }else
  if( sqlite3StrICmp(zLeft,"cipher_hmac_pgno")==0 ){
    if(ctx) {
      if(zRight) {
        char *deprecation = "PRAGMA cipher_hmac_pgno is deprecated, please remove from use";
        /* clear both pgno endian flags */
        if(sqlite3StrICmp(zRight, "le") == 0) {
          sqlcipher_codec_ctx_unset_flag(ctx, CIPHER_FLAG_BE_PGNO);
          sqlcipher_codec_ctx_set_flag(ctx, CIPHER_FLAG_LE_PGNO);
        } else if(sqlite3StrICmp(zRight, "be") == 0) {
          sqlcipher_codec_ctx_unset_flag(ctx, CIPHER_FLAG_LE_PGNO);
          sqlcipher_codec_ctx_set_flag(ctx, CIPHER_FLAG_BE_PGNO);
        } else if(sqlite3StrICmp(zRight, "native") == 0) {
          sqlcipher_codec_ctx_unset_flag(ctx, CIPHER_FLAG_LE_PGNO);
          sqlcipher_codec_ctx_unset_flag(ctx, CIPHER_FLAG_BE_PGNO);
        }
        codec_vdbe_return_string(pParse, "cipher_hmac_pgno", deprecation, P4_TRANSIENT);
        sqlite3_log(SQLITE_WARNING, deprecation);
 
      } else {
        if(sqlcipher_codec_ctx_get_flag(ctx, CIPHER_FLAG_LE_PGNO)) {
          codec_vdbe_return_string(pParse, "cipher_hmac_pgno", "le", P4_TRANSIENT);
        } else if(sqlcipher_codec_ctx_get_flag(ctx, CIPHER_FLAG_BE_PGNO)) {
          codec_vdbe_return_string(pParse, "cipher_hmac_pgno", "be", P4_TRANSIENT);
        } else {
          codec_vdbe_return_string(pParse, "cipher_hmac_pgno", "native", P4_TRANSIENT);
        }
      }
    }
  }else
  if( sqlite3StrICmp(zLeft,"cipher_hmac_salt_mask")==0 ){
    if(ctx) {
      if(zRight) {
        char *deprecation = "PRAGMA cipher_hmac_salt_mask is deprecated, please remove from use";
        if (sqlite3StrNICmp(zRight ,"x'", 2) == 0 && sqlite3Strlen30(zRight) == 5) {
          unsigned char mask = 0;
          const unsigned char *hex = (const unsigned char *)zRight+2;
          cipher_hex2bin(hex,2,&mask);
          sqlcipher_set_hmac_salt_mask(mask);
        }
        codec_vdbe_return_string(pParse, "cipher_hmac_salt_mask", deprecation, P4_TRANSIENT);
        sqlite3_log(SQLITE_WARNING, deprecation);
      } else {
        char *hmac_salt_mask = sqlite3_mprintf("%02x", sqlcipher_get_hmac_salt_mask());
        codec_vdbe_return_string(pParse, "cipher_hmac_salt_mask", hmac_salt_mask, P4_DYNAMIC);
      }
    }
  }else 
  if( sqlite3StrICmp(zLeft,"cipher_plaintext_header_size")==0 ){
    if(ctx) {
      if( zRight ) {
        int size = atoi(zRight);
        /* deliberately ignore result code, if size is invalid it will be set to -1
           and trip the error later in the codec */
        sqlcipher_codec_ctx_set_plaintext_header_size(ctx, size);
      } else {
        char *size = sqlite3_mprintf("%d", sqlcipher_codec_ctx_get_plaintext_header_size(ctx));
        codec_vdbe_return_string(pParse, "cipher_plaintext_header_size", size, P4_DYNAMIC);
      }
    }
  }else 
  if( sqlite3StrICmp(zLeft,"cipher_default_plaintext_header_size")==0 ){
    if( zRight ) {
      sqlcipher_set_default_plaintext_header_size(atoi(zRight));
    } else {
      char *size = sqlite3_mprintf("%d", sqlcipher_get_default_plaintext_header_size());
      codec_vdbe_return_string(pParse, "cipher_default_plaintext_header_size", size, P4_DYNAMIC);
    }
  }else
  if( sqlite3StrICmp(zLeft,"cipher_salt")==0 ){
    if(ctx) {
      if(zRight) {
        if (sqlite3StrNICmp(zRight ,"x'", 2) == 0 && sqlite3Strlen30(zRight) == (FILE_HEADER_SZ*2)+3) {
          unsigned char *salt = (unsigned char*) sqlite3_malloc(FILE_HEADER_SZ);
          const unsigned char *hex = (const unsigned char *)zRight+2;
          cipher_hex2bin(hex,FILE_HEADER_SZ*2,salt);
          sqlcipher_codec_ctx_set_kdf_salt(ctx, salt, FILE_HEADER_SZ);
          sqlite3_free(salt);
        }
      } else {
        void *salt;
        char *hexsalt = (char*) sqlite3_malloc((FILE_HEADER_SZ*2)+1);
        if((rc = sqlcipher_codec_ctx_get_kdf_salt(ctx, &salt)) == SQLITE_OK) {
          cipher_bin2hex(salt, FILE_HEADER_SZ, hexsalt);
          codec_vdbe_return_string(pParse, "cipher_salt", hexsalt, P4_DYNAMIC);
        } else {
          sqlite3_free(hexsalt);
          sqlcipher_codec_ctx_set_error(ctx, rc);
        }
      }
    }
  }else
  if( sqlite3StrICmp(zLeft,"cipher_hmac_algorithm")==0 ){
    if(ctx) {
      if(zRight) {
        rc = SQLITE_ERROR;
        if(sqlite3StrICmp(zRight, SQLCIPHER_HMAC_SHA1_LABEL) == 0) {
          rc = sqlcipher_codec_ctx_set_hmac_algorithm(ctx, SQLCIPHER_HMAC_SHA1);
        } else if(sqlite3StrICmp(zRight, SQLCIPHER_HMAC_SHA256_LABEL) == 0) {
          rc = sqlcipher_codec_ctx_set_hmac_algorithm(ctx, SQLCIPHER_HMAC_SHA256);
        } else if(sqlite3StrICmp(zRight, SQLCIPHER_HMAC_SHA512_LABEL) == 0) {
          rc = sqlcipher_codec_ctx_set_hmac_algorithm(ctx, SQLCIPHER_HMAC_SHA512);
        }
        if (rc != SQLITE_OK) sqlcipher_codec_ctx_set_error(ctx, SQLITE_ERROR);
        rc = codec_set_btree_to_codec_pagesize(db, pDb, ctx);
        if (rc != SQLITE_OK) sqlcipher_codec_ctx_set_error(ctx, SQLITE_ERROR);
      } else {
        int algorithm = sqlcipher_codec_ctx_get_hmac_algorithm(ctx);
        if(algorithm == SQLCIPHER_HMAC_SHA1) {
          codec_vdbe_return_string(pParse, "cipher_hmac_algorithm", SQLCIPHER_HMAC_SHA1_LABEL, P4_TRANSIENT);
        } else if(algorithm == SQLCIPHER_HMAC_SHA256) {
          codec_vdbe_return_string(pParse, "cipher_hmac_algorithm", SQLCIPHER_HMAC_SHA256_LABEL, P4_TRANSIENT);
        } else if(algorithm == SQLCIPHER_HMAC_SHA512) {
          codec_vdbe_return_string(pParse, "cipher_hmac_algorithm", SQLCIPHER_HMAC_SHA512_LABEL, P4_TRANSIENT);
        }
      }
    }
  }else 
  if( sqlite3StrICmp(zLeft,"cipher_default_hmac_algorithm")==0 ){
    if(zRight) {
      rc = SQLITE_ERROR;
      if(sqlite3StrICmp(zRight, SQLCIPHER_HMAC_SHA1_LABEL) == 0) {
        rc = sqlcipher_set_default_hmac_algorithm(SQLCIPHER_HMAC_SHA1);
      } else if(sqlite3StrICmp(zRight, SQLCIPHER_HMAC_SHA256_LABEL) == 0) {
        rc = sqlcipher_set_default_hmac_algorithm(SQLCIPHER_HMAC_SHA256);
      } else if(sqlite3StrICmp(zRight, SQLCIPHER_HMAC_SHA512_LABEL) == 0) {
        rc = sqlcipher_set_default_hmac_algorithm(SQLCIPHER_HMAC_SHA512);
      }
    } else {
      int algorithm = sqlcipher_get_default_hmac_algorithm();
      if(algorithm == SQLCIPHER_HMAC_SHA1) {
        codec_vdbe_return_string(pParse, "cipher_default_hmac_algorithm", SQLCIPHER_HMAC_SHA1_LABEL, P4_TRANSIENT);
      } else if(algorithm == SQLCIPHER_HMAC_SHA256) {
        codec_vdbe_return_string(pParse, "cipher_default_hmac_algorithm", SQLCIPHER_HMAC_SHA256_LABEL, P4_TRANSIENT);
      } else if(algorithm == SQLCIPHER_HMAC_SHA512) {
        codec_vdbe_return_string(pParse, "cipher_default_hmac_algorithm", SQLCIPHER_HMAC_SHA512_LABEL, P4_TRANSIENT);
      }
    }
  }else 
  if( sqlite3StrICmp(zLeft,"cipher_kdf_algorithm")==0 ){
    if(ctx) {
      if(zRight) {
        rc = SQLITE_ERROR;
        if(sqlite3StrICmp(zRight, SQLCIPHER_PBKDF2_HMAC_SHA1_LABEL) == 0) {
          rc = sqlcipher_codec_ctx_set_kdf_algorithm(ctx, SQLCIPHER_PBKDF2_HMAC_SHA1);
        } else if(sqlite3StrICmp(zRight, SQLCIPHER_PBKDF2_HMAC_SHA256_LABEL) == 0) {
          rc = sqlcipher_codec_ctx_set_kdf_algorithm(ctx, SQLCIPHER_PBKDF2_HMAC_SHA256);
        } else if(sqlite3StrICmp(zRight, SQLCIPHER_PBKDF2_HMAC_SHA512_LABEL) == 0) {
          rc = sqlcipher_codec_ctx_set_kdf_algorithm(ctx, SQLCIPHER_PBKDF2_HMAC_SHA512);
        }
        if (rc != SQLITE_OK) sqlcipher_codec_ctx_set_error(ctx, SQLITE_ERROR);
      } else {
        int algorithm = sqlcipher_codec_ctx_get_kdf_algorithm(ctx);
        if(algorithm == SQLCIPHER_PBKDF2_HMAC_SHA1) {
          codec_vdbe_return_string(pParse, "cipher_kdf_algorithm", SQLCIPHER_PBKDF2_HMAC_SHA1_LABEL, P4_TRANSIENT);
        } else if(algorithm == SQLCIPHER_PBKDF2_HMAC_SHA256) {
          codec_vdbe_return_string(pParse, "cipher_kdf_algorithm", SQLCIPHER_PBKDF2_HMAC_SHA256_LABEL, P4_TRANSIENT);
        } else if(algorithm == SQLCIPHER_PBKDF2_HMAC_SHA512) {
          codec_vdbe_return_string(pParse, "cipher_kdf_algorithm", SQLCIPHER_PBKDF2_HMAC_SHA512_LABEL, P4_TRANSIENT);
        }
      }
    }
  }else 
  if( sqlite3StrICmp(zLeft,"cipher_default_kdf_algorithm")==0 ){
    if(zRight) {
      rc = SQLITE_ERROR;
      if(sqlite3StrICmp(zRight, SQLCIPHER_PBKDF2_HMAC_SHA1_LABEL) == 0) {
        rc = sqlcipher_set_default_kdf_algorithm(SQLCIPHER_PBKDF2_HMAC_SHA1);
      } else if(sqlite3StrICmp(zRight, SQLCIPHER_PBKDF2_HMAC_SHA256_LABEL) == 0) {
        rc = sqlcipher_set_default_kdf_algorithm(SQLCIPHER_PBKDF2_HMAC_SHA256);
      } else if(sqlite3StrICmp(zRight, SQLCIPHER_PBKDF2_HMAC_SHA512_LABEL) == 0) {
        rc = sqlcipher_set_default_kdf_algorithm(SQLCIPHER_PBKDF2_HMAC_SHA512);
      }
    } else {
      int algorithm = sqlcipher_get_default_kdf_algorithm();
      if(algorithm == SQLCIPHER_PBKDF2_HMAC_SHA1) {
        codec_vdbe_return_string(pParse, "cipher_default_kdf_algorithm", SQLCIPHER_PBKDF2_HMAC_SHA1_LABEL, P4_TRANSIENT);
      } else if(algorithm == SQLCIPHER_PBKDF2_HMAC_SHA256) {
        codec_vdbe_return_string(pParse, "cipher_default_kdf_algorithm", SQLCIPHER_PBKDF2_HMAC_SHA256_LABEL, P4_TRANSIENT);
      } else if(algorithm == SQLCIPHER_PBKDF2_HMAC_SHA512) {
        codec_vdbe_return_string(pParse, "cipher_default_kdf_algorithm", SQLCIPHER_PBKDF2_HMAC_SHA512_LABEL, P4_TRANSIENT);
      }
    }
  }else
  if( sqlite3StrICmp(zLeft,"cipher_compatibility")==0 ){
    if(ctx) {
      if(zRight) {
        int version = atoi(zRight); 

        switch(version) {
          case 1: 
            rc = sqlcipher_codec_ctx_set_pagesize(ctx, 1024);
            if (rc != SQLITE_OK) sqlcipher_codec_ctx_set_error(ctx, SQLITE_ERROR);
            rc = sqlcipher_codec_ctx_set_hmac_algorithm(ctx, SQLCIPHER_HMAC_SHA1);
            if (rc != SQLITE_OK) sqlcipher_codec_ctx_set_error(ctx, SQLITE_ERROR);
            rc = sqlcipher_codec_ctx_set_kdf_algorithm(ctx, SQLCIPHER_PBKDF2_HMAC_SHA1);
            if (rc != SQLITE_OK) sqlcipher_codec_ctx_set_error(ctx, SQLITE_ERROR);
            rc = sqlcipher_codec_ctx_set_kdf_iter(ctx, 4000); 
            if (rc != SQLITE_OK) sqlcipher_codec_ctx_set_error(ctx, SQLITE_ERROR);
            rc = sqlcipher_codec_ctx_set_use_hmac(ctx, 0);
            if (rc != SQLITE_OK) sqlcipher_codec_ctx_set_error(ctx, SQLITE_ERROR);
            break;

          case 2: 
            rc = sqlcipher_codec_ctx_set_pagesize(ctx, 1024);
            if (rc != SQLITE_OK) sqlcipher_codec_ctx_set_error(ctx, SQLITE_ERROR);
            rc = sqlcipher_codec_ctx_set_hmac_algorithm(ctx, SQLCIPHER_HMAC_SHA1);
            if (rc != SQLITE_OK) sqlcipher_codec_ctx_set_error(ctx, SQLITE_ERROR);
            rc = sqlcipher_codec_ctx_set_kdf_algorithm(ctx, SQLCIPHER_PBKDF2_HMAC_SHA1);
            if (rc != SQLITE_OK) sqlcipher_codec_ctx_set_error(ctx, SQLITE_ERROR);
            rc = sqlcipher_codec_ctx_set_kdf_iter(ctx, 4000); 
            if (rc != SQLITE_OK) sqlcipher_codec_ctx_set_error(ctx, SQLITE_ERROR);
            rc = sqlcipher_codec_ctx_set_use_hmac(ctx, 1);
            if (rc != SQLITE_OK) sqlcipher_codec_ctx_set_error(ctx, SQLITE_ERROR);
            break;

          case 3:
            rc = sqlcipher_codec_ctx_set_pagesize(ctx, 1024);
            if (rc != SQLITE_OK) sqlcipher_codec_ctx_set_error(ctx, SQLITE_ERROR);
            rc = sqlcipher_codec_ctx_set_hmac_algorithm(ctx, SQLCIPHER_HMAC_SHA1);
            if (rc != SQLITE_OK) sqlcipher_codec_ctx_set_error(ctx, SQLITE_ERROR);
            rc = sqlcipher_codec_ctx_set_kdf_algorithm(ctx, SQLCIPHER_PBKDF2_HMAC_SHA1);
            if (rc != SQLITE_OK) sqlcipher_codec_ctx_set_error(ctx, SQLITE_ERROR);
            rc = sqlcipher_codec_ctx_set_kdf_iter(ctx, 64000); 
            if (rc != SQLITE_OK) sqlcipher_codec_ctx_set_error(ctx, SQLITE_ERROR);
            rc = sqlcipher_codec_ctx_set_use_hmac(ctx, 1);
            if (rc != SQLITE_OK) sqlcipher_codec_ctx_set_error(ctx, SQLITE_ERROR);
            break;

          default:
            rc = sqlcipher_codec_ctx_set_pagesize(ctx, 4096);
            if (rc != SQLITE_OK) sqlcipher_codec_ctx_set_error(ctx, SQLITE_ERROR);
            rc = sqlcipher_codec_ctx_set_hmac_algorithm(ctx, SQLCIPHER_HMAC_SHA512);
            if (rc != SQLITE_OK) sqlcipher_codec_ctx_set_error(ctx, SQLITE_ERROR);
            rc = sqlcipher_codec_ctx_set_kdf_algorithm(ctx, SQLCIPHER_PBKDF2_HMAC_SHA512);
            if (rc != SQLITE_OK) sqlcipher_codec_ctx_set_error(ctx, SQLITE_ERROR);
            rc = sqlcipher_codec_ctx_set_kdf_iter(ctx, 256000); 
            if (rc != SQLITE_OK) sqlcipher_codec_ctx_set_error(ctx, SQLITE_ERROR);
            rc = sqlcipher_codec_ctx_set_use_hmac(ctx, 1);
            if (rc != SQLITE_OK) sqlcipher_codec_ctx_set_error(ctx, SQLITE_ERROR);
            break;
        }  

        rc = codec_set_btree_to_codec_pagesize(db, pDb, ctx);
        if (rc != SQLITE_OK) sqlcipher_codec_ctx_set_error(ctx, SQLITE_ERROR);
      } 
    }
  }else 
  if( sqlite3StrICmp(zLeft,"cipher_default_compatibility")==0 ){
    if(zRight) {
      int version = atoi(zRight); 
      switch(version) {
        case 1: 
          sqlcipher_set_default_pagesize(1024);
          sqlcipher_set_default_hmac_algorithm(SQLCIPHER_HMAC_SHA1);
          sqlcipher_set_default_kdf_algorithm(SQLCIPHER_PBKDF2_HMAC_SHA1);
          sqlcipher_set_default_kdf_iter(4000);
          sqlcipher_set_default_use_hmac(0);
          break;

        case 2: 
          sqlcipher_set_default_pagesize(1024);
          sqlcipher_set_default_hmac_algorithm(SQLCIPHER_HMAC_SHA1);
          sqlcipher_set_default_kdf_algorithm(SQLCIPHER_PBKDF2_HMAC_SHA1);
          sqlcipher_set_default_kdf_iter(4000);
          sqlcipher_set_default_use_hmac(1);
          break;

        case 3:
          sqlcipher_set_default_pagesize(1024);
          sqlcipher_set_default_hmac_algorithm(SQLCIPHER_HMAC_SHA1);
          sqlcipher_set_default_kdf_algorithm(SQLCIPHER_PBKDF2_HMAC_SHA1);
          sqlcipher_set_default_kdf_iter(64000);
          sqlcipher_set_default_use_hmac(1);
          break;

        default:
          sqlcipher_set_default_pagesize(4096);
          sqlcipher_set_default_hmac_algorithm(SQLCIPHER_HMAC_SHA512);
          sqlcipher_set_default_kdf_algorithm(SQLCIPHER_PBKDF2_HMAC_SHA512);
          sqlcipher_set_default_kdf_iter(256000);
          sqlcipher_set_default_use_hmac(1);
          break;
      }  
    } 
  }else 
  if( sqlite3StrICmp(zLeft,"cipher_memory_security")==0 ){
    if( zRight ) {
      sqlcipher_set_mem_security(sqlite3GetBoolean(zRight,1));
    } else {
      char *on = sqlite3_mprintf("%d", sqlcipher_get_mem_security());
      codec_vdbe_return_string(pParse, "cipher_memory_security", on, P4_DYNAMIC);
    }
  }else
  if( sqlite3StrICmp(zLeft,"cipher_settings")==0 ){
    if(ctx) {
      int algorithm;
      char *pragma;

      pragma = sqlite3_mprintf("PRAGMA kdf_iter = %d;", sqlcipher_codec_ctx_get_kdf_iter(ctx));
      codec_vdbe_return_string(pParse, "pragma", pragma, P4_DYNAMIC);

      pragma = sqlite3_mprintf("PRAGMA cipher_page_size = %d;", sqlcipher_codec_ctx_get_pagesize(ctx));
      codec_vdbe_return_string(pParse, "pragma", pragma, P4_DYNAMIC);

      pragma = sqlite3_mprintf("PRAGMA cipher_use_hmac = %d;", sqlcipher_codec_ctx_get_use_hmac(ctx));
      codec_vdbe_return_string(pParse, "pragma", pragma, P4_DYNAMIC);

      pragma = sqlite3_mprintf("PRAGMA cipher_plaintext_header_size = %d;", sqlcipher_codec_ctx_get_plaintext_header_size(ctx));
      codec_vdbe_return_string(pParse, "pragma", pragma, P4_DYNAMIC);

      algorithm = sqlcipher_codec_ctx_get_hmac_algorithm(ctx);
      pragma = NULL;
      if(algorithm == SQLCIPHER_HMAC_SHA1) {
        pragma = sqlite3_mprintf("PRAGMA cipher_hmac_algorithm = %s;", SQLCIPHER_HMAC_SHA1_LABEL);
      } else if(algorithm == SQLCIPHER_HMAC_SHA256) {
        pragma = sqlite3_mprintf("PRAGMA cipher_hmac_algorithm = %s;", SQLCIPHER_HMAC_SHA256_LABEL);
      } else if(algorithm == SQLCIPHER_HMAC_SHA512) {
        pragma = sqlite3_mprintf("PRAGMA cipher_hmac_algorithm = %s;", SQLCIPHER_HMAC_SHA512_LABEL);
      }
      codec_vdbe_return_string(pParse, "pragma", pragma, P4_DYNAMIC);

      algorithm = sqlcipher_codec_ctx_get_kdf_algorithm(ctx);
      pragma = NULL;
      if(algorithm == SQLCIPHER_PBKDF2_HMAC_SHA1) {
        pragma = sqlite3_mprintf("PRAGMA cipher_kdf_algorithm = %s;", SQLCIPHER_PBKDF2_HMAC_SHA1_LABEL);
      } else if(algorithm == SQLCIPHER_PBKDF2_HMAC_SHA256) {
        pragma = sqlite3_mprintf("PRAGMA cipher_kdf_algorithm = %s;", SQLCIPHER_PBKDF2_HMAC_SHA256_LABEL);
      } else if(algorithm == SQLCIPHER_PBKDF2_HMAC_SHA512) {
        pragma = sqlite3_mprintf("PRAGMA cipher_kdf_algorithm = %s;", SQLCIPHER_PBKDF2_HMAC_SHA512_LABEL);
      }
      codec_vdbe_return_string(pParse, "pragma", pragma, P4_DYNAMIC);

    }
  }else
  if( sqlite3StrICmp(zLeft,"cipher_default_settings")==0 ){
    int algorithm;
    char *pragma;

    pragma = sqlite3_mprintf("PRAGMA cipher_default_kdf_iter = %d;", sqlcipher_get_default_kdf_iter());
    codec_vdbe_return_string(pParse, "pragma", pragma, P4_DYNAMIC);

    pragma = sqlite3_mprintf("PRAGMA cipher_default_page_size = %d;", sqlcipher_get_default_pagesize());
    codec_vdbe_return_string(pParse, "pragma", pragma, P4_DYNAMIC);

    pragma = sqlite3_mprintf("PRAGMA cipher_default_use_hmac = %d;", sqlcipher_get_default_use_hmac());
    codec_vdbe_return_string(pParse, "pragma", pragma, P4_DYNAMIC);

    pragma = sqlite3_mprintf("PRAGMA cipher_default_plaintext_header_size = %d;", sqlcipher_get_default_plaintext_header_size());
    codec_vdbe_return_string(pParse, "pragma", pragma, P4_DYNAMIC);

    algorithm = sqlcipher_get_default_hmac_algorithm();
    pragma = NULL;
    if(algorithm == SQLCIPHER_HMAC_SHA1) {
      pragma = sqlite3_mprintf("PRAGMA cipher_default_hmac_algorithm = %s;", SQLCIPHER_HMAC_SHA1_LABEL);
    } else if(algorithm == SQLCIPHER_HMAC_SHA256) {
      pragma = sqlite3_mprintf("PRAGMA cipher_default_hmac_algorithm = %s;", SQLCIPHER_HMAC_SHA256_LABEL);
    } else if(algorithm == SQLCIPHER_HMAC_SHA512) {
      pragma = sqlite3_mprintf("PRAGMA cipher_default_hmac_algorithm = %s;", SQLCIPHER_HMAC_SHA512_LABEL);
    }
    codec_vdbe_return_string(pParse, "pragma", pragma, P4_DYNAMIC);

    algorithm = sqlcipher_get_default_kdf_algorithm();
    pragma = NULL;
    if(algorithm == SQLCIPHER_PBKDF2_HMAC_SHA1) {
      pragma = sqlite3_mprintf("PRAGMA cipher_default_kdf_algorithm = %s;", SQLCIPHER_PBKDF2_HMAC_SHA1_LABEL);
    } else if(algorithm == SQLCIPHER_PBKDF2_HMAC_SHA256) {
      pragma = sqlite3_mprintf("PRAGMA cipher_default_kdf_algorithm = %s;", SQLCIPHER_PBKDF2_HMAC_SHA256_LABEL);
    } else if(algorithm == SQLCIPHER_PBKDF2_HMAC_SHA512) {
      pragma = sqlite3_mprintf("PRAGMA cipher_default_kdf_algorithm = %s;", SQLCIPHER_PBKDF2_HMAC_SHA512_LABEL);
    }
    codec_vdbe_return_string(pParse, "pragma", pragma, P4_DYNAMIC);
  }else
  if( sqlite3StrICmp(zLeft,"cipher_integrity_check")==0 ){
    if(ctx) {
      sqlcipher_codec_ctx_integrity_check(ctx, pParse, "cipher_integrity_check");
    }
  }else {
    return 0;
  }
  return 1;
}