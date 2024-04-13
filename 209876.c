bool Jsi_NumberIsFinite(Jsi_Number value)
{
    Jsi_Number r = INFINITY;
    return (Jsi_NumberIsNaN(value)==0 && value != r && r != -value);
}