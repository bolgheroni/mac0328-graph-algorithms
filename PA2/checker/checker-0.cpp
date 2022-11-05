#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <set>

using namespace std;

const size_t TEST_CASES = 12;

int parse(string s){
	stringstream ss (s);
	string k;
	while(getline(ss, k, ' '));
	return stoi(k);
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

		size_t debug, n, m; input >> debug >> n >> m;
  	
	  	std::vector<int> sol_to_student;
	  	std::vector<int> student_to_sol;

	  	std::vector<int> students_all_answers;
	  	int bcc = 0;

	  	sol_to_student.resize(m + 1);
	  	student_to_sol.resize(m + 1);

	  	std::fill(sol_to_student.begin(), sol_to_student.end(), -1);
	  	std::fill(student_to_sol.begin(), student_to_sol.end(), -1);

	  	bool bijection = true;

	  	for(int i = 0; i < m; i++){
	  		string sol_str, student_str;
	  		int sol, student;

	  		getline(output, student_str);
	  		getline(solution, sol_str);
	  		
	  		student = parse(student_str);
	  		sol = parse(sol_str);

	  		if(sol_to_student[sol] != -1){
	  			if(sol_to_student[sol] != student){
	  				bijection = false;
	  			}
	  		}
	  		else{
	  			sol_to_student[sol] = student;
	  		}

	  		if(student_to_sol[student] != -1){
	  			if(student_to_sol[student] != sol){
	  				bijection = false;
	  			}
	  		}
	  		else{
	  			student_to_sol[student] = sol;
	  		}

	  		students_all_answers.push_back(student);
	  		bcc = std::max(bcc, sol);
	  	}

	  	if(!bijection){
	  		cout << "WRONG - CANNOT FIND A BIJECTION BETWEEN STUDENT'S OUTPUT AND SOLUTION" << endl;
	  	}
	  	else{
	  		for(auto x: students_all_answers){
	  			if(x <= 0 || x > bcc){
	  				bijection = false;
	  				cout << "WRONG - NONPOSITIVE LABEL OR FOUND MORE BCC's" << endl;
	  				break;
	  			}
	  		}	  		
	  	}
  		if(bijection){
  			correct++;
  			cout << "CORRECT - BIJECTION FOUND" << endl;
  		}
  	}
	
	cout << correct << "/" << TEST_CASES << " Correct test cases" << endl;

	return 0;
}
