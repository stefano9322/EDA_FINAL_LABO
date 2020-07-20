// diningphilophers_waitress.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <iostream>
#include <mutex>
#include <random>
#include <thread>

const unsigned int kNumberPhilosophers = 5;
const unsigned int kNumberActions = 100000;

int get_random(int low, int high) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distribution(low, high);
	return distribution(gen);
}

class DiningPhilosophers;
class Waitress;

struct Philosopher {

	Philosopher(unsigned int id, std::string name, unsigned int number_actions,
		DiningPhilosophers* table, Waitress* waitress_)
		: id_(id), name_(name), number_actions_(number_actions), table_(table), waitress(waitress_) {}

	void operator()();

	void Think();
	void Eat();

	unsigned int id_;
	std::string name_;
	unsigned int number_actions_;
	DiningPhilosophers* table_;
	Waitress* waitress;
};


class Waitress {
public:
	Waitress(DiningPhilosophers* table) : table_(table) {
		for (int i = 0; i < kNumberPhilosophers; ++i)
			forks[i] = true;

		std::thread* philosophers[kNumberPhilosophers];
		std::string philosopher_names[] = {"Socrates", "Plato", "Aristotle", "Descartes", "Tomas"};
		for (unsigned int i = 0; i < kNumberPhilosophers; ++i) {
			philosophers[i] = new std::thread(
				Philosopher(i, philosopher_names[i], kNumberActions, table_, this));
		}

		for (unsigned int i = 0; i < kNumberPhilosophers; ++i) {
			philosophers[i]->join();
		}

	}
	bool forks[kNumberPhilosophers];
	DiningPhilosophers* table_;
	std::mutex camarera;

};

class DiningPhilosophers {
public:
	DiningPhilosophers() {
		waitress_ = new Waitress(this);

	}

	void PickFork(unsigned int id) {
		forks_[id].lock();
		forks_[(id + 1) % kNumberPhilosophers].lock();
		waitress_->forks[id] = false;
		waitress_->forks[(id + 1) % kNumberPhilosophers] = false;
	}

	void LeaveFork(unsigned int id) {
		forks_[id].unlock();
		forks_[(id + 1) % kNumberPhilosophers].unlock();

		waitress_->forks[id] = true;
		waitress_->forks[(id + 1) % kNumberPhilosophers] = true;
	}

	Waitress* waitress_;
private:
	std::mutex forks_[kNumberPhilosophers];
};

void Philosopher::Think() {
	table_->PickFork(id_);
	std::cout << name_ + " is Thinking." << std::endl;
	table_->LeaveFork(id_);
}

void Philosopher::Eat() {
	table_->PickFork(id_);
	std::cout << name_ + " is Eating." << std::endl;
	table_->LeaveFork(id_);
}

void Philosopher::operator()() {
	for (unsigned int i = 0; i < number_actions_; ++i) {
		while (true) {
			if ((waitress->forks[id_] == waitress->forks[(id_ + 1) % kNumberPhilosophers]) == true) {
				waitress->camarera.lock();
				Eat();
				waitress->camarera.unlock();
				break;
			}
		}
	}
}
int main() {
	DiningPhilosophers table;
	std::cout << " Termino " << std::endl;
	return 0;
}