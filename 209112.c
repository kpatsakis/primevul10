double date2serialdate(int day, int month, int year)
/* convert date to days since 1.1.1900 */
{
   int serialdate;

   serialdate = (int) ((1461 * (year + 4800 + (int) ((month - 14) / 12))) / 4) + (int) ((367 * (month - 2
                                                                                                -
                                                                                                12 *
                                                                                                ((month -
                                                                                                  14) /
                                                                                                 12))) / 12) -
                (int) ((3 * ((int) ((year + 4900 + (int) ((month - 14) / 12))
                                    / 100))) / 4) + day - 2415019 - 32075;

   return serialdate;
}