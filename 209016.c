BOOL equal_md5(unsigned char m1[16], unsigned char m2[16]) {
   int i;
   for (i = 0; i < 16; i++)
      if (m1[i] != m2[i])
         break;

   return i == 16;
}