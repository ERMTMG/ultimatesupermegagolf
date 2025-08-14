#include"simdjson.h"
#include<iostream>

using namespace simdjson;

int main(int argc, char** argv){
    if(argc < 2){
        std::cerr << "Expected at least one .json file.\n";
        exit(-1);
    }

    ondemand::parser json_parser;
    for(int arg_idx = 1; arg_idx < argc; arg_idx++){
        char* arg_i = argv[arg_idx];
        padded_string json = padded_string::load(arg_i);
        auto json_doc = json_parser.iterate(json);
        if(json_doc.type() != ondemand::json_type::object){
            std::cerr << "Error parsing " << arg_i << ": The .json file must be an object\n";
            continue;
        }
        std::cout << arg_i << " file analysis: \n";
        for(auto key : json_doc.get_object()){
            std::cout << '\t' << key.escaped_key() << " => ";
            auto val = key.value();
            switch(val.type()){
              case ondemand::json_type::object: {
                auto obj = val.get_object().value();
                std::cout << "{Object with " << (*obj.begin()).escaped_key() << " as 1st key}";
                break;
              };
              case ondemand::json_type::array: {
                auto arr = val.get_array().value();
                std::cout << "{Array with length " << arr.count_elements().value() << "}";
                break;
              };
              case ondemand::json_type::string: {
                auto str = val.get_string().value();
                std::cout << "{String \"" << str << "\"}";
                break;
              };
              case ondemand::json_type::number: {
                auto num = val.get_number().value();
                std::cout << "{Number " << num.as_double() << "}";
                break;
              };
              case ondemand::json_type::boolean: {
                auto boolean = val.get_bool().value();
                std::cout << (boolean ? "{True}" : "{False}");
                break;
              };
              case ondemand::json_type::null: {
                std::cout << "{Null}";
                break;
              };
            }
            std::cout << '\n';
        }
    }
}