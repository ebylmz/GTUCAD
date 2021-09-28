char * u_deci_to_hexa (int deci) {
    int r, i = 0, j;
    char tmp[20], * hexa;

    if (deci == 0)
        tmp[i++] = '0';
    else {
        do {
            r = deci % 16;
            
            if (r < 10)
                tmp[i++] =  r + '0';
            else
                tmp[i++] = r - 10 + 'A';
            
            deci /= 16;
        } while (deci > 0);
    }

    hexa = (char *) calloc(i + 1, sizeof(char));

    if (hexa != NULL) {
        j = 0;

        while (i >= 0)
            hexa[j++] = tmp[--i];
        hexa[j] = '\0';
    }

    return hexa;
}