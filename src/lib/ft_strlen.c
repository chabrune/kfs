unsigned int ft_strlen(const char *str)
{
    unsigned int i = 0;
    while(str[i])
        i++;
    return(i);
}