void serializerUnitTest() {
  ndpi_serializer serializer, deserializer;
  int i;
  u_int8_t trace = 0;

  assert(ndpi_init_serializer(&serializer, ndpi_serialization_format_tlv) != -1);

  for(i=0; i<16; i++) {
    char kbuf[32], vbuf[32];
    assert(ndpi_serialize_uint32_uint32(&serializer, i, i*i) != -1);

    snprintf(kbuf, sizeof(kbuf), "Hello %d", i);
    snprintf(vbuf, sizeof(vbuf), "World %d", i);
    assert(ndpi_serialize_uint32_string(&serializer, i, "Hello") != -1);
    assert(ndpi_serialize_string_string(&serializer, kbuf, vbuf) != -1);
    assert(ndpi_serialize_string_uint32(&serializer, kbuf, i*i) != -1);
    assert(ndpi_serialize_string_float(&serializer,  kbuf, (float)(i*i), "%f") != -1);
  }

  if(trace)
    printf("Serialization size: %u\n", ndpi_serializer_get_buffer_len(&serializer));

  assert(ndpi_init_deserializer(&deserializer, &serializer) != -1);

  while(1) {
    ndpi_serialization_type kt, et;
    et = ndpi_deserialize_get_item_type(&deserializer, &kt);

    if(et == ndpi_serialization_unknown)
      break;
    else {
      u_int32_t k32, v32;
      ndpi_string ks, vs;
      float vf;

      switch(kt) {
        case ndpi_serialization_uint32:
          ndpi_deserialize_key_uint32(&deserializer, &k32);
	  if(trace) printf("%u=", k32);
          break;
        case ndpi_serialization_string:
          ndpi_deserialize_key_string(&deserializer, &ks);
          if (trace) {
	    u_int8_t bkp = ks.str[ks.str_len];
	    ks.str[ks.str_len] = '\0';
            printf("%s=", ks.str);
	    ks.str[ks.str_len] = bkp;
          }
          break;
        default:
          printf("Unsupported TLV key type %u\n", kt);
          return;
      }

      switch(et) {
      case ndpi_serialization_uint32:
	assert(ndpi_deserialize_value_uint32(&deserializer, &v32) != -1);
	if(trace) printf("%u\n", v32);
	break;

      case ndpi_serialization_string:
	assert(ndpi_deserialize_value_string(&deserializer, &vs) != -1);
	if(trace) {
	  u_int8_t bkp = vs.str[vs.str_len];
	  vs.str[vs.str_len] = '\0';
	  printf("%s\n", vs.str);
	  vs.str[vs.str_len] = bkp;
	}
	break;

      case ndpi_serialization_float:
	assert(ndpi_deserialize_value_float(&deserializer, &vf) != -1);
	if(trace) printf("%f\n", vf);
	break;

      default:
	if (trace) printf("\n");
        printf("serializerUnitTest: unsupported type %u detected!\n", et);
        return;
	break;
      }
    }

    ndpi_deserialize_next(&deserializer);
  }

  ndpi_term_serializer(&serializer);
}