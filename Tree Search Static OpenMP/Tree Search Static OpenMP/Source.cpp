#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <time.h>
#include <iomanip>
#include <omp.h>
#include <stack> 
#include <list>

using namespace std;

const int number_cities = 4;
const int number_threads = 3;
int queue_size;


struct Tour {
	int cities[number_cities + 1];
	int count;
	int cost;

	Tour() {
		fill(cities, cities + number_cities + 1, -1);
		cities[0] = 0;
		count = 1;
	}
};

Tour best_tour;
list<Tour> queue;

int **adjacent_matrix;

void Fill() {

	adjacent_matrix[0][0] = 0; adjacent_matrix[0][1] = 1; adjacent_matrix[0][2] = 3; adjacent_matrix[0][3] = 8;
	adjacent_matrix[1][0] = 5; adjacent_matrix[1][1] = 0; adjacent_matrix[1][2] = 2; adjacent_matrix[1][3] = 6;
	adjacent_matrix[2][0] = 1; adjacent_matrix[2][1] = 18; adjacent_matrix[2][2] = 0; adjacent_matrix[2][3] = 10;
	adjacent_matrix[3][0] = 7; adjacent_matrix[3][1] = 4; adjacent_matrix[3][2] = 12; adjacent_matrix[3][3] = 0;

	/*for (int i = 0; i < number_cities; ++i) {
		for (int j = 0; j < number_cities; ++j) {
			if (i == j) {
				adjacent_matrix[i][j] = 0;
			}
			else {
				adjacent_matrix[i][j] = abs(rand());
			}
		}
	}*/
}

void PrintCity() {

	for (int i = 0; i < number_cities; ++i) {
		for (int j = 0; j < number_cities; ++j) {
			printf("%d \t", adjacent_matrix[i][j]);
		}
		printf("\n");
	}
}

void PrintTour(Tour& tour) {

	cout << "Best Travel Path: ";
	for (int i = 0; i < tour.count; ++i) {
		cout << tour.cities[i] << " ";
	}
	cout << "Cost: " << tour.cost << endl;
}

void Copy(Tour& tour_a, Tour& tour_b) {
	for (int i = 0; i <= number_cities; ++i) {
		tour_b.cities[i] = tour_a.cities[i];
	}
	tour_b.count = tour_a.count;
	tour_b.cost = tour_a.cost;

}

void AddCity(Tour& tour, int city) {
	int old_city = tour.cities[tour.count - 1];
	tour.cities[tour.count] = city;
	++tour.count;
	tour.cost += adjacent_matrix[old_city][city];
}

void RemoveLastCity(Tour& tour) {
	int old_city = tour.cities[tour.count - 1];
	tour.cities[tour.count - 1] = -1;
	--(tour.count);
	int new_city = tour.cities[tour.count - 1];
	tour.cost -= adjacent_matrix[new_city][old_city];
}

bool BestTour(Tour& tour) {

	if (tour.count + adjacent_matrix[tour.cities[tour.count - 1]][0] < best_tour.cost) {
		return true;
	}
	return false;
}

void UpdateBestTour(Tour& tour) {
	if (BestTour(tour)) {
		Copy(tour, best_tour);
		AddCity(best_tour, 0);
	}
}

bool IsVisited(Tour& tour, int city) {
	for (int i = 0; i < tour.count; ++i) {
		if (tour.cities[i] == city) {
			return true;
		}
	}
	return false;
}

bool Feasible(Tour& tour, int city) {

	int last_city = tour.cities[tour.count - 1];

	if (!IsVisited(tour, city) && tour.cost + adjacent_matrix[last_city][city] < best_tour.cost) {
		return true;
	}
	return false;

}

int GetQueueSize() {
	int fact = number_cities - 1;
	int size = number_cities - 1;

	while (size < number_threads) {
		fact++;
		size *= fact;
	}

	return size;
}

void BuildQueue() {
	int curren_size = 0;

	Tour tour;
	tour.cost = 0;

	queue.push_back(tour);

	while (curren_size < number_threads) {

		tour = queue.front();
		queue.pop_front();
		--curren_size;
		for (int i = 1; i < number_cities; ++i) {
			if (!IsVisited(tour, i)) {
				AddCity(tour, i);
				queue.push_back(tour);
				++curren_size;
				RemoveLastCity(tour);
			}
		}
	}
	queue_size = curren_size;

}

void SetBoundaries(long id, int& first, int& last) {
	int quotient, remainder, my_count;

	quotient = queue_size / number_threads;
	remainder = queue_size % number_threads;
	if (id < remainder) {
		my_count = quotient + 1;
		first = id * my_count;
	}
	else {
		my_count = quotient;
		first = id * my_count + remainder;
	}
	last = first + my_count - 1;

}

void PartitionTree(long id, stack<Tour>& stack) {
	int first, last;

#pragma omp master
	BuildQueue();

#pragma omp barrier

	SetBoundaries(id, first, last);

	list<Tour>::iterator it, jt = next(queue.begin(), first);

	for (it = next(queue.begin(), last); jt != it; --it) {
		stack.push(*it);
	}
	stack.push(*it);
}

void Search() {

	int my_id = omp_get_thread_num();

	int city;

	Tour my_tour;

	stack<Tour> my_stack;

	PartitionTree(my_id, my_stack);

	while (!my_stack.empty()) {
		my_tour = my_stack.top();
		my_stack.pop();

		if (my_tour.count == number_cities) {
			if (BestTour(my_tour)) {
#pragma omp critical
				UpdateBestTour(my_tour);
				//PrintTour(best_tour);
			}
		}
		else {
			for (city = number_cities - 1; 1 <= city; --city) {
				if (Feasible(my_tour, city)) {
					AddCity(my_tour, city);
					my_stack.push(my_tour);
					RemoveLastCity(my_tour);
				}
			}
		}
	}

}

int main(int argc, char* argv[]) {

	srand(time(NULL));

	best_tour.cost = INT_MAX;

	adjacent_matrix = new int*[number_cities];
	for (int i = 0; i < number_cities; ++i) {
		adjacent_matrix[i] = new int[number_cities];
	}

	Fill();
	PrintCity();

#pragma omp parallel num_threads(number_threads) default(none)
	Search();

	PrintTour(best_tour);

	return EXIT_SUCCESS;
}