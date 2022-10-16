#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>
#include <set>

using namespace std;

const size_t TEST_CASES = 12;

set<pair<int, int>> clauses;
set<int> vis;

bool verify(pair<int, int> x, bool f, bool s){
	if(x.first < 0) f = !f;
	if(x.second < 0) s = !s;

	return (f | s);
}

bool verify(vector<int>& assignment){
	for(auto x: clauses){
		if(!verify(x, assignment[abs(x.first)], assignment[abs(x.second)]))
			return false;
	}
	return true;
}

int valid(std::vector<int>& path){
	for(int i = 0; i < path.size() - 1; i++){
		int at = path[i], prox = path[i + 1];
		bool found = false;
		if(clauses.find({-at, prox}) != clauses.end() || clauses.find({prox, -at}) != clauses.end()) found = true;
		if(!found) return 3;
	}
	return 0;
}

int is_path(int start, int end, std::vector<int>& path){
	if(path[0] != start || path[path.size() - 1] != end) return 1;
	for(auto x: path){
		if(vis.find(x) != vis.end()) return 2;
		vis.insert(x);
	}

	return valid(path);
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

		size_t num_clauses, num_var, debug; input >> debug >> num_var >> num_clauses;
		clauses.clear();
		for(int i = 0; i < num_clauses; i++){
			int a, b; input >> a >> b;
			clauses.insert({a, b});
		}

		string sol_verdict; solution >> sol_verdict;
		string student_verdict; output >> student_verdict;

		if(sol_verdict != student_verdict){
			cout << "WRONG - INCORRECT VERDICT" << endl;
			continue;
		}

		if(student_verdict == "YES"){
			vector<int> assignment;
			assignment.clear();
			assignment.push_back(-1);
			for(int i = 0; i < num_var; i++){
				bool a; output >> a;
				assignment.push_back(a);
			}
			if(verify(assignment)){
				correct++;
				cout << "CORRECT - VALID TRUTH ASSIGNMENT" << endl;
			}
			else{
				cout << "WRONG - INVALID TRUTH ASSIGNMENT" << endl;
			}
		}
		else{
			int off_vtx; output >> off_vtx;
			std::vector<int> path, rpath;
			int path_size, rpath_size;
			output >> path_size;
			for(int i = 0; i <= path_size; i++){
				int var; output >> var;
				path.push_back(var);
			}
			output >> rpath_size;
			for(int i = 0; i <= rpath_size; i++){
				int var; output >> var;
				rpath.push_back(var);
			}
			vis.clear();
			int go = is_path(off_vtx, -off_vtx, path);
			vis.clear();
			int rgo = is_path(-off_vtx, off_vtx, rpath);

			if(go == 1 || rgo == 1)
				cout << "WRONG - PATH ENDS ARE INCORRRECT" << endl;
			else if(go == 2 || rgo == 2)
				cout << "WRONG - REPEATED VERTICES IN PATH" << endl;
			else if(go == 3 || rgo == 3)
				cout << "WRONG - PATH USES ARC THAT DOES NOT EXIST" << endl;
			else{
				correct++;
				cout << "CORRECT - UNSATISFIABLE INSTANCE" << endl;
			}
		}
  	
	  	
  	}
	
	cout << correct << "/" << TEST_CASES << " Correct test cases" << endl;

	return 0;
}
