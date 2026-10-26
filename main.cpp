#include <iostream>
#include <memory>
#include <thread>
#include <vector>
#include <stop_token>
#include <string>
#include <chrono>

struct Widget
{
	int id;

	Widget(int i) : id(i)
	{
	}

	~Widget() = default;
};

// UNSAFE: Takes a reference to an external shared_ptr.
// If the caller reassigns or resets that specific shared_ptr instance concurrently,
// reading through this reference causes a data race on the handle's internal pointers.
void unsafe_reader(std::stop_token stop, const std::shared_ptr<Widget> &ptr_ref)
{
	uint64_t reads = 0;
	while (!stop.stop_requested())
	{
		// Attempting to read the pointer or copy from the reference concurrently
		// while another thread writes to ptr_ref is a data race.
		if (ptr_ref)
		{
			volatile int val = ptr_ref->id;
			(void) val;
			reads++;
		}
	}
	std::cout << "[Unsafe Reader] Completed " << reads << " reads.\n";
}

// SAFE: Passed by value.
// An independent shared_ptr instance is constructed via atomic ref-count increment.
// Main thread mutations to its own instance do not affect this local handle.
void safe_reader(std::stop_token stop, std::shared_ptr<Widget> ptr_val)
{
	uint64_t reads = 0;
	while (!stop.stop_requested())
	{
		if (ptr_val)
		{
			volatile int val = ptr_val->id;
			(void) val;
			reads++;
		}
	}
	std::cout << "[Safe Reader] Completed " << reads << " reads.\n";
}

int main(int argc, char *argv[])
{
	bool run_unsafe = true;
	if (argc > 1 && std::string(argv[1]) == "--safe")
	{
		run_unsafe = false;
	}

	auto shared_resource = std::make_shared<Widget>(42);

	std::jthread worker;

	if (run_unsafe)
	{
		std::cout << ">>> Running UNSAFE test (passing by reference)...\n";
		// Passing shared_resource by reference (std::cref)
		worker = std::jthread(unsafe_reader, std::cref(shared_resource));
	}
	else
	{
		std::cout << ">>> Running SAFE test (passing by value)...\n";
		// Passing shared_resource by value
		worker = std::jthread(safe_reader, shared_resource);
	}

	// Mutator: Rapidly reassign the local shared_ptr instance
	for (int i = 0; i < 200'000; ++i)
	{
		shared_resource = std::make_shared<Widget>(i);
	}

	worker.request_stop();
	worker.join();

	std::cout << ">>> Test finished cleanly.\n";
	return 0;
}
