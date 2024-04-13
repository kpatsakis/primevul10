void serialdate2date(double days, int *day, int *month, int *year)
/* convert days since 1.1.1900 to date */
{
   int i, j, l, n;

   l = (int) days + 68569 + 2415019;
   n = (int) ((4 * l) / 146097);
   l = l - (int) ((146097 * n + 3) / 4);
   i = (int) ((4000 * (l + 1)) / 1461001);
   l = l - (int) ((1461 * i) / 4) + 31;
   j = (int) ((80 * l) / 2447);
   *day = l - (int) ((2447 * j) / 80);
   l = (int) (j / 11);
   *month = j + 2 - (12 * l);
   *year = 100 * (n - 49) + i + l;
}