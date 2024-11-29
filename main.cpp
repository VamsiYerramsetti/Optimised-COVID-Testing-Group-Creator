#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>
#include <numeric>

using namespace std;

using Connection = tuple<int, int>;

// Structure to hold input data for each test case
struct Data {
    int people_count;             // Total number of people
    int connection_count;         // Number of connections (edges)
    int initial_infected;         // Number of initially infected people
    double spread_chance;         // Probability of disease spread
    int lower_bound;              // Lower bound for infected people range
    int upper_bound;              // Upper bound for infected people range
    vector<Connection> connections;  // List of connections between people
};

// Global variables for tracking test counts
int round_tests_needed = 0;   // Number of tests performed in the current round
int total_tests_needed = 0;   // Total tests performed across all rounds

// Function declarations for all operations
void receive_data(Data& data);                          // Reads input data for each test case
void print_data(const Data& data);                      // Prints test case data for debugging
bool test_group(const vector<int>& group);              // Tests a group of people for infection
bool test_person(int person);                           // Tests a single person for infection
void remove_item(vector<int>& v, int item);             // Removes a person from the list
bool item_exists(const vector<int>& v, int item);       // Checks if a person exists in the list
void create_groups_simple(vector<int>& possible_infected, int group_size, vector<vector<int>>& groups);  // Creates fixed-size groups for testing
void group_testing_solver_optimal(const Data& data, vector<int>& infected);  // Solves the problem with optimal group testing
void group_testing_solver_fixed(const Data& data, vector<int>& infected, int group_size);  // Solves with fixed group size
bool submit_answer(const vector<int>& infected);        // Submits the list of infected people for validation
void log_failed_attempts(const vector<pair<Data, vector<int>>>& failed_attempts);  // Logs failed test case results

// Function definitions

/**
 * Receives input data for a test case and stores it in the data structure.
 * 
 * Arguments:
 *   - data: A reference to a Data object where the test case data will be stored.
 * 
 * Time Complexity: O(m), where m is the number of connections (data.connection_count).
 * This is because we loop through each connection and read the input.
 */
void receive_data(Data& data) {
    cin >> data.people_count >> data.connection_count >> data.initial_infected 
        >> data.spread_chance >> data.lower_bound >> data.upper_bound;

    // Read the list of connections between people
    data.connections.resize(data.connection_count);
    for (int i = 0; i < data.connection_count; ++i) {
        int a, b;
        cin >> a >> b;
        data.connections[i] = {a, b};
    }
}

/**
 * Prints the data of the test case for debugging purposes.
 * 
 * Arguments:
 *   - data: A reference to a Data object whose information will be printed.
 * 
 * Time Complexity: O(1), as it only involves printing the values of the data structure.
 */
void print_data(const Data& data) {
    cerr << "People: " << data.people_count 
         << ", Connections: " << data.connection_count
         << ", Initial Infected: " << data.initial_infected
         << ", Spread Chance: " << data.spread_chance
         << ", Infected Bounds: " << data.lower_bound << "-" << data.upper_bound << endl;
}

/**
 * Tests a group of people and returns true if the group contains an infected person.
 * 
 * Arguments:
 *   - group: A vector of integers representing the IDs of the people in the group.
 * 
 * Returns:
 *   - true if the group contains an infected person, false otherwise.
 * 
 * Time Complexity: O(g), where g is the size of the group being tested.
 * We need to print the group and process the response, which takes linear time in terms of group size.
 */
bool test_group(const vector<int>& group) {
    if (group.empty()) return false;

    // Output the group to be tested
    cout << "test";
    for (int person : group) {
        cout << " " << person;
    }
    cout << endl;

    // Get the response (true if the group has infected, false otherwise)
    string response;
    getline(cin, response);

    // Track the number of tests
    round_tests_needed++;
    total_tests_needed++;
    return response == "true";
}

/**
 * Tests a single person and returns true if the person is infected.
 * 
 * Arguments:
 *   - person: An integer representing the ID of the person to be tested.
 * 
 * Returns:
 *   - true if the person is infected, false otherwise.
 * 
 * Time Complexity: O(1), as it only involves a single test operation.
 */
bool test_person(int person) {
    // Output the person to be tested
    cout << "test " << person << endl;

    // Get the response (true if the person is infected, false otherwise)
    string response;
    getline(cin, response);

    // Track the number of tests
    round_tests_needed++;
    total_tests_needed++;
    return response == "true";
}

/**
 * Removes a specific item from the list (e.g., removes a person from the potential infected list).
 * 
 * Arguments:
 *   - v: A reference to a vector of integers representing the list of people.
 *   - item: The integer ID of the person to remove from the list.
 * 
 * Time Complexity: O(n), where n is the size of the list.
 * This is because remove may need to shift elements to close the gap after the item is removed.
 */
void remove_item(vector<int>& v, int item) {
    v.erase(remove(v.begin(), v.end(), item), v.end());
}

/**
 * Checks if a person exists in the list.
 * 
 * Arguments:
 *   - v: A vector of integers representing the list of people.
 *   - item: The integer ID of the person to search for in the list.
 * 
 * Returns:
 *   - true if the person exists in the list, false otherwise.
 * 
 * Time Complexity: O(n), where n is the size of the list.
 * This is because we need to scan through the entire list to check for the existence of the item.
 */
bool item_exists(const vector<int>& v, int item) {
    return find(v.begin(), v.end(), item) != v.end();
}

/**
 * Creates fixed-size groups of people for testing.
 * 
 * Arguments:
 *   - possible_infected: A reference to a vector of integers representing the list of possible infected people.
 *   - group_size: The fixed size of the groups to be created.
 *   - groups: A reference to a 2D vector of integers where the created groups will be stored.
 * 
 * Time Complexity: O(n), where n is the number of possible infected people.
 * We iterate through the list of possible infected people in increments of group_size to create the groups.
 */
void create_groups_simple(vector<int>& possible_infected, int group_size, vector<vector<int>>& groups) {
    for (size_t i = 0; i < possible_infected.size(); i += group_size) {
        // Create a group and add it to the groups list
        vector<int> group(possible_infected.begin() + i, 
                          possible_infected.begin() + min(possible_infected.size(), i + group_size));
        groups.push_back(group);
    }
}

/**
 * Solves the problem using an optimal group testing strategy, splitting groups recursively.
 * 
 * Arguments:
 *   - data: A reference to the Data object containing the test case information.
 *   - infected: A reference to a vector of integers where the identified infected people will be stored.
 * 
 * Time Complexity: O(n log n), where n is the number of possible infected people.
 * In the worst case, each group splits in half, so the number of tests grows logarithmically.
 * However, each test involves linear work (testing a group).
 */
void group_testing_solver_optimal(const Data& data, vector<int>& infected) {
    vector<int> possible_infected(data.people_count);
    iota(possible_infected.begin(), possible_infected.end(), 0);  // Create a list of all people

    while (!possible_infected.empty()) {
        // Test the group and split if necessary
        if (test_group(possible_infected)) {
            if (possible_infected.size() == 1) {
                infected.push_back(possible_infected[0]);
                possible_infected.clear();
            } else {
                // Split the group into two and test both
                size_t mid = possible_infected.size() / 2;
                vector<int> left(possible_infected.begin(), possible_infected.begin() + mid);
                vector<int> right(possible_infected.begin() + mid, possible_infected.end());

                // Recurse on the smaller group that tests positive
                if (test_group(left)) {
                    possible_infected = left;
                } else {
                    possible_infected = right;
                }
            }
        } else {
            break;  // No infected people in the current group
               }
}

/**
 * Solves the problem using fixed-size group testing.
 * 
 * Arguments:
 *   - data: A reference to the Data object containing the test case information.
 *   - infected: A reference to a vector of integers where the identified infected people will be stored.
 *   - group_size: The fixed size of each group to be tested.
 * 
 * Time Complexity: O(n * g), where n is the number of possible infected people and g is the group size.
 * We loop over all possible infected people to create groups and then test each group. Each group test takes O(g) time.
 */
void group_testing_solver_fixed(const Data& data, vector<int>& infected, int group_size) {
    vector<int> possible_infected(data.people_count);
    iota(possible_infected.begin(), possible_infected.end(), 0);  // List of all people

    // Create fixed-size groups for testing
    vector<vector<int>> groups;
    create_groups_simple(possible_infected, group_size, groups);

    // Test each group
    for (const auto& group : groups) {
        if (test_group(group)) {
            // If a group tests positive, check each person individually
            for (int person : group) {
                if (test_person(person)) {
                    infected.push_back(person);
                }
            }
        }
    }
}

/**
 * Submits the list of infected people and checks if the answer is correct.
 * 
 * Arguments:
 *   - infected: A vector of integers representing the IDs of infected people.
 * 
 * Returns:
 *   - true if the answer is correct (the infected list is validated), false otherwise.
 * 
 * Time Complexity: O(k), where k is the number of infected people.
 * We need to print the infected people and compare the response, which depends on the number of infected people.
 */
bool submit_answer(const vector<int>& infected) {
    cout << "answer";
    for (int person : infected) {
        cout << " " << person;
    }
    cout << endl;

    string response;
    getline(cin, response);
    return response == "correct";
}

/**
 * Logs the details of failed attempts for debugging and analysis.
 * 
 * Arguments:
 *   - failed_attempts: A reference to a vector of pairs, where each pair contains the Data object for a test case
 *     and the list of infected people submitted during the test.
 * 
 * Time Complexity: O(f * k), where f is the number of failed test cases and k is the number of infected people in each failed case.
 * We loop through each failed test case and print the infected list, which involves iterating over the infected list.
 */
void log_failed_attempts(const vector<pair<Data, vector<int>>>& failed_attempts) {
    for (const auto& [data, infected] : failed_attempts) {
        cerr << "Test case failed:\n";
        print_data(data);  // Print the failed test case data
        cerr << "Submitted infected list:";
        for (int person : infected) {
            cerr << " " << person;
        }
        cerr << endl;
    }
}

/**
 * Main function to handle multiple test cases and process each.
 * 
 * Time Complexity: O(t * (n log n + n * g)), where t is the number of test cases, n is the number of people in the test case, and g is the group size.
 * Each test case involves either the optimal or fixed group testing strategy, which has complexities O(n log n) and O(n * g), respectively.
 * In the worst case, the total number of tests grows linearly with the number of test cases.
 */
int main() {
    int test_cases;
    cin >> test_cases;  // Read the number of test cases

    vector<pair<Data, vector<int>>> failed_attempts;  // List to store failed test cases

    while (test_cases--) {
        Data data;
        receive_data(data);  // Read input data for the test case

        vector<int> infected;
        if (data.people_count <= 10) {
            // Use the optimal solver for small cases
            group_testing_solver_optimal(data, infected);
        } else {
            // Use fixed-size group testing for larger cases
            int group_size = 3;  // Example fixed group size, could be adjusted
            group_testing_solver_fixed(data, infected, group_size);
        }

        // Submit the answer for validation
        if (!submit_answer(infected)) {
            failed_attempts.push_back({data, infected});
        }
    }

    // Log any failed attempts for debugging
    if (!failed_attempts.empty()) {
        log_failed_attempts(failed_attempts);
    }

    return 0;
}
