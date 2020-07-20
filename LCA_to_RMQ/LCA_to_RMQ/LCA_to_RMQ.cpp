// LCA_to_RMQ.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.

#include <iostream>
#include <vector>
#include <math.h>
//nodes
using namespace std;
typedef struct _node
{
	int parent;
	vector<int> child;
}N;
//RMQ
class RMQ
{
public:
	RMQ() {};
	RMQ(int b) :B(b) {};

	//Hash table
	vector <vector<int>> buildHashTable(vector<int> input);
	int queryRMQ_HashTable(vector<vector<int>> M, vector<int>, int i, int j);

	// Cartesian Tree
	void displayNodes(vector<int> A, vector<N> T, int C);
	int buildCartesianTree(vector<int> A, vector<N>& T);
	void EulerTour(vector<N>& T, int level, int& count, int index, vector<int>& E, vector<int>& L, vector<int>& R);

	vector<int> E;
	vector<int> L;
	vector<int> Li;
	vector<int> R;

	//Reduced RMQ
	int blocksize;
	int blockcount;
	int blockcombination;
	vector<int> M;
	vector <vector<int>> ST;
	vector <int> ST_input;
	vector <int> ST_Index;
	int B;

	void Reduced_RMQ(vector<int> input);
	void buildMappingTable(vector<int> input, vector<N>& T, int root = 0);
	int  query_Reduced_RMQ(vector<int>, int x, int y, bool lca = false);
};
//Hash Table
vector <vector<int>>  RMQ::buildHashTable(vector<int> input)
{
	vector <vector<int>> M(input.size(), vector<int>(input.size()));
	for (int u = 0; u < input.size(); u++)
		for (int v = 0; 1 << v < input.size(); v++)
			if (v == 0)
				M[u][v] = u;
			else
				M[u][v] = -1;
	for (int u = 1; (1 << u) <= input.size(); u++)
		for (int v = 0; v <= input.size() - (1 << u); v++)
		{
			int end = v + (1 << u) - 1;
			int mid = (v + end) / 2;
			if (input[M[v][u - 1]] <= input[M[mid + 1][u - 1]])
				M[v][u] = M[v][u - 1];
			else
				M[v][u] = M[mid + 1][u - 1];
		}
	return M;

}
//Query HT
int RMQ::queryRMQ_HashTable(vector<vector<int>>M, vector<int> input, int i, int j)
{
	int count = floor(log2(j - i + 1));
	int block2_start = j - (1 << count) + 1;
	return (input[M[i][count]] < input[M[block2_start][count]]) ? M[i][count] : M[block2_start][count];
}
//Cartesian tree
int RMQ::buildCartesianTree(vector<int> A, vector<N>& T)
{
	const int C = A.size();
	const int stack[C];
	int tos = -1;
	for (int i = 0; i < C; i++)
	{
		T[i].child.resize(B);
		int j = tos;
		while ((j != -1) && (A[stack[j]] > A[i]))
			j--;

		if (tos == -1)
		{
			T[i].parent = -1;
			T[i].child[0] = -1;
			T[i].child[1] = -1;
		}
		else if (j == tos)
		{
			T[i].parent = stack[tos];
			T[i].child[0] = T[i].child[1] = -1;
			if (T[stack[tos]].child[0] == -1)
				T[stack[tos]].child[0] = i;
			else
				T[stack[tos]].child[1] = i;
		}
		else
		{
			if (j != -1)
			{
				if (T[stack[j]].child[1] == -1)
					T[stack[j]].child[0] = i;
				else
					T[stack[j]].child[1] = i;

				T[i].parent = stack[j];
			}
			else
			{
				T[i].child[1] = -1;
				T[i].parent = -1;
			}
			T[i].child[1] = -1;
			T[i].child[0] = stack[j + 1];
			T[stack[j + 1]].parent = i;
		}

		stack[++j] = i;
		tos = j;
	}
	return stack[0];
}
//print
void RMQ::displayNodes(vector<int> A, vector<N> T, int C)
{
	cout << "Index |Value |Parent |Left |Right" << endl;
	for (int i = 0; i < C; i++)
		cout << i << "      " << A[i] << "      " << T[i].parent << "      " << T[i].child[0] << "      " << T[i].child[1] << endl;
}
//Euler tour
void RMQ::EulerTour(vector<N>& T, int level, int& index, int node, vector<int>& E, vector<int>& L, vector<int>& R)
{
	E[index] = node;
	L[index] = level;
	if (R[node] == -1)
		R[node] = index;
	for (int i = 0; i < B; i++)
	{
		if (T[node].child[i] == -1)
			return;
		EulerTour(T, level + 1, ++index, T[node].child[i], E, L, R);
		++index;
		E[index] = node;
		L[index] = level;
	}
}
void RMQ::Reduced_RMQ(vector<int> input)
{
	int C = input.size();
	vector<N> T(C);
	B = 2;
	int root = buildCartesianTree(input, T);
	buildMappingTable(input, T, root);
}
void RMQ::buildMappingTable(vector<int> input, vector<N>& T, int root)
{
	int C = input.size();
	E.resize(2 * C - 1);
	L.resize(2 * C - 1);
	R.resize(C, -1);
	int c = 0;
	EulerTour(T, 0, c, root, E, L, R);
	Li.resize(2 * C - 1);
	for (int ii = 1; ii < L.size(); ii++)
	{
		Li[ii] = L[ii] - L[ii - 1];
		if (Li[ii] < 0)
			Li[ii] = 0;
	}
	blocksize = ceil((double)log2(2 * C - 1)) / 2;
	blockcount = ceil((double)(2 * C - 1) / (double)blocksize);
	blockcombination = blocksize * (blocksize + 1) / 2;
	M.resize(blockcombination * blockcount);
	for (int ii = 0; ii < blockcount; ii++)
	{
		int startIndex = ii * blockcombination;
		int rowIndex = 0;
		for (int jj = 0; jj < blocksize; jj++)
		{
			int min = Li[ii * blocksize];
			for (int k = jj; k < blocksize; k++)
				if (Li[(ii * blocksize) + k] == min)
				{
					if (min == 1)
						M[startIndex + (rowIndex++)] = jj;
					else
						M[startIndex + (rowIndex++)] = k;
				}
				else if (Li[(ii * blocksize) + k] < min)
					M[startIndex + (rowIndex++)] = k;
				else
					M[startIndex + (rowIndex++)] = jj;
		}
	}
	// Using Hash table
	ST.resize(blockcount, vector<int>(blockcount));
	ST_input.resize(blockcount);
	ST_Index.resize(blockcount);
	for (int ii = 0; ii < blockcount; ii++)
	{
		int idx = M[blocksize - 1 + ii * blockcombination];
		idx = (ii * blocksize) + idx;
		ST_input[ii] = input[E[idx]];
		ST_Index[ii] = idx;
	}
	ST = buildHashTable(ST_input);
	return;
}
int RMQ::query_Reduced_RMQ(vector<int> input, int x, int y, bool lca)
{
	int i = R[x];
	int j = R[y];
	int ST_start = i / blocksize;
	int ST_end = j / blocksize;
	int inblock_start = i % blocksize;
	int inblock_end = (j % blocksize);
	int inblock_start_index = -1, inblock_end_index = -1;
	if (ST_start == ST_end)
	{
		int left = (blocksize)-inblock_start;
		inblock_start_index = (ST_start)* blockcombination + (blockcombination - ((left * (left + 1)) / 2)) + inblock_end;
	}
	else
	{
		if (inblock_start)
		{
			int left = ((inblock_start + 1) * (2 * blocksize - inblock_start)) / 2;
			inblock_start_index = ((ST_start)* blockcombination) + left - 1;
			ST_start++;
		}
		if (inblock_end != blocksize - 1)
		{
			inblock_end_index = ((ST_end * blockcombination) - 1) + inblock_end + 1;
			ST_end--;
		}
	}
	int firstBlockMinimum = (inblock_start_index != -1) ? E[M[inblock_start_index] + ((i / blocksize) * blocksize)] : -1;
	int lastBlockMinimum = (inblock_end_index != -1) ? E[M[inblock_end_index] + ((j / blocksize) * blocksize)] : -1;
	int stBlockMinimum = (ST_end >= ST_start) ? E[ST_Index[queryRMQ_HashTable(ST, ST_input, ST_start, ST_end)]] : -1;
	/*
	returns the positive infinity value of the given floating-point type
	(public static member function of std::numeric_limits<T>)
	https://en.cppreference.com/w/cpp/numeric/math/INFINITY
	*/
	if (lca)
	{
		int minimum = firstBlockMinimum == -1 ? INFINITY : firstBlockMinimum;
		if (lastBlockMinimum != -1 && lastBlockMinimum <= minimum)
		{
			minimum = lastBlockMinimum;
		}
		if (stBlockMinimum != -1 && stBlockMinimum <= minimum)
		{
			minimum = stBlockMinimum;
		}
		return minimum;
	}
	else
	{
		int minimum = firstBlockMinimum == -1 ? INFINITY : input[firstBlockMinimum];
		int minimum_index = firstBlockMinimum;
		if (lastBlockMinimum != -1 && input[lastBlockMinimum] <= minimum)
		{
			minimum = input[lastBlockMinimum];
			minimum_index = lastBlockMinimum;
		}
		if (stBlockMinimum != -1 && input[stBlockMinimum] <= minimum)
		{
			minimum = input[stBlockMinimum];
			minimum_index = stBlockMinimum;
		}
		return minimum_index;
	}

}

int main()
{
	RMQ r;
	vector<int> a = { 2, 4, 3, 1, 6, 7, 8, 9, 1, 7 };
	int i = 3, j = 5;
	//Sparse Table 
	vector<vector<int>> sparseTable = r.buildHashTable(a);
	int min = r.queryRMQ_HashTable(sparseTable, a, i, j);
	cout << "[Sparse_Table] RMQ " << min << " is " << a[min] << endl;
	//LCA
	int b;
	cin >> b;
	int n;
	cin >> n;
	vector<N> nodes(n);
	vector<int> input(n);
	for (int ii = 0; ii < n; ii++)
	{
		int count = 0;
		int t;
		input[ii] = ii;
		do {

			cin >> t;
			nodes[ii].child.push_back(t);
		} while (t != -1);
	}
	RMQ r(b);
	r.buildMappingTable(input, nodes, 0);
	int min = r.query_Reduced_RMQ(input, 1, 1, true);
	cout << " LCA = " << min;
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
