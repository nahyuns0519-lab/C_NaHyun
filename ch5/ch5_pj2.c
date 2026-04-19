#include <stdio.h>

int main(void)
{
    int cred2, cred3;
    cred2 = 2, cred3 = 4;
    cred2 = 2 * cred2;
    printf("2학점 과목: %d\n", cred2);
    printf("3학점 과목: %d\n", cred3 = 3 * cred3);
    
    return 0;
}