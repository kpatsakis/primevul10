static void bitmapUnitTest() {
  u_int32_t val, i, j;

  for(i=0; i<32; i++) {
    NDPI_ZERO_BIT(val);
    NDPI_SET_BIT(val, i);
    
    assert(NDPI_ISSET_BIT(val, i));
    
    for(j=0; j<32; j++) {
      if(j != i) {
	assert(!NDPI_ISSET_BIT(val, j));
      }
    }
  }
}