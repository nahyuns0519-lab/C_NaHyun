#include <stdio.h>  /* (Standard Input/Output)
이 함수가 있어야 printf 같은 도구 이용가능 */

int main(void)  // *main 함수(프로그램의 시작 키)
{  // 함수의 시작과 끝을 감쌈
    printf("hello world\n");  //화면에 글자를 출력해라
    puts("C언어, 재미있다!"); /*Put String (문자열을 놓다)
    문자열을 출력하고 자동으로 줄을 바꿔줌
    printf와 달리 자동 줄 바꿈 기능(\n) 포함
    변한 문자 (%d)는 사용x */ 
    return 0; //프로그램을 정상적으로 끝내라
}