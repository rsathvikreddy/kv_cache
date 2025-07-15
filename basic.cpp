#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <shared_mutex>
#include <mutex>

using namespace std;

shared_mutex kv_mutex;  // Global mutex for the KV store

string put(vector<string> tokens, unordered_map<string, string>& kv_store) {
    // Validation
    if(tokens.size() != 3) {
        return "ERROR:PUT_INVALID_FORMAT (Usage: put <key> <value>)";
    }

    // Critical section (write operation)
    {
        unique_lock<shared_mutex> lock(kv_mutex);
        kv_store[tokens[1]] = tokens[2];
    }

    return "SUCCESS: Added '" + tokens[2] + "' with key '" + tokens[1] + "'";
}

string get(vector<string> tokens, unordered_map<string, string>& kv_store) {
    // Validation
    if(tokens.size() != 2) {
        return "ERROR:GET_INVALID_FORMAT (Usage: get <key>)";
    }

    string value;
    // Critical section (read operation)
    {
        shared_lock<shared_mutex> lock(kv_mutex);
        auto it = kv_store.find(tokens[1]);
        if(it == kv_store.end()) {
            return "ERROR:KEY_NOT_FOUND '" + tokens[1] + "'";
        }
        value = it->second;
    }

    return "SUCCESS: " + tokens[1] + " = " + value;
}

string deletee(vector<string> tokens, unordered_map<string, string>& kv_store) {
    // Validation
    if(tokens.size() != 2) {
        return "ERROR:DELETE_INVALID_FORMAT (Usage: delete <key>)";
    }

    bool deleted = false;
    // Critical section (write operation)
    {
        unique_lock<shared_mutex> lock(kv_mutex);
        deleted = kv_store.erase(tokens[1]) > 0;
    }

    return deleted ? 
        "SUCCESS: Removed '" + tokens[1] + "'" : 
        "ERROR:KEY_NOT_FOUND '" + tokens[1] + "'";
}

#ifdef TESTING
int main(){
    unordered_map<string, string> kv_store;
    while(true){
        string cmd;
        cout<<"Enter your command : ";
        getline(cin,cmd);
        if(cmd == "exit"){
            break;
        }
        else if(cmd == "help"){
            cout << "Available commands:\n"
              << " - put <key> <value>\n"
              << " - get <key>\n"
              << " - delete <key>\n"
              << " - exit\n";
            continue;
        }
        istringstream iss(cmd);
        string word;
        vector<string> tokens;
        while (iss >> word) {
            tokens.push_back(word);
        }

        if(tokens[0] == "put"){
            put(tokens, kv_store);
        }
        else if(tokens[0] == "get"){
            get(tokens, kv_store);
        }
        else if(tokens[0] == "delete"){
            deletee(tokens, kv_store);
        }
        else{
            cout<<"You have entered wrong command; Please retry"<<endl;
        }
    }


    return 0;
}
#endif