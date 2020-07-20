// Copyright [year] <Copyright Owner
#include <iostream>
#include <mutex>
#include <random>
#include <thread>

const unsigned int kNumberPhilosophers = 5;
const unsigned int kNumberActions = 1000;

int get_random(int low, int high) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distribution(low, high);
	return distribution(gen);
}

class DiningPhilosophers;

struct Philosopher {
	Philosopher(unsigned int id, std::string name, unsigned int number_actions,
		DiningPhilosophers* table)
		: id_(id), name_(name), number_actions_(number_actions), table_(table) {}

	void operator()() {
		for (unsigned int i = 0; i < number_actions_; ++i) {
			Eat();
			// if (get_random(1, 2) < 2) {
			//   Think();
			// } else {
			//   Eat();
			// }
		}
	}

	void Think();
	void Eat();

	unsigned int id_;
	std::string name_;
	unsigned int number_actions_;
	DiningPhilosophers* table_;
};

class DiningPhilosophers {
public:
	DiningPhilosophers() {
		std::thread* philosophers[kNumberPhilosophers];
		std::string philosopher_names[] = {
			"Socrates", "Plato", "Aristotle", "Descartes", "Tomas",
		};
		for (unsigned int i = 0; i < kNumberPhilosophers; ++i) {
			philosophers[i] = new std::thread(
				Philosopher(i, philosopher_names[i], kNumberActions, this));
		}
		for (unsigned int i = 0; i < kNumberPhilosophers; ++i) {
			philosophers[i]->join();
		}
	}

	void PickFork(unsigned int id) {
		forks_[id].lock();
		forks_[(id + 1) % kNumberPhilosophers].lock();
	}

	void LeaveFork(unsigned int id) {
		forks_[id].unlock();
		forks_[(id + 1) % kNumberPhilosophers].unlock();
	}

private:
	std::mutex forks_[kNumberPhilosophers];
};

void Philosopher::Think() {
	std::cout << name_ + " is Thinking." << std::endl;
	// std::this_thread::sleep_for(std::chrono::seconds(get_random(1, 6)));
}

void Philosopher::Eat() {
	table_->PickFork(id_);
	std::cout << name_ + " is Eating." << std::endl;
	// std::this_thread::sleep_for(std::chrono::seconds(get_random(1, 6)));
	table_->LeaveFork(id_);
}

int main() {
	DiningPhilosophers table;
	return 0;
}