/*
 * Name-Surname: Abdulkadir Pazar
 * Student No: 150180028
 * Date: 03/12/2020
 */

#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<iomanip>
#include<vector>
#include<chrono>

using namespace std;

//Sale struct to hold related data
struct Sale
{
	string country;
	string type;
	string order_id;
	int units_sold;
	float total_profit;

	/*
	 *overloaded smaller than operator
	 *sorts alphabetically if country names are different
	 *sorts in descending order for same country
	 */
	friend bool operator<(const Sale &s1, const Sale &s2)
	{
	if(s1.country != s2.country)
		return (s1.country < s2.country);
	else 
		return (s1.total_profit > s2.total_profit);
	}
};

//quicksort with rightmost element as pivot
void quicksort(Sale* arr, int l, int r)
{
	int i = l, j = r;
    Sale temp; //temp sale struct used for swap
    Sale pivot = arr[r];
    while (i <= j) {
        while (arr[i] < pivot) i++;//skip if values to the left are smaller than pivot
        while (pivot < arr[j]) j--;//skip if values to the right are larger than pivot
        if (i <= j) {
            temp = arr[i];   //
            arr[i] = arr[j]; //swap values
            arr[j] = temp;   //
            i++;             //change indexes after values were swapped
            j--;
        }
    }
    if (l < j) quicksort(arr, l, j);//recursive call for left partition
    if (i < r) quicksort(arr, i, r);//recursive call for right partition
}

int main(int argc, char** argv)
{
	ifstream file;
	file.open("sales.txt");
	if (!file){
		cerr << "File cannot be opened!";
		exit(1);
	}
	int N = stoi(argv[1]);//read n from command line
	string header;
	string endline;
	getline(file, header); //this is the header line

	Sale* sale_arr = new Sale[N];//dynamic array to hold sale data

	for(int i = 0; i<N; i++){//read from file and fill the array
		Sale sale;
		getline(file, sale.country, '\t'); //country (string)
		getline(file, sale.type, '\t'); //item type (string)
		getline(file, sale.order_id, '\t'); //order id (string)
		file >> sale.units_sold; //units sold (integer)
		file >> sale.total_profit; //total profit (float)
	    getline(file, endline, '\n'); //this is for reading the \n character into dummy variable.
		sale_arr[i] = sale;
	}
	file.close();

	//run quicksort and calculate time
	auto start = chrono::high_resolution_clock::now();  
	quicksort(sale_arr, 0, N - 1);
	auto end = chrono::high_resolution_clock::now();

	ofstream sorted("sorted.txt");//write sorted array into sorted.txt
	sorted << header << "\n";//write header line as first line
	for(int i = 0; i < N; i++){
		ostringstream ss;
		ss << fixed << setprecision(2) << sale_arr[i].total_profit; //display float with 2 significant digits
		string str = ss.str();
		sorted << sale_arr[i].country << "\t" << sale_arr[i].type << "\t" << sale_arr[i].order_id << "\t" << sale_arr[i].units_sold << "\t" << str << "\n";
	}
	sorted.close();

	chrono::duration<double>elapsed_time = end-start;
	cout << setprecision(30) <<  "Elapsed time: " << elapsed_time.count();//print elapsed time in seconds.
	delete [] sale_arr; //delete dynamic array to avoid memory leak
	return 0;
}