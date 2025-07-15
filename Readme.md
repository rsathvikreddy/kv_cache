# C++ Key-Value Store

## Overview

This project is a networked, multi-threaded key-value (KV) data store built in C++. It features a client-server architecture that communicates over TCP/IP, allowing multiple clients to concurrently connect and perform Create, Read, Update, and Delete (CRUD) operations. The server is designed for concurrency and data persistence, making it a foundational project for understanding distributed systems and cloud-edge software services.

This project was developed to demonstrate core software engineering principles, including concurrent programming, network communication, and modular design, directly aligning with the skills required for developing robust and scalable software solutions.

## Features

*   **Client-Server Model**: A dedicated server application handles data storage logic, while a separate command-line client interacts with it over the network.
*   **CRUD Functionality**: Supports the fundamental database operations:
    *   `put <key> <value>`: Adds or updates a key-value pair.
    *   `get <key>`: Retrieves the value associated with a key.
    *   `delete <key>`: Removes a key-value pair.
*   **Multi-threaded Server**: The server uses `std::thread` to spawn a new thread for each connected client, enabling it to handle multiple simultaneous connections efficiently.
*   **Thread-Safe Data Access**: Concurrent read/write operations are managed using `std::shared_mutex` to prevent data races. This allows multiple clients to read data simultaneously (`get`) while ensuring exclusive access for write operations (`put`, `delete`), optimizing performance.
*   **Data Persistence**: The key-value store automatically saves its state to a local file (`kvstore.db`) after any modification, ensuring data is retained across server restarts.

## Technology Stack

*   **Language**: C++
*   **Networking**: Winsock2 API for TCP socket programming on Windows.
*   **Concurrency**: C++ Standard Library (`<thread>`, `<shared_mutex>`).
*   **Data Structures**: `std::unordered_map` for efficient O(1) average time complexity on key lookups.
*   **File I/O**: C++ Standard Library (`<fstream>`) for data persistence.

## Project Structure

The codebase is organized into logical components to promote modularity and separation of concerns:

*   `server.cpp`: The entry point for the server application. It initializes Winsock, listens for incoming TCP connections, and spawns a new thread for each client.
*   `client.cpp`: The entry point for the client application. It connects to the server and provides a command-line interface for users to send commands.
*   `kvstore.h` / `kvstore.cpp`: A self-contained class that encapsulates all the business logic for the key-value store. It manages the in-memory data, handles thread-safe access, and serializes data to a file. This separation ensures that the core logic is independent of the networking layer.

## How to Build and Run

This project is designed for a Windows environment and can be compiled using g++.

### Prerequisites

*   A C++ compiler that supports C++11 or later (for `std::thread`, `std::shared_mutex`). MinGW-w64 (g++) is a good option.

### Compilation

Open a terminal or command prompt and run the following commands to build the server and client executables:

```bash
# Compile the server
g++ src/server.cpp src/kvstore.cpp -o server -lws2_32

# Compile the client
g++ src/client.cpp -o client -lws2_32
```

### Execution

1.  **Start the Server**: Run the compiled server executable. It will load any existing data from `kvstore.db` and start listening for connections on port 9000.
    ```bash
    ./server.exe
    ```
    You should see the output: `Server running on port 9000...`

2.  **Start the Client**: Open one or more new terminal windows and run the client executable.
    ```bash
    ./client.exe
    ```
    The client will connect to the server, and you can start sending commands.

## How to Use

The client provides a simple command-line interface. Enter commands in the format `<command> <key> [value]`.

*   **Add/Update a value:**
    ```
    Enter the command: put name john
    Server: SUCCESS: Added 'john' with key 'name'
    ```

*   **Retrieve a value:**
    ```
    Enter the command: get name
    Server: SUCCESS: name = john
    ```

*   **Delete a value:**
    ```
    Enter the command: delete name
    Server: SUCCESS: Removed 'name'
    ```

*   **Attempt to get a non-existent key:**
    ```
    Enter the command: get name
    Server: ERROR:KEY_NOT_FOUND 'name'
    ```

*   **Exit the client:**
    ```
    Enter the command: exit
    ```