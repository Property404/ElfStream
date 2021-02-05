/* Simple example of two threads incrementing a counter
 * with mutual exclusion
 *
 * This program should generate the same result every time
 * (ie no race conditions)
 */
#include <thread>
#include <iostream>
#include <unistd.h>
#include <mutex>

static constexpr auto BIGGEST = 100;
std::mutex mutex;

static uintmax_t count= 0;
void increment()
{
	for(uintmax_t i=0;i<1000000L;i++)
	{
		mutex.lock();
		count++;
		mutex.unlock();
	}
}
int main()
{
	std::thread thread_1(increment);
	std::thread thread_2(increment);
	thread_1.join();
	thread_2.join();
	std::cout<<count<<std::endl;
}
