/*
 *  Name-Surname: Abdulkadir Pazar
 *	Student Number: 150180028
 *  Date: 21.12.2020
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <cmath>
#include <chrono>

#define HOTEL_X 33.40819//hotel longitude
#define HOTEL_Y 39.19001//hotel latitude

using namespace std;

class MinHeap
{
    vector<float> arr;

public:
    MinHeap(int m){arr.reserve(m);}//constructor
    void minHeapify(unsigned int);//recursively heapify the subtree with given index as root
    void insert(double);//insert given element to the appopriate place in the heap
    void decreaseKey(unsigned int);//decrease the element in given index by 0.01 and move it to the appopriate place in heap
    double popMin();//remove the root of the heap and minheapify the remaining elements
    int getSize(){return arr.size();}//returns the size of the heap
};

void MinHeap::minHeapify(unsigned int index)
{
    unsigned int left_index = 2 * index + 1;//left child of the root
    unsigned int right_index = 2 * index + 2;//right child of the root

    unsigned int min_index = index;

    if (left_index < arr.size() && arr[left_index] < arr[index]) //if root is bigger than left child
        min_index = left_index; //min index becomes left child

    if (right_index < arr.size() && arr[right_index] < arr[min_index]) //if root is bigger than right child
        min_index = right_index; //min index becomes right child

    if (min_index != index){ //if subtree didnt become a minheap yet
        double temp = arr[index];//swap element at index with element at min_index
        arr[index] = arr[min_index];
        arr[min_index] = temp;

        minHeapify(min_index);//call function on min_index
    }
}

void MinHeap::insert(double element)
{
    arr.push_back(element);//place element to last index

    unsigned int i = arr.size() - 1;//index of the inserted element of the heap

    while (i > 0 && arr[i] < arr[(i - 1) / 2]){//percolate up operation
        double temp = arr[(i - 1) / 2]; //swap inserted element with its parent
        arr[(i - 1) / 2] = arr[i];
        arr[i] = temp;

        i = (i - 1) / 2; 
    }
}

void MinHeap::decreaseKey(unsigned int index)
{
    if (arr[index] < 0.01){//if distance of the taxi is already less than 0.01, skip operation
        return;
    }

    arr[index] -= 0.01;//decrease distance

    while (index > 0 && arr[index] < arr[(index - 1) / 2]){//percolate up operation
        double temp = arr[(index - 1) / 2]; //swap decreased element with its parent
        arr[(index - 1) / 2] = arr[index];
        arr[index] = temp;

        index = (index - 1) / 2;
    }
}

double MinHeap::popMin()
{
    if (arr.size() == 0){
        cerr << "EMPTY HEAP!" << endl;
        exit(1);
    }

    double min_element = arr[0];//index of the root
    double last_element = arr[arr.size() - 1];
    arr.pop_back();//remove last element

    if (arr.size() > 0){//if the heap didnt become empty heapify the remaining values
        arr[0] = last_element;//set last element to root
        minHeapify(0);//call minheapify from the new root
    }
    return min_element;//return value of the removed root
}

int main(int argc, char** argv)
{
    auto start = chrono::high_resolution_clock::now();
    ifstream in("locations.txt");

    if (!in){
        cerr << "CANNOT FIND FILE!" << endl;
        return 1;
    }
    if (argc < 3){
        cerr << "NOT ENOUGH ARGUMENTS!" << endl;
        return 1;
    }
    if (stod(argv[2]) > 1 || stod(argv[2]) < 0 || stoi(argv[1]) > 1000000 || stoi(argv[1]) < 0){
        cerr << "ARGUMENTS ARE NOT IN APPOPRIATE RANGE!" << endl;
        return 1;
    }

    unsigned int m = stoi(argv[1]);//number of operations
    double p = stod(argv[2]);//probability of addition operation

    string s;//to read header line and endlines of each line
    getline(in, s); //read header

    MinHeap distances(m);

    vector<float> called_taxis;//vector to keep called taxis
    called_taxis.reserve(m / 100);

    int taxi_addition_count = 0;
    int distance_update_count = 0;

    //probability simulation
    default_random_engine generator;
    bernoulli_distribution distrubition(p);
 
    for (unsigned int i = 0; i < m; i++){
        if (((i + 1) % 100) == 0)//remove closest taxi once every 100 operations
            called_taxis.push_back(distances.popMin());
        else{
            if (distrubition(generator)){//probability of being an update
                if (distances.getSize() > 0){//if heap isn't empty
                    uniform_int_distribution<int> distrib_size(0, distances.getSize() - 1);
                    int random_index = distrib_size(generator);//get a random index
                    distances.decreaseKey(random_index);//decrease distance of a random taxi by 0.01
                    distance_update_count++;
                }
            }else{//probability of being an addition
                double x, y;
                in >> x;//read longitude
                in >> y;//read latitude
                getline(in, s, '\n');//read endline

                double distance = sqrt((x - HOTEL_X) * (x - HOTEL_X) + (y - HOTEL_Y) * (y - HOTEL_Y));//calculate distance
                distances.insert(distance);//add taxi to heap
                taxi_addition_count++;
            }
        }
    }
    in.close();

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

    cout << "Distances of called taxis" << endl;

    if (called_taxis.size() > 0){
        for (unsigned int i = 0; i < called_taxis.size(); i++)
            cout << called_taxis[i] << endl;
    }else
        cout << "No taxi has been called" << endl;

    cout << "The number of taxi additions: " << taxi_addition_count << endl;
    cout << "The number of taxi distance updates: " << distance_update_count << endl;
    cout << "Program execution time: " << duration.count() / 1000 << " ms" << endl;

    return 0;
}