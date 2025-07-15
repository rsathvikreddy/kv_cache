#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <shared_mutex>

class KVStore {
public:
    // The main entry point for processing any command
    std::string process_command(const std::string& command);

    // Loads data from the persistence file
    void load_from_file();

private:
    // These functions now become private helpers
    std::string put(const std::vector<std::string>& tokens);
    std::string get(const std::vector<std::string>& tokens);
    std::string deletee(const std::vector<std::string>& tokens);

    // Saves data to the persistence file
    void save_to_file();

    // Member variables: The data store and its mutex are now part of the class
    std::unordered_map<std::string, std::string> m_data;
    std::shared_mutex m_mutex;
    const std::string m_filename = "kvstore.db";
};