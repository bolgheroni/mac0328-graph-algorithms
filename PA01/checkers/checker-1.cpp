#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <set>

using namespace std;

const size_t TEST_CASES = 12;

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

		size_t num_var, debug; input >> debug >> num_var;
  	
	  	std::vector<int> sol_to_student;
	  	std::vector<int> student_to_sol;

	  	std::vector<int> students_all_answers;
	  	int nscc = 0;

	  	sol_to_student.resize(2*num_var + 1);
	  	student_to_sol.resize(2*num_var + 1);

	  	std::fill(sol_to_student.begin(), sol_to_student.end(), -1);
	  	std::fill(student_to_sol.begin(), student_to_sol.end(), -1);

	  	bool bijection = true;

	  	for(int i = 0; i < 2*num_var; i++){
	  		int sol, student;
	  		output >> student;
	  		solution >> sol;

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
	  		nscc = std::max(nscc, sol);
	  	}

	  	if(!bijection){
	  		cout << "WRONG - CANNOT FIND A BIJECTION BETWEEN STUDENT'S OUTPUT AND SOLUTION" << endl;
	  	}
	  	else{
	  		for(auto x: students_all_answers){
	  			if(x <= 0 || x > nscc){
	  				bijection = false;
	  				cout << "WRONG - NONPOSITIVE LABEL OR FOUND MORE SCC's" << endl;
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
