#include "gtest/gtest.h"  // The main header for the Google Test framework
#include "kvstore.h"      // The class we want to test
#include <string>

// This is a "Test Fixture". It's a helper class that sets up a clean
// environment for each test. This ensures that one test cannot
// accidentally affect another.
class KVStoreTest : public ::testing::Test {
protected:
    // This function is called after each test finishes
    void TearDown() override {
        // We delete the database file to ensure the next test starts fresh.
        remove("kvstore.db");
    }

    // We declare a KVStore object here. Each test will get its own fresh instance.
    KVStore store;
};

// TEST_F is how you define a test using a fixture (KVStoreTest).
// The first argument is the name of our test group (KVStoreTest).
// The second is the specific name of this test (HandlesBasicPutAndGet).
TEST_F(KVStoreTest, HandlesBasicPutAndGet) {
    std::string response;
    
    // 1. Action: Try to put a value.
    response = store.process_command("put key1 value1");
    // 2. Check: Assert that the response is exactly what we expect.
    ASSERT_EQ(response, "SUCCESS: Added 'value1' with key 'key1'");

    // 3. Action: Try to get the value back.
    response = store.process_command("get key1");
    // 4. Check: Assert that we got the correct value.
    ASSERT_EQ(response, "SUCCESS: key1 = value1");
}

// A new test to check how we handle getting a key that doesn't exist.
TEST_F(KVStoreTest, HandlesGetOnNonExistentKey) {
    std::string response = store.process_command("get non_existent_key");
    ASSERT_EQ(response, "ERROR:KEY_NOT_FOUND 'non_existent_key'");
}

// A new test to check the delete functionality.
TEST_F(KVStoreTest, HandlesDelete) {
    // First, add a key so we have something to delete
    store.process_command("put key_to_delete value");
    
    // Now, delete it
    std::string response = store.process_command("delete key_to_delete");
    ASSERT_EQ(response, "SUCCESS: Removed 'key_to_delete'");

    // Finally, verify it's gone by trying to get it again
    response = store.process_command("get key_to_delete");
    ASSERT_EQ(response, "ERROR:KEY_NOT_FOUND 'key_to_delete'");
}

// A test for various invalid commands to make sure they return errors.
TEST_F(KVStoreTest, HandlesInvalidCommandFormats) {
    // We use ASSERT_NE (Assert Not Equal) combined with find()
    // to check if the response string contains the word "ERROR:".
    ASSERT_NE(store.process_command("put key_only").find("ERROR:"), std::string::npos);
    ASSERT_NE(store.process_command("get").find("ERROR:"), std::string::npos);
    ASSERT_NE(store.process_command("delete").find("ERROR:"), std::string::npos);
    ASSERT_EQ(store.process_command("unknown_command"), "ERROR: Invalid command");
}

// A test to explicitly check if our file persistence is working.
TEST_F(KVStoreTest, HandlesPersistence) {
    // Add data using our first 'store' object. This will create 'kvstore.db'.
    store.process_command("put persistent_key persistent_value");
    
    // Now, create a completely NEW, second KVStore instance.
    KVStore new_store;
    // Tell the new store to load data from the file.
    new_store.load_from_file();

    // Check if the new store has the data that the first store saved.
    std::string response = new_store.process_command("get persistent_key");
    ASSERT_EQ(response, "SUCCESS: persistent_key = persistent_value");
}