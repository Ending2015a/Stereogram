#pragma once

#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include <cctype>
#include <cstring>



namespace ending{
	class ArgParser{
	private:
		std::map<std::string, bool> _valid;
		std::map<std::string, std::string> _value;

		int _error;
		std::string _errormsg;

	public:

		ArgParser(){
			clear();
		}

		ArgParser(std::vector<std::string> commands, int argc, char *argv[]){
			clear();


			setCommand(commands);
			parse(argc, argv);
		}

		int parse(int argc, char *argv[]){
			for (int i = 1; i<argc; i++){
				std::string param = std::string(argv[i]);
				std::string lparam = param.substr(0, param.find('='));
				std::string rparam = param.substr(param.find('=')+1, std::string::npos);
				if (_valid.find(lparam) == _valid.end()){
					_error = 1;
					_errormsg = std::string("Undefined parameter: ") + param;
					return _error;
				}
				else{
					_valid[lparam] = true;
					_value[lparam] = rparam;
				}
			}
			return 0;
		}

		int getErrorType(){
			return _error;
		}

		std::string getErrorMsg(){
			return _errormsg;
		}

		void setCommand(std::vector<std::string> commands){
			for (size_t i = 0; i<commands.size(); i++){
				_valid[commands[i]] = false;
			}
		}

		void clear(){
			_valid.clear();
			_value.clear();
			_error = 0;
			_errormsg = "";
		}

		std::string getString(std::string command, std::string _default = ""){
			if (_valid[command] == false)return _default;
			else return _value[command];
		}

		int getInt(std::string command, int _default = 0){
			std::string v = getString(command, "");
			return (v != "") ? stoi(v) : _default;
		}

		double getDouble(std::string command, int _default = 0){
			std::string v = getString(command, "");
			return (v != "") ? stod(v) : _default;
		}

		bool getValid(std::string command){
			if (_valid[command] == false)return false;
			return true;
		}


	};
}