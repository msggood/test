#include <locale.h>
#include <stdio.h>
using namespace std;
int main(void)
{
	setlocale(LC_CTYPE, ""); // ⑧
        printf("%S", "\u4e00\u53f7\u7ba1\u7ebf\u5de1\u68c0\n"); // ⑤
　　    wprintf("%s", "\u4e00\u53f7\u7ba1\u7ebf\u5de1\u68c0\n"); // ⑦	
	return 0;
}
