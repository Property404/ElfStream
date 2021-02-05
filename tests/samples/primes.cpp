/*
 * Program to print out list of primes from 2 to some large number
 */
#include <iostream>
#include <deque>
int main()
{
	std::deque<uintmax_t>primes = {2};

	for(uintmax_t i = 3; i< 100000L;i+=2)
	{
		bool is_prime = true;
		for(const auto old_prime : primes)
		{
			if(i%old_prime == 0)
			{
				is_prime = false;
				break;
			}
		}
		if(is_prime)
			primes.push_back(i);
	}

	std::cout<<"Primes"<<std::endl;
	std::cout<<"------"<<std::endl;

	for(const auto prime : primes)
		std::cout<<prime<<std::endl;
}
