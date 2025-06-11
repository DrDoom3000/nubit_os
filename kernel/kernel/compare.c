#include <stdbool.h>
#include <stddef.h>

int compare_string(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char *)str1 - *(unsigned char *)str2;
}

int cmp_list_string(const char *str, const char *list[], int size) {
    for (int i = 0; i < size; ++i) {
        if (compare_string(str, list[i]) == 0) {
            return 1; 
        }
    }
    return 0; 
}

bool string_compare(char *str1, char *str2)
{
    int i = 0;
    while (str1[i])
    {
        if (str1[i] != str2[i])
            return false;
        i++;
    }
    if (str2[i])
        return false;
    return true;
}

void string_copy(char *strd, char *strs)
{
    for (int i = 0; strs[i]; i++)
    {
        strd[i] = strs[i];
    }
}
