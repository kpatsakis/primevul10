static int enough(int a, int b) {
  u_int8_t percentage = 20;

  if(b == 0) return(0);
  if(a == 0) return(1);

  if(b > (((a+1)*percentage)/100)) return(1);

  return(0);
}