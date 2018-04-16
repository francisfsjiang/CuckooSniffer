#include "option_parser.hpp"

#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "cuckoo_sniffer.hpp"


namespace cs::util {


const int k_HELP_DESC_NUM = 4;

const char* k_HELP_DESC[k_HELP_DESC_NUM][2] = {
        {"help,h",                      "help message"                  },
        {"config-file,c",               "set compression level"         },
        {"interface",                   "set client's ip bind address"  },
        {"submit_url",                  "set client's ip bind address"  },
};

void parse_variables_to_map(std::map<std::string, std::string>& m, const boost::program_options::variables_map& vm) {
    for(int i = 0; i < k_HELP_DESC_NUM; ++i) {
        if (vm.count(k_HELP_DESC[i][0])) {
            m[k_HELP_DESC[i][0]] = vm[k_HELP_DESC[i][0]].as<std::string>();
        }
    }
}

int parse_cfg(int& argc, const char** &argv, std::map<std::string, std::string>& parsed_cfg) {

    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
            (k_HELP_DESC[0 ][0],                                               k_HELP_DESC[0][1])
            (k_HELP_DESC[1 ][0], boost::program_options::value<std::string>(), k_HELP_DESC[1 ][1])
            (k_HELP_DESC[2 ][0], boost::program_options::value<std::string>(), k_HELP_DESC[2 ][1])
            (k_HELP_DESC[3 ][0], boost::program_options::value<std::string>(), k_HELP_DESC[3 ][1]);

    boost::program_options::positional_options_description p;
    p.add("interface", -1);

    try {
        boost::program_options::variables_map vm;
        boost::program_options::store(
                boost::program_options::command_line_parser(argc, argv).
                options(desc).positional(p).run(), vm
        );

        boost::program_options::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << "\n";
            return -1;
        }

        if (vm.count("config-file")) {
            boost::program_options::variables_map config_vm;

            boost::filesystem::path config_file_path(vm["config-file"].as<std::string>());

            if (boost::filesystem::exists(config_file_path)) {
                std::ifstream config_file(config_file_path.string());
                boost::program_options::store(
                        boost::program_options::parse_config_file(config_file, desc), config_vm
                );

                boost::program_options::notify(config_vm);

                config_file.close();

                parse_variables_to_map(parsed_cfg, config_vm);
            }
            else {
            std::cout << "error: config-file not found" << std::endl;
            return -1;
            }
        }

        parse_variables_to_map(parsed_cfg, vm);

        return 0;
    }
    catch(std::exception& e) {
        LOG_ERROR << "Error value: " << e.what() << std::endl;
        LOG_ERROR << desc << std::endl;
        return -1;
    }
    catch(...) {
        LOG_ERROR << "Exception of unknown type!\n";
        return -1;
    }
}


}
