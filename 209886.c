bool Jsi_NumberIsSafeInteger(Jsi_Number n) {
    Jsi_Number n1 = fabsl(n),
    n2 = 9007199254740991LL;
    return (Jsi_NumberIsInteger(n) && n1<=n2);
}