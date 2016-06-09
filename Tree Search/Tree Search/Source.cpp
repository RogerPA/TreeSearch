#include <stdlib.h>
#include <iostream>
#include <limits.h>
#include <time.h>
#include <stack> 

using namespace std;

const int number_cities = 4;

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

	/*for (int i = 0; i <= number_cities; ++i) {
	tour_b.cities[i] = tour_a.cities[i];
	}*/

	memcpy(tour_b.cities, tour_a.cities, (number_cities + 1) * sizeof(int));

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
	Copy(tour, best_tour);
	AddCity(best_tour, 0);
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

void DepthFirstSearchRecursive(Tour& tour) {

	if (tour.count == number_cities) {
		if (BestTour(tour)) {
			UpdateBestTour(tour);
			//PrintTour(best_tour);
		}
	}
	else {
		for (int i = 1; i < number_cities; ++i) {
			if (Feasible(tour, i)) {
				AddCity(tour, i);
				DepthFirstSearchRecursive(tour);
				RemoveLastCity(tour);
			}
		}
	}
}

void DepthFirstIterative1() {

	int city;

	Tour my_tour;

	my_tour.cost = 0;

	stack<int> my_stack;

	//my_stack.push(-1);

	for (city = number_cities - 1; 1 <= city; --city) {
		my_stack.push(city);
	}

	while (!my_stack.empty()) {
		city = my_stack.top();
		my_stack.pop();

		if (city == -1) {
			RemoveLastCity(my_tour);
		}
		else if (Feasible(my_tour, city)) {
			AddCity(my_tour, city);

			if (my_tour.count == number_cities) {
				if (BestTour(my_tour)) {
					UpdateBestTour(my_tour);
					//PrintTour(best_tour);
				}
				RemoveLastCity(my_tour);
			}
			else {
				my_stack.push(-1);
				for (city = number_cities - 1; 1 <= city; --city) {
					if (!IsVisited(my_tour, city)) {
						my_stack.push(city);
					}
				}
			}
		}

	}
}

void DepthFirstIterative2() {

	int city;

	Tour my_tour;

	my_tour.cost = 0;

	stack<Tour> my_stack;
	my_stack.push(my_tour);

	while (!my_stack.empty()) {

		my_tour = my_stack.top();
		my_stack.pop();

		if (my_tour.count == number_cities) {
			if (BestTour(my_tour)) {
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

	Tour my_tour;

	adjacent_matrix = new int*[number_cities];
	for (int i = 0; i < number_cities; ++i) {
		adjacent_matrix[i] = new int[number_cities];
	}

	my_tour.cost = 0;
	best_tour.cost = INT_MAX;

	Fill();
	PrintCity();

	DepthFirstSearchRecursive(my_tour);
	//DepthFirstIterative1();
	//DepthFirstIterative2();


	PrintTour(best_tour);


	return EXIT_SUCCESS;
}