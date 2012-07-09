// UnitTests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using namespace mtd;

long Foo(int left, int right)
{
	long result = 1;
	while(left != right)
	{
		result *= left++;
	}
	return result;
}

BOOST_AUTO_TEST_CASE(TestBarrierAsync)
{
	long result1 = 0, result2 = 0;
	long r1 = 0, r2 = 0, r3 = 0;
	Mutex mutex;
	auto queue = TaskDispatcher::Instance().GetQueue(HIGH);
	queue.EnqueueAsyncTask(
		[&]()
		{
			auto temp = Foo(1, 6);
			{
				Lock lock(mutex);
				r1 += temp;
			}
		}
	);

	queue.EnqueueAsyncTask(
		[&]()
		{
			auto temp = Foo(6, 11);
			{
				Lock lock(mutex);
				r2 += temp;
			}
		}
	);

	queue.EnqueueAsyncTask(
		[&]()
		{
			auto temp = Foo(11, 15);
			{
				Lock lock(mutex);
				r3 += temp;
			}
		}
	);

	queue.EnqueueSyncBarrier(
		[&]()
		{
			Lock lock(mutex);
			result1 = r1 + r2 + r3;
		}
	);

	result2 = Foo(1, 6) + Foo(6, 11) + Foo(11, 15);

	BOOST_CHECK(result1 == result2);
}


BOOST_AUTO_TEST_CASE(TestTaskExecute)
{
	int a = 0;
	Task task([&a](){ a += 5; });
	task.Execute();
	BOOST_CHECK(a == 5);
}

BOOST_AUTO_TEST_CASE(TestBarrierExecute)
{
	int a = 0;
	Barrier taskBarrier([&a](){ a += 5; });
	taskBarrier.Execute();
	BOOST_CHECK(a == 5);
}

