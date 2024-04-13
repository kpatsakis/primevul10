static int detect_netkey(
	sc_pkcs15_card_t *p15card
){
	sc_card_t *card=p15card->card;
	sc_path_t p;
	sc_file_t *f;
	int keylen;
	char dir[10];
	const char *c_auth;

	/* NKS-Applikation ? */
	memset(&p, 0, sizeof(sc_path_t));
	p.type=SC_PATH_TYPE_DF_NAME;
	memcpy(p.value, "\xD2\x76\x00\x00\x03\x01\x02", p.len=7);
	if (sc_select_file(card,&p,&f)!=SC_SUCCESS) return 1;
	sprintf(dir,"%04X", f->id);
	sc_file_free(f);

	set_string(&p15card->tokeninfo->manufacturer_id, "TeleSec GmbH");
	set_string(&p15card->tokeninfo->label, card->type==SC_CARD_TYPE_TCOS_V3 ? "NetKey V3 Card" : "NetKey Card");
	keylen= card->type==SC_CARD_TYPE_TCOS_V3 ? 2048 : 1024;
	c_auth= card->type==SC_CARD_TYPE_TCOS_V3 ? "C500" : "C100";

	insert_cert(p15card, dirpath(dir,"4331"), 0x45, 1, "Signatur Zertifikat 1");
	insert_cert(p15card, dirpath(dir,"4332"), 0x45, 1, "Signatur Zertifikat 2");
	insert_cert(p15card, dirpath(dir,"C000"), 0x45, 0, "Telesec Signatur Zertifikat");
	insert_cert(p15card, dirpath(dir,"43B1"), 0x46, 1, "Verschluesselungs Zertifikat 1");
	insert_cert(p15card, dirpath(dir,"43B2"), 0x46, 1, "Verschluesselungs Zertifikat 2");
	insert_cert(p15card, dirpath(dir,"C200"), 0x46, 0, "Telesec Verschluesselungs Zertifikat");
	insert_cert(p15card, dirpath(dir,"4371"), 0x47, 1, "Authentifizierungs Zertifikat 1");
	insert_cert(p15card, dirpath(dir,"4372"), 0x47, 1, "Authentifizierungs Zertifikat 2");
	insert_cert(p15card, dirpath(dir,c_auth), 0x47, 0, "Telesec Authentifizierungs Zertifikat");
	insert_cert(p15card, dirpath(dir,"C201"), 0x48, 0, "Telesec 1024bit Zertifikat");

	insert_key(p15card, dirpath(dir,"5331"), 0x45, 0x80, keylen, 4, "Signatur Schluessel");
	insert_key(p15card, dirpath(dir,"53B1"), 0x46, 0x81, keylen, 3, "Verschluesselungs Schluessel");
	insert_key(p15card, dirpath(dir,"5371"), 0x47, 0x82, keylen, 3, "Authentifizierungs Schluessel");
	insert_key(p15card, dirpath(dir,"0000"), 0x48, 0x83, 1024,   3, "1024bit Schluessel");

	insert_pin(p15card, "5000", 1, 2, 0x00, 6, "PIN",
		SC_PKCS15_PIN_FLAG_CASE_SENSITIVE | SC_PKCS15_PIN_FLAG_INITIALIZED
	);
	insert_pin(p15card, "5001", 2, 0, 0x01, 8, "PUK",
		SC_PKCS15_PIN_FLAG_CASE_SENSITIVE | SC_PKCS15_PIN_FLAG_INITIALIZED |
		SC_PKCS15_PIN_FLAG_UNBLOCKING_PIN | SC_PKCS15_PIN_FLAG_SO_PIN
	);
	if(card->type==SC_CARD_TYPE_TCOS_V3){
		insert_pin(p15card, dirpath(dir,"0000"), 3, 1, 0x83, 6, "NetKey PIN2",
			SC_PKCS15_PIN_FLAG_CASE_SENSITIVE | SC_PKCS15_PIN_FLAG_LOCAL |
			SC_PKCS15_PIN_FLAG_INITIALIZED
		);
	} else {
		insert_pin(p15card, dirpath(dir,"5080"), 3, 1, 0x80, 6, "NetKey PIN0",
			SC_PKCS15_PIN_FLAG_CASE_SENSITIVE | SC_PKCS15_PIN_FLAG_LOCAL |
			SC_PKCS15_PIN_FLAG_INITIALIZED
		);
	}
	insert_pin(p15card, dirpath(dir,"5081"), 4, 1, 0x81, 6, "NetKey PIN1",
		SC_PKCS15_PIN_FLAG_CASE_SENSITIVE | SC_PKCS15_PIN_FLAG_LOCAL |
		SC_PKCS15_PIN_FLAG_INITIALIZED
	);

	/* SigG-Applikation */
	p.len=7; p.type=SC_PATH_TYPE_DF_NAME;
	memcpy(p.value, "\xD2\x76\x00\x00\x66\x01", p.len=6);
	if (sc_select_file(card,&p,&f)==SC_SUCCESS){
		sprintf(dir,"%04X", f->id);
		sc_file_free(f);

		insert_cert(p15card, dirpath(dir,"C000"), 0x49, 1, "SigG Zertifikat 1");
		insert_cert(p15card, dirpath(dir,"4331"), 0x49, 1, "SigG Zertifikat 2");
		insert_cert(p15card, dirpath(dir,"4332"), 0x49, 1, "SigG Zertifikat 3");
		
		if(card->type==SC_CARD_TYPE_TCOS_V3){
			insert_key(p15card, dirpath(dir,"0000"), 0x49, 0x84, 2048, 5, "SigG Schluessel");
		} else {
			insert_key(p15card, dirpath(dir,"5331"), 0x49, 0x80, 1024, 5, "SigG Schluessel");
		}

		insert_pin(p15card, dirpath(dir,"5081"), 5, 0, 0x81, 6, "SigG PIN",
			SC_PKCS15_PIN_FLAG_CASE_SENSITIVE | SC_PKCS15_PIN_FLAG_LOCAL |
			SC_PKCS15_PIN_FLAG_INITIALIZED
		);
		if(card->type==SC_CARD_TYPE_TCOS_V3){
			insert_pin(p15card, dirpath(dir,"0000"), 6, 0, 0x83, 8, "SigG PIN2",
				SC_PKCS15_PIN_FLAG_CASE_SENSITIVE | SC_PKCS15_PIN_FLAG_LOCAL |
				SC_PKCS15_PIN_FLAG_INITIALIZED
			);
		}
	}

	return 0;
}