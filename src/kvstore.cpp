#include "kvstore.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <mutex>

// Public method to process a raw command string
std::string KVStore::process_command(const std::string& command)
{
    std::istringstream iss(command);
    std::vector<std::string> tokens;
    std::string word;

    while (iss >> word)
    {
        tokens.push_back(word);
    }

    if (tokens.empty())
    {
        return "ERROR: Empty command";
    }

    if (tokens[0] == "put")
    {
        return put(tokens);
    }

    if (tokens[0] == "get")
    {
        return get(tokens);
    }

    if (tokens[0] == "delete")
    {
        return deletee(tokens);
    }

    return "ERROR: Invalid command";
}

// Private helper methods
std::string KVStore::put(const std::vector<std::string>& tokens)
{
    if (tokens.size() != 3)
    {
        return "ERROR:PUT_INVALID_FORMAT (Usage: put <key> <value>)";
    }

    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_data[tokens[1]] = tokens[2];
    save_to_file();

    return "SUCCESS: Added '" + tokens[2] + "' with key '" + tokens[1] + "'";
}

std::string KVStore::get(const std::vector<std::string>& tokens)
{
    if (tokens.size() != 2)
    {
        return "ERROR:GET_INVALID_FORMAT (Usage: get <key>)";
    }

    std::shared_lock<std::shared_mutex> lock(m_mutex);
    auto it = m_data.find(tokens[1]);

    if (it == m_data.end())
    {
        return "ERROR:KEY_NOT_FOUND '" + tokens[1] + "'";
    }

    return "SUCCESS: " + tokens[1] + " = " + it->second;
}

std::string KVStore::deletee(const std::vector<std::string>& tokens)
{
    if (tokens.size() != 2)
    {
        return "ERROR:DELETE_INVALID_FORMAT (Usage: delete <key>)";
    }

    std::unique_lock<std::shared_mutex> lock(m_mutex);

    if (m_data.erase(tokens[1]) > 0)
    {
        save_to_file();
        return "SUCCESS: Removed '" + tokens[1] + "'";
    }

    return "ERROR:KEY_NOT_FOUND '" + tokens[1] + "'";
}

// Persistence methods
void KVStore::load_from_file()
{
    std::ifstream file(m_filename);

    if (!file.is_open())
    {
        std::cout << "No existing database file found. Starting fresh." << std::endl;
        return;
    }

    std::string line;
    std::unique_lock<std::shared_mutex> lock(m_mutex);

    while (getline(file, line))
    {
        std::stringstream ss(line);
        std::string key, value;

        if (getline(ss, key, ',') && getline(ss, value))
        {
            m_data[key] = value;
        }
    }

    file.close();
    std::cout << "Successfully loaded " << m_data.size() << " pairs from " << m_filename << std::endl;
}

void KVStore::save_to_file()
{
    std::ofstream file(m_filename, std::ios::trunc);

    if (!file.is_open())
    {
        std::cerr << "ERROR: Could not open " << m_filename << " for writing." << std::endl;
        return;
    }

    for (const auto& pair : m_data)
    {
        file << pair.first << "," << pair.second << std::endl;
    }

    file.close();
}
