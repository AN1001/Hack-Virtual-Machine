#include <iostream>
#include <array>
#include <string>
#include <fstream>
#include <unordered_map>

int parse_a_instruction(const std::string& binary) {
  std::string trimmed = binary.substr(1); // Remove first bit
  return std::stoi(trimmed, nullptr, 2);  // Convert to integer
}


std::array<int, 3> simulate_alu(int x, int y, const std::string& alu_control) {
  bool zx = alu_control[0] == '1'; // Zero the x input
  bool nx = alu_control[1] == '1'; // Negate the x input
  bool zy = alu_control[2] == '1'; // Zero the y input
  bool ny = alu_control[3] == '1'; // Negate the y input
  bool f  = alu_control[4] == '1'; // Function code: 1 for add, 0 for and
  bool no = alu_control[5] == '1'; // Negate the output
  
  // Zero inputs if zx or zy are set
  if (zx) x = 0;
  if (zy) y = 0;
  
  // Negate inputs if nx or ny are set
  if (nx) x = ~x;
  if (ny) y = ~y;
  
  // Perform the function
  int result = f ? (x + y) : (x & y);
  
  // Negate output if no is set
  if (no) result = ~result;
  
  //Output, Zero?, Negative?
  return {result, (result == 0) ? 1 : 0, (result < 0) ? 1 : 0};
}

int main(){
	const std::string path = "./Desktop/proj_euler/";
  std::cout << "Assuming path is " << path << std::endl <<"Enter hack asm to execute: ";
  std::string inFileName;
  std::cin >> inFileName;
  
  std::ifstream file(path + inFileName);
  
  if(!file.is_open()) {
    std::cerr << "Error, could not find file; ensure you have read the readme.md" << std::endl;
    return 1;
  }

	bool jump = false;
	std::string line;
	std::array<int, 3> alu_output;
	std::unordered_map<int, int> memory;
	int a_register = 0;
	int d_register = 0;

	int breakout = 0;
	while(file >> line){
		jump = false;
		breakout++;
		if(breakout>10000){
			std::cout<<"Force Quit - too long to finish execution" << std::endl;
			break;
		}

		if(line[0] == '0'){
			a_register = parse_a_instruction(line);
		} else {
			//C-instruction
			if(line[3] == '0'){
				alu_output = simulate_alu(d_register, a_register, line.substr(4, 6));
			} else {
				alu_output = simulate_alu(d_register, memory[a_register], line.substr(4, 6));
			}

			//Where to store ALU output?
			if(line[10]=='1'){
				a_register = alu_output[0];
			}
			if(line[11]=='1'){
				d_register = alu_output[0];
			}
			if(line[12]=='1'){
				memory[a_register] = alu_output[0];
			}

			//Should it Jump?
			jump = (line[13]=='1' && alu_output[1]) || (line[14]=='1' && alu_output[2]) || (line[15]=='1' && !(alu_output[1]||alu_output[2]));
			if(jump){
				file.seekg(17 * a_register);
			}
			
		}
	}

	std::cout<<"Memory:"<<std::endl;
	for (auto it = memory.begin(); it != memory.end(); ++it) {
        	std::cout << "Address: " << it->first << ", Value: " << it->second << std::endl;
    	}
}
