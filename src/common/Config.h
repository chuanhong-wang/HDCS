#ifndef HDCS_CONFIG_H
#define HDCS_CONFIG_H

#include <map>
#include <string>
#include <vector>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#define HDCS_OK        0
#define HDCS_ERR      -1
#define HDCS_EAGIN    -2
#define HDCS_ENOMEM   -3


namespace hdcs {

class Config{

public:
    typedef std::map<std::string, std::string> ConfigInfo;
    ConfigInfo configValues{
        {"log_to_file","false"},
        {"rbd_pool_name","rbd"},
        {"rbd_volume_name","volume_1"},
        {"cache_dir","/tmp/"},
        {"cache_mode","write_back"},
        {"total_size","10737418240"},
        {"cache_total_size","10737418240"},
        {"cache_ratio_health","0.85"},
        {"cache_dirty_timeout_nanoseconds", "10000000000"},
        {"cache_min_alloc_size","4096"},
        {"cacheservice_threads_num","64"},
        {"master_ip",  "127.0.0.1"},
        {"slave_ip",  "192.168.5.11"},
        {"messenger_port",  "9090"},
        {"slave_messenger_port",  "9091"}
    };
    Config(std::string name){

        const std::string cfg_file = "general.conf";
        boost::property_tree::ptree pt;
        try {
            boost::property_tree::ini_parser::read_ini(cfg_file, pt);
        } catch(...) {
            std::cout << "error when reading: " << cfg_file
                      << ", config file for missing?" << std::endl;
            // assume general.conf should be created by admin manually
            assert(0);
        }

        std::string s;
        for (ConfigInfo::const_iterator it = configValues.begin(); it!=configValues.end(); it++) {
            try {
                s = pt.get<std::string>(name + "." + it->first);
            } catch(...) {
                try {
                    s = pt.get<std::string>("global." + it->first);
                } catch(...) {
                    continue;
                }
                if ((it->first == "log_to_file")&&(s != "false")) {
                    pt.put(name + "." + it->first, s + "_" + name + ".log");
                    s = s + "_" + name + ".log";
                } else {
                    pt.put(name + "." + it->first, s);
                }
            }
            if (s == "") {
                s = pt.get<std::string>("global." + it->first);
                pt.put(name + "." + it->first, s);
            }
            configValues[it->first] = s;
            std::cout << it->first << " : " << s << std::endl;
            s = "";
        }
        boost::property_tree::ini_parser::write_ini(cfg_file, pt);

        configValues["cache_dir_dev"] = configValues["cache_dir"] + "/" + name + "_cache.data";
        configValues["cache_dir_meta"] = configValues["cache_dir"] + "/" + name + "_meta";
        configValues["cache_dir_run"] = configValues["cache_dir"] + "/" + name + "_run";
    }
    ~Config(){
    }
};

}

#endif
