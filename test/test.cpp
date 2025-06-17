
#include "ap_timer.hpp"

#include <stdio.h>
#include <memory.h>


void test1(int timer_id, void* param) {
	printf("test1\n");
	printf((char *)param);
	printf("\n");
	ap_timer::ap_del_timer(timer_id);
	int timer_id1 = ap_timer::ap_add_timer(1000, test1, (void *)"123123");
}

void test2(int timer_id, void* param) {
	printf("test2\n");
}

class AA {
public:

	static void run(int timer_id, void* param) {
		AA* p = (AA*)(param);
		p->test();
	}

	void test() {
		printf("AA::test\n");
		
	}
};

int main(int argc, char *argv[])
{
	//start timer
	ap_timer::ap_start_timer();

	char* p = (char*)malloc(20);
	memset(p, 0, 20);
	strcpy(p, "ABCD");
	int timer_id1 = ap_timer::ap_add_timer(10000, test1, p);
	int timer_id2 = ap_timer::ap_add_timer(10000, test2, NULL);


	//test c++
	AA aa;
	int timer_id3 = ap_timer::ap_add_timer(10000, AA::run, &aa);


	ap_timer::ap_del_timer(timer_id2);
	ap_timer::ap_close_timer();
	while (1) {
		if ('q' == getchar()) {
			break;
		}
	}
	return 0;
}