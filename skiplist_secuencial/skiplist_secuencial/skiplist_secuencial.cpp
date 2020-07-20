// skiplist_secuencial.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <iostream>
#include <random>

#include "skiplist_secuencial.h"

int get_random() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distribution(1, 100);
	return distribution(gen);
}

int main() {
	ADE::Sequential::SkipList<int> test;

	unsigned int counter = 100;

	for (unsigned int i = 0; i < counter;) {
		if (test.insert(get_random())) {
			++i;
		}
	}
	//Impriminos 
	test.debug_print();
	//Eliminaremos el dato 89
	test.insert(89);
	test.Delete_element(89);
	//Volvemos a imprimir
	test.debug_print();


	std::cout << std::endl;
	while (!test.empty()) {
		if (test.Delete(get_random())) {
			--counter;
		}
	}

	std::cout << "Counter: " << counter << std::endl;

	test.insert(1);
	test.insert(2);
	test.insert(3);
	std::cout << test.size() << std::endl;

	test.clear();
	test.insert(1);
	test.insert(2);
	test.insert(3);
	test.insert(4);
	test.insert(5);
	std::cout << std::endl << std::endl;

	test.debug_print();
	test.Delete(3);
	if (test.contains(3)) {
		std::cout << "El elemento 2 no debería estar" << std::endl;
	}
	std::cout << std::endl;
	test.debug_print();

	return 0;
}
// Ejecutar programa: Ctrl + F5 o menú Depurar > Iniciar sin depurar
// Depurar programa: F5 o menú Depurar > Iniciar depuración

// Sugerencias para primeros pasos: 1. Use la ventana del Explorador de soluciones para agregar y administrar archivos
//   2. Use la ventana de Team Explorer para conectar con el control de código fuente
//   3. Use la ventana de salida para ver la salida de compilación y otros mensajes
//   4. Use la ventana Lista de errores para ver los errores
//   5. Vaya a Proyecto > Agregar nuevo elemento para crear nuevos archivos de código, o a Proyecto > Agregar elemento existente para agregar archivos de código existentes al proyecto
//   6. En el futuro, para volver a abrir este proyecto, vaya a Archivo > Abrir > Proyecto y seleccione el archivo .sln
