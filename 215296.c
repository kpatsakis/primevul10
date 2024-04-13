iasecc_sdo_key_rsa_put_data(struct sc_card *card, struct iasecc_sdo_rsa_update *update)
{
	struct sc_context *ctx = card->ctx;
	unsigned char scb;
	int rv;

	LOG_FUNC_CALLED(ctx);

	if (update->sdo_prv_key)   {
		sc_log(ctx, "encode private rsa in %p", &update->update_prv);
		rv = iasecc_sdo_encode_rsa_update(card->ctx, update->sdo_prv_key, update->p15_rsa, &update->update_prv);
		LOG_TEST_RET(ctx, rv, "failed to encode update of RSA private key");
	}

	if (update->sdo_pub_key)   {
		sc_log(ctx, "encode public rsa in %p", &update->update_pub);
		if (card->type == SC_CARD_TYPE_IASECC_SAGEM)   {
			if (update->sdo_pub_key->data.pub_key.cha.value)   {
				free(update->sdo_pub_key->data.pub_key.cha.value);
				memset(&update->sdo_pub_key->data.pub_key.cha, 0, sizeof(update->sdo_pub_key->data.pub_key.cha));
			}
		}
		rv = iasecc_sdo_encode_rsa_update(card->ctx, update->sdo_pub_key, update->p15_rsa, &update->update_pub);
		LOG_TEST_RET(ctx, rv, "failed to encode update of RSA public key");
	}

	if (update->sdo_prv_key)   {
		sc_log(ctx, "reference of the private key to store: %X", update->sdo_prv_key->sdo_ref);

		if (update->sdo_prv_key->docp.acls_contact.size == 0)
			LOG_TEST_RET(ctx, SC_ERROR_INVALID_DATA, "extremely strange ... there are no ACLs");

		scb = update->sdo_prv_key->docp.scbs[IASECC_ACLS_RSAKEY_PUT_DATA];
		sc_log(ctx, "'UPDATE PRIVATE RSA' scb 0x%X", scb);

		do   {
			unsigned all_conditions = scb & IASECC_SCB_METHOD_NEED_ALL ? 1 : 0;

			if ((scb & IASECC_SCB_METHOD_USER_AUTH) && !all_conditions)
				break;

			if (scb & IASECC_SCB_METHOD_EXT_AUTH)
				LOG_TEST_RET(ctx, SC_ERROR_NOT_SUPPORTED, "Not yet");

			if (scb & IASECC_SCB_METHOD_SM)   {
#ifdef ENABLE_SM
				rv = iasecc_sm_rsa_update(card, scb & IASECC_SCB_METHOD_MASK_REF, update);
				LOG_FUNC_RETURN(ctx, rv);
#else
				LOG_TEST_RET(ctx, SC_ERROR_NOT_SUPPORTED, "built without support of Secure-Messaging");
#endif
			}
		} while(0);

		rv = iasecc_sdo_put_data(card, &update->update_prv);
		LOG_TEST_RET(ctx, rv, "failed to update of RSA private key");
	}

	if (update->sdo_pub_key)   {
		sc_log(ctx, "reference of the public key to store: %X", update->sdo_pub_key->sdo_ref);

		rv = iasecc_sdo_put_data(card, &update->update_pub);
		LOG_TEST_RET(ctx, rv, "failed to update of RSA public key");
	}

	LOG_FUNC_RETURN(ctx, SC_SUCCESS);
}