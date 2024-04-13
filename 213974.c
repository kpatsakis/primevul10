static int insert_cert(
	sc_pkcs15_card_t *p15card,
	const char       *path,
	unsigned char     id,
	int               writable,
	const char       *label
){
	sc_card_t *card=p15card->card;
	sc_context_t *ctx=p15card->card->ctx;
	struct sc_pkcs15_cert_info cert_info;
	struct sc_pkcs15_object cert_obj;
	unsigned char cert[20];
	int r;

	memset(&cert_info, 0, sizeof(cert_info));
	cert_info.id.len      = 1;
	cert_info.id.value[0] = id;
	cert_info.authority   = 0;
	sc_format_path(path, &cert_info.path);

	memset(&cert_obj, 0, sizeof(cert_obj));
	strlcpy(cert_obj.label, label, sizeof(cert_obj.label));
	cert_obj.flags = writable ? SC_PKCS15_CO_FLAG_MODIFIABLE : 0;

	if(sc_select_file(card, &cert_info.path, NULL)!=SC_SUCCESS){
		sc_log(ctx, 
			"Select(%s) failed\n", path);
		return 1;
	}
	if(sc_read_binary(card, 0, cert, sizeof(cert), 0)<0){
		sc_log(ctx, 
			"ReadBinary(%s) failed\n", path);
		return 2;
	}
	if(cert[0]!=0x30 || cert[1]!=0x82){
		sc_log(ctx, 
			"Invalid Cert: %02X:%02X:...\n", cert[0], cert[1]);
		return 3;
	}

	/* some certificates are prefixed by an OID */
	if(cert[4]==0x06 && cert[5]<10 && cert[6+cert[5]]==0x30 && cert[7+cert[5]]==0x82){
		cert_info.path.index=6+cert[5];
		cert_info.path.count=(cert[8+cert[5]]<<8) + cert[9+cert[5]] + 4;
	} else {
		cert_info.path.index=0;
		cert_info.path.count=(cert[2]<<8) + cert[3] + 4;
	}

	r=sc_pkcs15emu_add_x509_cert(p15card, &cert_obj, &cert_info);
	if(r!=SC_SUCCESS){
		sc_log(ctx,  "sc_pkcs15emu_add_x509_cert(%s) failed\n", path);
		return 4;
	}
	sc_log(ctx,  "%s: OK, Index=%d, Count=%d\n", path, cert_info.path.index, cert_info.path.count);
	return 0;
}