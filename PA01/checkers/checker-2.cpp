#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <set>

using namespace std;

const size_t TEST_CASES = 12;

bool valid(int x, int n){
	if(x > 0)
		return x >= 1 && x <= n;
	if(x < 0){
		return x >= -n && x <= -1;
	}
	return false;
}

bool valid(pair<int, int> x,  int n){
	return valid(x.first, n) && valid(x.second, n);
}

int main(){
	size_t correct = 0;
  	for (int testcase = 1; testcase <= TEST_CASES; ++testcase) {
    	stringstream ss;
    	ss << std::setfill('0') << std::setw(2) << testcase;
		string test = ss.str();

		cout << "Testcase " << test << ": ";

		string output_name = "out";
		string solution_name = "sol";
		string input_name = "in";

		output_name += test;
		solution_name += test;
		input_name += test;

		ifstream output, solution, input;

		output.open(output_name);
		solution.open(solution_name);
		input.open(input_name);

		int d, num_var, num_clauses;
		input >> d >> num_var >> num_clauses;
  	
  		set<pair<int, int>> sol_arcs;
  		set<pair<int, int>> student_arcs;

 	 	size_t num_vertices_sol, num_arcs_sol;
 	 	solution >> num_vertices_sol >> num_arcs_sol;

	  	for(int i = 0; i < num_arcs_sol; i++){
	  		int a, b; solution >> a >> b;
	  		sol_arcs.insert({a, b});
	  	}


	  	size_t num_vertices_student, num_arcs_student;
	  	output >> num_vertices_student >> num_arcs_student;

	  	for(int i = 0; i < num_arcs_student; i++){
	  		int a, b; output >> a >> b;
	  		student_arcs.insert({a, b});
	  	}


	  	bool ok = true;
	  	if(num_vertices_student != 2*num_var || num_arcs_student > 2*num_clauses){
	  		ok = false;
	  		cout << "WRONG - INCORRECT NUMBER OF VERTICES OR ARCS" << endl;
	  	}
	  	else{
	  		for(auto x: sol_arcs){
	  			if(student_arcs.find(x) == student_arcs.end()){
	  				ok = false;
	  				cout << "WRONG - THERE IS A MISSING ARC" << endl;
	  				break;
	  			}
	  		}
	  		if(ok){
	  			for(auto x: student_arcs){
	  				if(sol_arcs.find(x) == sol_arcs.end()){
	  					ok = false;
	  					cout << "WRONG - THERE IS AN EXTRA ARC" << endl;
	  					break;
	  				}
	  				if(!valid(x, num_var)){
	  					ok = false;
	  					cout << "WRONG - THERE IS AN INVALID ARC" << endl;
	  					break;
	  				}
	  			}
	  		}
	  	}

	  	if(ok){
	  		cout << "CORRECT - SAME SET OF VERTICES AND ARCS" << endl;
	  		correct++;
	  	}
	}

	cout << correct << "/" << TEST_CASES << " Correct test cases" << endl;

	return 0;
}
