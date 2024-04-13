static int parse_exthdrs(struct sk_buff *skb, const struct sadb_msg *hdr, void **ext_hdrs)
{
	const char *p = (char *) hdr;
	int len = skb->len;

	len -= sizeof(*hdr);
	p += sizeof(*hdr);
	while (len > 0) {
		const struct sadb_ext *ehdr = (const struct sadb_ext *) p;
		uint16_t ext_type;
		int ext_len;

		if (len < sizeof(*ehdr))
			return -EINVAL;

		ext_len  = ehdr->sadb_ext_len;
		ext_len *= sizeof(uint64_t);
		ext_type = ehdr->sadb_ext_type;
		if (ext_len < sizeof(uint64_t) ||
		    ext_len > len ||
		    ext_type == SADB_EXT_RESERVED)
			return -EINVAL;

		if (ext_type <= SADB_EXT_MAX) {
			int min = (int) sadb_ext_min_len[ext_type];
			if (ext_len < min)
				return -EINVAL;
			if (ext_hdrs[ext_type-1] != NULL)
				return -EINVAL;
			switch (ext_type) {
			case SADB_EXT_ADDRESS_SRC:
			case SADB_EXT_ADDRESS_DST:
			case SADB_EXT_ADDRESS_PROXY:
			case SADB_X_EXT_NAT_T_OA:
				if (verify_address_len(p))
					return -EINVAL;
				break;
			case SADB_X_EXT_SEC_CTX:
				if (verify_sec_ctx_len(p))
					return -EINVAL;
				break;
			case SADB_EXT_KEY_AUTH:
			case SADB_EXT_KEY_ENCRYPT:
				if (verify_key_len(p))
					return -EINVAL;
				break;
			default:
				break;
			}
			ext_hdrs[ext_type-1] = (void *) p;
		}
		p   += ext_len;
		len -= ext_len;
	}

	return 0;
}