static int detect_datev(
	sc_pkcs15_card_t *p15card
){
	if(insert_cert(p15card,"3000C500", 0x45, 0, "Signatur Zertifikat")) return 1;
	set_string(&p15card->tokeninfo->manufacturer_id, "DATEV");
	set_string(&p15card->tokeninfo->label, "DATEV Classic");

	insert_cert(p15card,"DF02C200", 0x46, 0, "Verschluesselungs Zertifikat");
	insert_cert(p15card,"DF02C500", 0x47, 0, "Authentifizierungs Zertifikat");

	insert_key(p15card,"30005371", 0x45, 0x82, 1024, 1, "Signatur Schluessel");
	insert_key(p15card,"DF0253B1", 0x46, 0x81, 1024, 1, "Verschluesselungs Schluessel");
	insert_key(p15card,"DF025371", 0x47, 0x82, 1024, 1, "Authentifizierungs Schluessel");

	insert_pin(p15card,"5001", 1, 0, 0x01, 6, "PIN",
		SC_PKCS15_PIN_FLAG_CASE_SENSITIVE | SC_PKCS15_PIN_FLAG_INITIALIZED
	);

	return 0;
}