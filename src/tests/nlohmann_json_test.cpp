#include<iostream>
#include<fstream>
#include"nlohmann/json.hpp"

using json = nlohmann::json;

int main(int argc, char** argv){
    if(argc < 2){
        std::cerr << "Expected at least one .json file.\n";
        exit(-1);
    }
    for(int arg_idx = 1; arg_idx < argc; arg_idx++){
        char* arg_i = argv[arg_idx];
        std::ifstream json_file{arg_i};
        try {
            json json_data = json::parse(json_file);
            if(!json_data.is_object()){
                std::cerr << "File " << arg_i << " doesn't correspond to a JSON object\n";
                continue;
            }
            std::cout << "Analyzing file " << arg_i << "...\n";
            for(auto& [key, value] : json_data.items()){
                std::cout << "\t" << key << " => ";
                switch(value.type()){
                  case json::value_t::null: {
                    std::cout << "{Null}\n";
                    break;
                  }
                  case json::value_t::boolean: {
                    std::cout << (value.get<bool>() ? "{True}\n" : "{False}\n");
                    break;
                  }
                  case json::value_t::number_float:
                  case json::value_t::number_integer:
                  case json::value_t::number_unsigned: {
                    std::cout << "{Number " << value.get<double>() << "}\n";
                    break;
                  }
                  case json::value_t::string: {
                    std::cout << "{String \"" << value.get<std::string>() << "\"}\n";
                    break;
                  }
                  case json::value_t::array: {
                    std::cout << "{Array of size " << value.size() << "}\n";
                    break;
                  }
                  case json::value_t::object: {
                    std::cout << "{Object with keys: \n";
                    for(auto& [inner_key, _] : value.items()){
                        std::cout << "\t\t-" << inner_key << "\n";
                    }
                    std::cout << "\t}\n";
                  }
                }
            }
        } catch(const json::parse_error& err){
            std::cerr << "Error parsing file " << arg_i << " (" << err.what() << ")\n";
        }
    }
}