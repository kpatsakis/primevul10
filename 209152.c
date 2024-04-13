void _MD5_transform(unsigned int state[4], unsigned char block[64]) {
   unsigned int lA = state[0], lB = state[1], lC = state[2], lD = state[3];
   unsigned int x[16];

   _MD5_decode(x, block, 64);

   /* round 1 */
   FF(lA, lB, lC, lD, x[0], 7, 0xd76aa478);     // 1
   FF(lD, lA, lB, lC, x[1], 12, 0xe8c7b756);    // 2
   FF(lC, lD, lA, lB, x[2], 17, 0x242070db);    // 3
   FF(lB, lC, lD, lA, x[3], 22, 0xc1bdceee);    // 4
   FF(lA, lB, lC, lD, x[4], 7, 0xf57c0faf);     // 5
   FF(lD, lA, lB, lC, x[5], 12, 0x4787c62a);    // 6
   FF(lC, lD, lA, lB, x[6], 17, 0xa8304613);    // 7
   FF(lB, lC, lD, lA, x[7], 22, 0xfd469501);    // 8
   FF(lA, lB, lC, lD, x[8], 7, 0x698098d8);     // 9
   FF(lD, lA, lB, lC, x[9], 12, 0x8b44f7af);    // 10
   FF(lC, lD, lA, lB, x[10], 17, 0xffff5bb1);   // 11
   FF(lB, lC, lD, lA, x[11], 22, 0x895cd7be);   // 12
   FF(lA, lB, lC, lD, x[12], 7, 0x6b901122);    // 13
   FF(lD, lA, lB, lC, x[13], 12, 0xfd987193);   // 14
   FF(lC, lD, lA, lB, x[14], 17, 0xa679438e);   // 15
   FF(lB, lC, lD, lA, x[15], 22, 0x49b40821);   // 16

   /* round 2 */
   GG(lA, lB, lC, lD, x[1], 5, 0xf61e2562);     // 17
   GG(lD, lA, lB, lC, x[6], 9, 0xc040b340);     // 18
   GG(lC, lD, lA, lB, x[11], 14, 0x265e5a51);   // 19
   GG(lB, lC, lD, lA, x[0], 20, 0xe9b6c7aa);    // 20
   GG(lA, lB, lC, lD, x[5], 5, 0xd62f105d);     // 21
   GG(lD, lA, lB, lC, x[10], 9, 0x2441453);     // 22
   GG(lC, lD, lA, lB, x[15], 14, 0xd8a1e681);   // 23
   GG(lB, lC, lD, lA, x[4], 20, 0xe7d3fbc8);    // 24
   GG(lA, lB, lC, lD, x[9], 5, 0x21e1cde6);     // 25
   GG(lD, lA, lB, lC, x[14], 9, 0xc33707d6);    // 26
   GG(lC, lD, lA, lB, x[3], 14, 0xf4d50d87);    // 27
   GG(lB, lC, lD, lA, x[8], 20, 0x455a14ed);    // 28
   GG(lA, lB, lC, lD, x[13], 5, 0xa9e3e905);    // 29
   GG(lD, lA, lB, lC, x[2], 9, 0xfcefa3f8);     // 30
   GG(lC, lD, lA, lB, x[7], 14, 0x676f02d9);    // 31
   GG(lB, lC, lD, lA, x[12], 20, 0x8d2a4c8a);   // 32

   /* round 3 */
   HH(lA, lB, lC, lD, x[5], 4, 0xfffa3942);     // 33
   HH(lD, lA, lB, lC, x[8], 11, 0x8771f681);    // 34
   HH(lC, lD, lA, lB, x[11], 16, 0x6d9d6122);   // 35
   HH(lB, lC, lD, lA, x[14], 23, 0xfde5380c);   // 36
   HH(lA, lB, lC, lD, x[1], 4, 0xa4beea44);     // 37
   HH(lD, lA, lB, lC, x[4], 11, 0x4bdecfa9);    // 38
   HH(lC, lD, lA, lB, x[7], 16, 0xf6bb4b60);    // 39
   HH(lB, lC, lD, lA, x[10], 23, 0xbebfbc70);   // 40
   HH(lA, lB, lC, lD, x[13], 4, 0x289b7ec6);    // 41
   HH(lD, lA, lB, lC, x[0], 11, 0xeaa127fa);    // 42
   HH(lC, lD, lA, lB, x[3], 16, 0xd4ef3085);    // 43
   HH(lB, lC, lD, lA, x[6], 23, 0x4881d05);     // 44
   HH(lA, lB, lC, lD, x[9], 4, 0xd9d4d039);     // 45
   HH(lD, lA, lB, lC, x[12], 11, 0xe6db99e5);   // 46
   HH(lC, lD, lA, lB, x[15], 16, 0x1fa27cf8);   // 47
   HH(lB, lC, lD, lA, x[2], 23, 0xc4ac5665);    // 48

   /* round 4 */
   II(lA, lB, lC, lD, x[0], 6, 0xf4292244);     // 49
   II(lD, lA, lB, lC, x[7], 10, 0x432aff97);    // 50
   II(lC, lD, lA, lB, x[14], 15, 0xab9423a7);   // 51
   II(lB, lC, lD, lA, x[5], 21, 0xfc93a039);    // 52
   II(lA, lB, lC, lD, x[12], 6, 0x655b59c3);    // 53
   II(lD, lA, lB, lC, x[3], 10, 0x8f0ccc92);    // 54
   II(lC, lD, lA, lB, x[10], 15, 0xffeff47d);   // 55
   II(lB, lC, lD, lA, x[1], 21, 0x85845dd1);    // 56
   II(lA, lB, lC, lD, x[8], 6, 0x6fa87e4f);     // 57
   II(lD, lA, lB, lC, x[15], 10, 0xfe2ce6e0);   // 58
   II(lC, lD, lA, lB, x[6], 15, 0xa3014314);    // 59
   II(lB, lC, lD, lA, x[13], 21, 0x4e0811a1);   // 60
   II(lA, lB, lC, lD, x[4], 6, 0xf7537e82);     // 61
   II(lD, lA, lB, lC, x[11], 10, 0xbd3af235);   // 62
   II(lC, lD, lA, lB, x[2], 15, 0x2ad7d2bb);    // 63
   II(lB, lC, lD, lA, x[9], 21, 0xeb86d391);    // 64

   state[0] += lA;
   state[1] += lB;
   state[2] += lC;
   state[3] += lD;

   /* lClear sensitive information */
   memset(x, 0, sizeof(x));
}