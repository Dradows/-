#include <iostream>
#include <thread>
#include <windows.h>
#include <ctime>
#include <mutex>
#include <fstream>
using namespace std;

class Semaphore {
public:
	Semaphore(int count_ = 0)
		: count(count_) {}

	inline void notify()
	{
		std::unique_lock<std::mutex> lock(mtx);
		count++;
		cv.notify_one();
	}

	inline void wait()
	{
		std::unique_lock<std::mutex> lock(mtx);

		while (count == 0) {
			cv.wait(lock);
		}
		count--;
	}

private:
	std::mutex mtx;
	std::condition_variable cv;
	int count;
};

thread th[100];
Semaphore mtx(1), mtxR(1),mtxW(1);
time_t start = time(NULL), now;
int i, id, delay, duration, cntR = 0,cntW=0,cnt;
char c;

void thR(int id, int delay, int duration) {
	Sleep(delay * 1000);
	mtxR.wait();
	if (cntW>0){}
	if (cntR == 0)
		mtx.wait();
	cntR++;
	mtxR.notify();
	printf("Thread\t%04d R\tstarts at\t%lld\tseconds\n", id, time(NULL) - start);
	Sleep(duration * 1000);
	printf("Thread\t%04d R\tends at\t\t%lld\tseconds\n", id, time(NULL) - start);
	mtxR.wait();
	cntR--;
	if (cntR == 0)
		mtx.notify();
	mtxR.notify();
}

void thW(int id, int delay, int duration) {
	Sleep(delay * 1000);
	mtxW.wait();
	cntW++;
	if (cntW == 1)
		mtxR.wait();
	mtxW.notify();
	mtx.wait();
	printf("Thread\t%04d W\tstarts at\t%lld\tseconds\n", id, time(NULL) - start);
	Sleep(duration * 1000);
	printf("Thread\t%04d W\tends at\t\t%lld\tseconds\n", id, time(NULL) - start);
	mtx.notify();
	mtxW.wait();
	cntW--;
	if (cntW == 0)
		mtxR.notify();
	mtxW.notify();
}

int main()
{
	ifstream fin("input.txt");
	while (fin >> id >> c >> delay >> duration) {
		if (c == 'R') {
			th[id] = thread(thR, id, delay, duration);
		}
		else if (c == 'W') {
			th[id] = thread(thW, id, delay, duration);
		}
	}
	Sleep(500 * 1000);
	return 0;
}

