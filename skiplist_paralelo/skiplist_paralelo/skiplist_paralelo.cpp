// skiplist_paralelo.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//PRUEBA FALLIDA NO COMPLETADA

#include <iostream>
#include <cassert>
#include <random>
#include <thread>
#include <vector>
#include "skiplist_concurrente.h"
#include "skiplist_paralelo.h"

int get_random(int low, int high) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distribution(low, high);
	return distribution(gen);
}
class Tester {
public:
	static const int MAX_NUM_OPERATIONS = 10000;
	static int MIN_VALS;
	static int MAX_VALS;

public:
	Tester(ADE::Concurrent::SkipList<int>& lista, int modifier)
		: lista_(lista), modifier_(modifier) {}
	void operator()() {
		static const double CONTAINS_WEIGHT = 0.2;
		static const double INSERT_WEIGHT = 0.7;

		for (int i = 0; i < MAX_NUM_OPERATIONS; ++i) {
			if (get_random_float(0, 1) < INSERT_WEIGHT) {
				lista_.my_insert(get_random(0, 1000));
			}
			else {

				lista_.deleteElement(get_random(0, 1000));
			}
		}
	}
}

	ADE::Concurrent::SkipList<int>& lista_;
	int modifier_;
};
int main() {
	std::cout << "Test on consurrent SkipListSet" << std::endl
		<< "==============================" << std::endl;

	int num_threads = 8;
	ADE::Concurrent::SkipList<int> lista;

	std::vector<std::thread*> threads;
	for (int i = 0; i < num_threads; i++) {
		Tester tester(lista, i * Tester::MAX_NUM_OPERATIONS);
		std::thread* process = new std::thread(tester);
		threads.push_back(process);
	}
	for (int i = 0; i < num_threads; ++i) {
		threads[i]->join();
	}
	lista.debug_print();
	lista.deleteElement(4);
	lista.debug_print();
	return 0;
}
/* TEST PRUEBA
int get_random(int lower, int higher) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distribution(lower, higher);
  return distribution(gen);
}
struct IntWithObjCounter {
  IntWithObjCounter(int n = 0) : num(n) { ++counter; }
  IntWithObjCounter(const IntWithObjCounter& o) : num(o.num) { ++counter; }
  ~IntWithObjCounter() { --counter; }
  int num;
  static unsigned int counter;
};
unsigned int IntWithObjCounter::counter = 0;
bool operator<(const IntWithObjCounter& left, const IntWithObjCounter& right) {
  return left.num < right.num;
}
bool operator==(const IntWithObjCounter& left, const IntWithObjCounter& right) {
  return left.num == right.num;
}
void TestSequential() {
  std::cout << "Basic test on consurrent SkipListSet" << std::endl
			<< "====================================" << std::endl;
  {
	int num_iter = 1000;
	ADE::Concurrent::ConcurrentSkipList<IntWithObjCounter> test;
	for (int i = 1; i < num_iter; ++i) test.insert(num_iter);
	for (int i = 1; i < num_iter; ++i) test.remove(num_iter);
	assert(IntWithObjCounter::counter == 2);
  }
  {
	ADE::Concurrent::ConcurrentSkipList<int> test(10);
	int max_value = 1000;
	int num_iter = 1000;
	for (int i = 1; i < num_iter; ++i) test.insert(get_random(0, max_value));
	for (int i = 1; i < num_iter; ++i) test.remove(get_random(0, max_value));
	test.debug_print();
	std::cout << std::endl;
  }
  {
	ADE::Concurrent::ConcurrentSkipList<int> test(10);
	test.insert(1);
	test.insert(2);
	test.insert(3);
	test.remove(3);
	test.remove(2);
	test.remove(1);
	assert(!test.contains(1));
  }
  {
	ADE::Concurrent::ConcurrentSkipList<int> test(10);
	test.insert(1);
	test.insert(2);
	test.insert(3);
	test.insert(4);
	test.insert(5);
	assert(!test.insert(3));
	test.remove(3);
	assert(test.insert(3));
	test.remove(3);
	assert(!test.contains(3));
	assert(test.contains(5));
  }
}
class ListTester {
 public:
  static const int MAX_NUM_OPERATIONS = 10000;
  static int MIN_VALS;
  static int MAX_VALS;
 public:
  ListTester(SkipListSet<int>& lista, int modifier)
	  : lista_(lista), modifier_(modifier) {}
  void operator()() {
	static const double CONTAINS_WEIGHT = 0.2;
	static const double INSERT_WEIGHT = 0.7;
	for (int i = 0; i < MAX_NUM_OPERATIONS; ++i) {
	  if (sample_01_() < CONTAINS_WEIGHT) {
		lista_.contains(random_());
	  } else {
		if (sample_01_() < INSERT_WEIGHT)
		  lista_.insert(random_());
		else
		  lista_.remove(random_());
	  }
	}
  }
 private:
  ADE::Concurrent::ConcurrentSkipListSet<int>& lista_;
  int modifier_;
  static std::variate_generator<std::mt19937, std::uniform_real<double> >
	  sample_01_;
  static std::variate_generator<std::mt19937, std::uniform_int<unsigned int> >
	  random_;
};
int ListTester::MIN_VALS = 1;
int ListTester::MAX_VALS = 15000;
std::variate_generator<std::mt19937, std::uniform_real<double> >
	ListTester::sample_01_(
		std::mt19937(static_cast<unsigned long>(std::time(NULL))),
		std::uniform_real<double>());
std::variate_generator<std::mt19937, std::uniform_int<unsigned int> >
	ListTester::random_(
		std::mt19937(static_cast<unsigned long>(std::time(NULL))),
		std::uniform_int<unsigned int>(ListTester::MIN_VALS,
									   ListTester::MAX_VALS));
void RunTest(bool print) {
  int num_threads = 8;
  ADE::Concurrent::ConcurrentSkipList<int> lista;
  std::vector<std::thread*> threads;
  for (int i = 0; i < num_threads; i++) {
	ListTester tester(lista, i * ListTester::MAX_NUM_OPERATIONS);
	std::thread* process = new std::thread(tester);
	threads.push_back(process)
  }
  for (int i = 0; i < num_threads; ++i) {
	threads[i]->join();
  }
  if (print) {
	cout << "Test on consurrent SkipListSet" << endl
		 << "==============================" << endl;
	stringstream os;
	os << endl;
	lista.debug_print(os);
	os << endl;
	cout << os.str();
  }
}
int main() {
  freopen("output.txt", "w", stdout);
  TestSequential();
  const int trials = 10;
  for (int i = 0; i < trials; ++i) {
	cerr << "Running test case " << i << endl;
	RunTest(i == trials - 1);
  }
  return 0;
}
*/

