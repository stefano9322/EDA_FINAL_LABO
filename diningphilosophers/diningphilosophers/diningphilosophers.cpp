// diningphilosophers.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
// y luego del waitress implementar la de dijkstra es más fácil
#include <iostream>
#include <thread>
#include <mutex>
#include <random>
using namespace std;
const int number_philosophers = 5;
class DiningPhilopsophers;
//cada filosofo puede comer o pensar mediante random
int get_random(int low, int high) {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> distribution(low, high);
	return distribution(gen);
}
//Requerimientos

// implementar una class waitress
struct Philosopher
{
	Philosopher(unsigned int id, string name, unsigned int number_actions, DiningPhilopsophers* table) 
		: id_(id), name_(name), number_actions_(number_actions), table_(table) {}
	//cada thread imprimir su nombre 10x, en vez de eso un numero random del 1 al 2
	//functor
	void operator ()() {
		for (unsigned int i = 0; i < number_actions_; ++i) {
			if (get_random(1, 2) < 2) {
				Think();
			}
			else {
				Eat();
			}
			 
		}
	}
	void Think();
	void Eat();
	unsigned int id_;
	string name_;
	unsigned int number_actions_;
	DiningPhilopsophers* table_;

};

class DiningPhilopsophers {
public:
	DiningPhilopsophers() {
		thread t1(Philosopher(0, "Socrates", 10, this));
		thread t2(Philosopher(1, "PLaton", 10, this));
		thread t3(Philosopher(2, "Aristoteles", 10, this));
		thread t4(Philosopher(3, "Descartes", 10, this));
		thread t5(Philosopher(4, "Tomas", 10, this));
		//voy a esperar hasta que termine esta thread
		t1.join();
		t2.join();
		t3.join();
		t4.join();
		t5.join();
	}
	//modificar estas dos funcs para la class waitress para pedir permiso como arbitro
	void PickFork(unsigned int id ){
		forks_[id].lock();
		forks_[(id + 1) % number_philosophers].lock();
	}
	void LeaveFork(unsigned int id) {
		forks_[id].unlock();
		forks_[(id + 1) % number_philosophers].unlock();
	}

private:
	mutex forks_[number_philosophers];
};

void Philosopher::Think(){
	cout << name_ + " esta pensando." << endl;
	//para dormirlo mientras piensa; voy a pensar por 6 segundos
	this_thread::sleep_for(chrono::seconds(get_random(1, 6)));


}
//hacer lock al recurso a comer
void Philosopher::Eat() {
	table_->PickFork(id_);
	cout << name_ + " esta comiendo" << endl;
	//comer or un lapso de tiempo
	this_thread::sleep_for(chrono::seconds(get_random(1, 6)));
	table_->LeaveFork(id_);


}


int main()
{
	DiningPhilopsophers table; 
	return 0;
}


