#include <iostream>
#include <vector>
#include <ctime>
#include <queue>
#include <list>
#include <thread>
#include <omp.h>

using namespace std;

const unsigned COUNT_PETS = 7;
const unsigned COUNT_MUTATIONS = 2;
const unsigned COUNT_GENERATIONS = 50;

vector<vector<int>> pets;
const int n = 8;
const int k = 2;
vector<vector<int>> edges = {{0, 1},
                             {0, 2},
                             {0, 3},
                             {1, 2},
                             {2, 3},
                             {2, 4},
                             {4, 5},
                             {4, 6},
                             {4, 7},
                             {5, 6},
                             {6, 7},};

vector<int> createPet() {
    vector<int> reb(edges.size());
    return reb;
}
void createGeneration () {
    for (int i = 0; i < COUNT_PETS; ++i) {
        pets.push_back(createPet());
    }
}
void displayPet(const vector<int>& vector) {
    for (int i : vector)
        cout << i << " ";
}
void mutationGeneration() {
    srand(time(nullptr));
    for (unsigned i = 0; i < pets.size(); ++i) {
        for (unsigned j = 0; j < COUNT_MUTATIONS; ++j) {
            int mutationGen = rand() % edges.size();
            if (rand() % edges.size() % 2 == 0) {
                pets[i][mutationGen] = !pets[i][mutationGen];
            }
        }
    }
}
vector<int> crossingGeneration(const vector<int>& vector1, const vector<int>& vector2) {
    vector<int> vector3 = createPet();
    #pragma omp parallel for num_threads(4)
    for (unsigned i = 0; i < edges.size(); ++i) {
        if (i < edges.size()) {
            vector3[i] = vector1[i];
        } else {
            vector3[i] = vector2[i];
        }
    }
    return vector3;
}
vector<vector<int>> crossingGeneration() {
    vector<vector<int>> newPets;
    for (unsigned i = 0; i < pets.size(); ++i) {
        for (unsigned j = i + 1; j < pets.size(); ++j) {
            newPets.push_back(crossingGeneration(pets[i], pets[j]));
            newPets.push_back(crossingGeneration(pets[j], pets[i]));
        }
    }
    return newPets;
};
vector<vector<int>> extinctionGeneration() {
    vector<vector<int>> newPets;
    newPets.reserve(COUNT_PETS);
    for (int i = 0; i < COUNT_PETS; ++i) {
        newPets.push_back(pets[i]);
    }
    return newPets;
}
int calcCount(const vector<int>& vector, int& count) {
    queue<int> q;
    bool used[n];
    for (bool & i : used) {
        i = false;
    }
    count = 0;
    for (int number = 0; number < n; number++) {
        if (!used[number]) {
            used[number]  = true;
            q.push(number);
            count++;
        }
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            used[v] = true;
//            cout << endl << v << " use";
            for (unsigned i = 0; i < edges.size(); ++i) {
                if (!vector[i] && edges[i][0] == v) {
//                    cout << endl << edges[i][0] << "-" << edges[i][1];
                    int to = edges[i][1];
                    if (!used[to]) {
                        used[to] = true;
                        q.push(to);
                    }
                }
                if (!vector[i] && edges[i][1] == v) {
//                    cout << endl << edges[i][0] << "-" << edges[i][1];
                    int to = edges[i][0];
                    if (!used[to]) {
                        used[to] = true;
                        q.push(to);
                    }
                }
            }
        }
    }
    return count;
}
int calcMax(const vector<int>& vector, int& max) {
    list<int> weights;
    queue<int> q;
    bool used[n];
    for (bool & i : used) {
        i = false;
    }
    int weight = 0;
    int count = 0;
    for (int number = 0; number < n; number++) {
        if (used[number]) {
            continue;
        }
        used[number]  = true;
        q.push(number);
        count++;
        weight = 0;
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            used[v] = true;
            for (unsigned i = 0; i < edges.size(); ++i) {
                if (!vector[i] && edges[i][0] == v) {
                    int to = edges[i][1];
                    if (!used[to]) {
                        weight++;
                        used[to] = true;
                        q.push(to);
                    }
                }
                if (!vector[i] && edges[i][1] == v) {
                    int to = edges[i][0];
                    if (!used[to]) {
                        weight++;
                        used[to] = true;
                        q.push(to);
                    }
                }
            }
        }
        weights.push_back(weight);
    }
    weights.sort();
    max = abs(weights.front() - weights.back());
    return max;
}
void displayGeneration(int number_generation, const string& str = "") {
    if (!str.empty()) {
        cout << str + " " << number_generation << endl;
    } else {
        cout << "generation " << number_generation << endl;
    }
    for (const auto & pet : pets) {
        displayPet(pet);
        int a;
        cout << "count: " << calcCount(pet, a) << " ";
        cout << "count: " << calcMax(pet, a) << endl;
    }
}
bool betteAdapted(const vector<int>& vector1, const vector<int>& vector2) {
    int count1 = calcCount(vector1, count1);
    int count2 = calcCount(vector2, count2);
    int max1 = calcMax(vector1, max1);
    int max2 = calcMax(vector2, max2);
    if (abs(k - count2) - abs(k - count1) == 0) {
        return max1 < max2;
    }
    return abs(k - count2) > abs(k - count1);
}
bool betteAdaptedParallel(const vector<int>& vector1, const vector<int>& vector2) {

    int count1, count2;

    thread thread1(calcCount, vector1, std::ref(count1));
    thread thread2(calcCount, vector2, std::ref(count2));

    thread1.join();
    thread2.join();

    int max1, max2;
    thread threadMax1(calcMax, vector1, std::ref(max1));
    thread threadMax2(calcMax, vector2, std::ref(max2));

    threadMax1.join();
    threadMax2.join();

    if (abs(k - count2) - abs(k - count1) == 0) {
        return max1 < max2;
    }
    return abs(k - count2) > abs(k - count1);
}
void sortPets() {
    for (unsigned i = 0; i < pets.size(); i++) {
        for (unsigned j = 0; j < pets.size(); j++) {
            if (betteAdaptedParallel(pets[i], pets[j])) {
                vector<int> pet = pets[i];
                pets[i] = pets[j];
                pets[j] = pet;
            }
        }
    }
}

int main() {
    createGeneration();
    displayGeneration(1);
    for (int i = 0; i < COUNT_GENERATIONS; ++i) {
        pets = crossingGeneration();
//        displayGeneration(2 + i, "cross");
        mutationGeneration();
//        displayGeneration(2 + i, "mut");
        sortPets();
//        displayGeneration(2 + i, "sort");
        pets = extinctionGeneration();
        displayGeneration(2 + i);
    }
    return 0;
}
