#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <shared_mutex>
#include <mutex>

using namespace std;

string put(vector<std::string> tokens, unordered_map<string, string>&kv_store);
string get(vector<string> tokens, unordered_map<string, string>& kv_store);
string deletee(vector<string> tokens, unordered_map<string, string>& kv_store);