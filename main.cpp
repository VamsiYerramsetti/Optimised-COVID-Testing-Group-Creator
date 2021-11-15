#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <numeric>
#include <cmath>
#include <map>

using namespace std;

typedef tuple<int,int> connection;

struct Data {
	int people_count;
	int connection_count;
	int initial_infected;
	float spread_chance;
	int lower, upper;
	vector<connection> connections;
};

void receive_data(Data& data);
void print_data(Data& data);
bool test_group(vector<int>& group);
bool test_person(int person);
void remove_item(vector<int>& v, int item);
bool item_exists(vector<int>& v, int item);
int find_most_connected(vector<int>& group, vector<int>& available, map<int, map<int, double>>& scores);
tuple<int, int> pick_best_pair(map<int, map<int, double>>& scores, vector<int>& available);
void create_scores(vector<tuple<int, int>>& connections, int levels, double spread_rate, map<int, map<int, double>> scores[]);
void create_groups(map<int, map<int, double>> scores[], int group_size, int levels, double spread_rate, vector<int> possible_infected, vector<vector<int>>& groups);
int optimal_group_size(Data& data, int population);
void create_groups(vector<int> possible_infected, int group_size, vector<vector<int>>& groups);
void group_testing_solver_with_optimal_group_size(Data& data, vector<int>& infected);
void group_testing_solver(Data& data, vector<int>& infected, int group_size);
bool submit_answer(vector<int>& infected);
void print_failed_attempts(vector<tuple<Data, vector<int>>>& failed_attempts);

int round_tests_needed;
int total_tests_needed;

#define LEVELS 3


/**
 *
 * Complexity = O(n)
 *
 * Receive data for a run from the server
 * @param data The data that was received
 */
void receive_data(Data& data)
{
	cin >> data.people_count;
	cin >> data.connection_count;
	cin >> data.initial_infected;
	cin >> data.spread_chance;
	cin >> data.lower >> data.upper;

	//Add all connections as tuples
	int a, b;
	for(int i = 0; i < data.connection_count; i++) // O(n)
	{
		cin >> a >> b;
		data.connections.emplace_back(a,b);
	}
}

/**
 *
 * Complexity = O(1)
 *
 * Print the data that was received
 * @param data The data that was received at the start of this run
 */
void print_data(Data& data)
{
	cerr << "----- data for current run" << endl;
	cerr << "People\t\t\t\t" << data.people_count << endl;
	cerr << "Connections\t\t\t" << data.connection_count << endl;
	cerr << "Init. infected\t\t" << data.initial_infected << endl;
	cerr << "Spread chance\t\t" << data.spread_chance << endl;
	cerr << "Infected bounds\t\t" << data.lower << " - " << data.upper << endl;
	cerr << endl;
}

/**
 *
 * Complexity = O(n)
 *
 * Test a group for corona
 * @param group The group to test for. If one or more person in this group is tested Covid-positive, the entire
 * test will be positive
 * @return True if there is one or more Covid-positive person in this group
 */
bool test_group(vector<int>& group)
{
	// If we test an empty group we can be sure we will receive no positive tests.
	if (group.empty())
		return false;

	cout << "test";
	for (int p : group) // O(n)
	{
		cout << " " << p;
	}
	cout << endl;

	string in;
	do
	{
		getline(cin, in);
	} while(in.empty());

	round_tests_needed++;
	total_tests_needed++;
	return in == "true";
}

/**
 *
 * Complexity = O(1)
 *
 * Test a single person
 * @param person The person to test
 * @return True if this person has been tested Covid-positive, false otherwise
 */
bool test_person(int person)
{
	cout << "test " << person << endl;
	string in;
	do
	{
		getline(cin, in);
	} while (in.empty());

	round_tests_needed++;
	total_tests_needed++;
	return in == "true";
}

/**
 *
 * Complexity = O(n)
 *
 * Remove an item in a list
 * @param v The list to remove the item from
 * @param item The item to remove
 */
void remove_item(vector<int>& v, int item)
{
	// Search through all items
	for (int i = 0; i < v.size(); i++) // O(n)
	{
		if (v[i] == item)
		{
			v.erase(v.begin()+i);
			return;
		}
	}
}

/**
 *
 * Complexity = O(n)
 *
 * Check if an item exists
 * @param v The list to check
 * @param item The item to check
 * @return True if item exists in v
 */
bool item_exists(vector<int>& v, int item)
{
	for (int& i : v) // O(n)
	{
		if (i == item)
			return true;
	}

	return false;
}

/**
 *
 * Complexity = O(n)
 *
 * Find the most connected available person to the group based on the scores
 * @param group The group to check for
 * @param available The people available
 * @param scores The scores to check from
 * @return The most connected person for this group
 */
int find_most_connected(vector<int>& group, vector<int>& available, map<int, map<int, double>>& scores)
{
	if (available.empty())
		throw invalid_argument("available must not be empty");

	int best = available[0];
	double best_score = -1.0;

	for (int p : available) // O(n)
	{
		double curr = 0.0;

		for (int m : group) // O(g)
		{
			curr += scores[m][p];
		}

		if (curr > best_score)
		{
			best = p;
			best_score = curr;
		}
	}

	return best;
}

/**
 *
 * Complexity = O(n^2)
 *
 * Pick the best pair available in the scores set
 * @param scores The scores to check from
 * @param available The people we can choose from
 * @return The best pair that is available based on the scores
 */
tuple<int, int> pick_best_pair(map<int, map<int, double>>& scores, vector<int>& available)
{
	if (available.size() < 2)
		throw invalid_argument("size of available persons must be at least 2");

	tuple<int, int> best = {available[0], available[1]};
	double best_score = -1.0;

	for (auto& a : scores) // O(n)
	{
		if (!item_exists(available, a.first))
			continue;

		for (auto& b : a.second) // O(n)
		{
			if (item_exists(available, b.first) && a.first != b.first)
			{
				const double score = b.second;
				if (score > best_score)
				{
					best = {a.first, b.first};
					best_score = score;
				}
			}
		}
	}

	return best;
}

/**
 *
 * Complexity = O(n^3)
 *
 * Create scores to use in the [create_groups] functions. When you want to use the [create_groups] function multiple
 * times, it is wise to cache the result of this function, since it does not depend on the possible infected people.
 * @param connections The connections that were given at the start of the run
 * @param levels The amount of times to calculate scores based on the previous levels, must be at least 1.
 * @param spread_rate The spread rate
 * @param scores The scores that will be made
 */
void create_scores(vector<tuple<int, int>>& connections, int levels, double spread_rate, map<int, map<int, double>> scores[])
{
	if (levels < 1)
	{
		throw invalid_argument("Levels must be at least 1");
	}

	for (auto conn : connections) // O(c)
	{
		int a, b;
		tie(a, b) = conn;

		scores[0][a][b] += spread_rate;
		scores[0][b][a] += spread_rate;
	}

	// Now loop through all scores and add them again
	for (int level = 1; level < levels; level++) // O(l)
	{
		for (auto& conn_a : scores[level-1]) // O(n)
		{
			const int a = conn_a.first;

			for (auto& conn_b : conn_a.second) // O(n)
			{
				const int b = conn_b.first;

				const double weight = conn_b.second;
				scores[level][a][b] = weight;

				// Now go look up scores and add those
				auto& b_conns = scores[level-1][b];

				for (auto& b_conn : b_conns) // O(n)
				{
					scores[level][a][b_conn.first] += weight * b_conn.second * pow(spread_rate, level);
				}
			}
		}
	}
}

/**
 *
 * Complexity = O(n^3)
 *
 * Create groups based on scores
 * @param scores The scores that were created with [create_scores]
 * @param group_size The size of groups
 * @param levels The amount of levels that were used, use the same as in [create_scores]
 * @param spread_rate The spread rate of the virus
 * @param possible_infected The possibly infected persons to choose from
 * @param groups The groups that will be made
 */
void create_groups(map<int, map<int, double>> scores[], int group_size, int levels, double spread_rate, vector<int> possible_infected, vector<vector<int>>& groups)
{
	// Let's make the groups!
	while (!possible_infected.empty()) // O(n/groupsize)
	{
		if (possible_infected.size() < group_size)
		{
			// We are at the last group
			groups.emplace_back(possible_infected);
			break;
		}

		vector<int> group;

		// Pick the pair with the highest score:
		auto best_pair = pick_best_pair(scores[levels-1], possible_infected); // O(n^2)
		int a, b;
		tie(a, b) = best_pair;
		group.emplace_back(a);
		group.emplace_back(b);
		// Remove those items
		remove_item(possible_infected, a); // O(n)
		remove_item(possible_infected, b); // O(n)

		for (int i = 2; i < group_size; i++) // O(groupsize)
		{
			int next_person = find_most_connected(group, possible_infected, scores[levels-1]); // O(groupsize*n)
			group.emplace_back(next_person);
			remove_item(possible_infected, next_person); // O(n)
		}

		groups.emplace_back(group);
	}
}

/**
 *
 * Complexity = O(1)
 *
 * Find the optimal group size
 * @param data The data that we received
 * @param population The population size to calculate the group size for
 * @return The optimal group size
 */
int optimal_group_size(Data& data, int population)
{
	double average_infected = ((double) data.upper + (double) data.lower) / 2;
	double result = 0.693 / ((average_infected / (double) population));
	return (int) round(result);
}

/**
 *
 * Complexity: O(n)
 *
 * Create groups of group_size
 * @param possible_infected The possibly infected people
 * @param group_size The size of the groups
 * @param groups The groups that will be made
 */
void create_groups(vector<int> possible_infected, int group_size, vector<vector<int>>& groups)
{
	while (!possible_infected.empty()) // O(n/groupsize)
	{
		if (possible_infected.size() <= group_size)
		{
			groups.emplace_back(possible_infected);
			break;
		}

		vector<int> group;

		for (int i = 0; i < group_size; i++) // O(groupsize)
		{
			group.emplace_back(possible_infected.back());
			possible_infected.pop_back();
		}

		groups.emplace_back(group);
	}
}

/**
 *
 * Complexity = O(n^3)
 *
 * A group testing solver that takes the optimal group size into account
 * @param data The data received at the start of this run
 * @param infected The infected people will be put in here
 */
void group_testing_solver_with_optimal_group_size(Data& data, vector<int>& infected)
{
	// Use a deque so we can pop from the front and back.
	vector<int> possible_infected(data.people_count);
	// Fill vector with 0...size()
	iota(possible_infected.begin(), possible_infected.end(), 0);

	// Calculate the scores once so we don't have to redo when making new (smaller) groups
	int levels = LEVELS;

	map<int, map<int, double>> scores[levels];

	// We cannot calculate the scores for more than 1100 people as we do not have enough memory
	if (data.people_count <= 1100)
		create_scores(data.connections, levels, data.spread_chance, scores); // O(n^3)

	while (!possible_infected.empty()) // O(log n)
	{
		int group_size = optimal_group_size(data, possible_infected.size());

		cerr << "[-] new optimal group size: " << group_size << endl;
		cerr << "[-] possible infected group size: " << possible_infected.size() << endl;

		vector<vector<int>> groups;

		if (data.people_count <= 1100 && group_size >= 2)
			create_groups(scores, group_size, 3, data.spread_chance, possible_infected, groups); // O(n^2)
		else
			create_groups(possible_infected, group_size, groups); // O(n)

		for (vector<int>& group : groups) // O(n/size)
		{
			if(group_size > 2)
			{
				if (!test_group(group))
				{
					for (int person : group) // O(size)
					{
						remove_item(possible_infected, person); // O(n)
					}
				}
			}
			else
			{
				for(int p : group) // O(1), since the group size is either 1 or 2
				{
					if(test_person(p))
						infected.emplace_back(p);
					remove_item(possible_infected, p); // O(n)

					if (infected.size() == data.upper)
					{
						cerr << "[x] We have found all infected people. Skipping tests" << endl;
						return;
					}

				}
			}
		}
	}
}

/**
 *
 * Complexity = O(n^3)
 *
 * A group testing solver that uses a fixed group size
 * @param data The data received at the start of the program
 * @param infected The infected people will be put in here
 * @param group_size The size of the groups
 */
void group_testing_solver(Data& data, vector<int>& infected, int group_size)
{
	cerr << "Group size\t\t\t" << group_size << endl;

	vector<int> possible_infected(data.people_count);
	// Fill vector with 0...size()
	iota(possible_infected.begin(), possible_infected.end(), 0);

	const int levels = 5;

	vector<vector<int>> groups;

	// Only create scores if we have do not have more than 1100 connections, because else we will run out of storage
	if (data.people_count <= 1100)
	{
		// Calculate the scores once so we don't have to redo when making new (smaller) groups
		map<int, map<int, double>> scores[levels];
		create_scores(data.connections, levels, data.spread_chance, scores); // O(n^3)
		create_groups(scores, group_size, 3, data.spread_chance, possible_infected, groups); // O(n^3)
	}
	else
	{
		create_groups(possible_infected, group_size, groups); // O(n)
	}

	for (vector<int>& group : groups) // O(n/groupsize)
	{
		if (test_group(group))
		{
			// Somebody in this group has been tested positive
			for (int p : group) // O(groupsize)
			{
				// Test everyone
				if (test_person(p))
				{
					infected.emplace_back(p);
				}

				if (infected.size() >= data.upper)
				{
					cerr << "[x] skipped tests since we are at upper bound" << endl;
					return;
				}
			}
		}
	}
}

/**
 *
 * Complexity = O(n)
 *
 * Submit which people are infected
 * @param infected The people that are infected in this run
 * @return True if the answer was successful, false otherwise
 */
bool submit_answer(vector<int>& infected)
{
	vector<int> submitted;

	cout << "answer ";
	for (int infected_person : infected) // O(n)
	{
		if (item_exists(submitted, infected_person))
			continue;

		cout << infected_person << " ";
		submitted.emplace_back(infected_person);
	}
	cout << endl;

	string result;
	cin >> result;

	bool success = result == "success";
	if (success)
		cerr << "[+] " <<  result << endl;
	else
		cerr << "[-] " << result << endl;

	return success;
}

/**
 *
 * Complexity = O(1)
 *
 * Print the failed attempts
 * @param failed_attempts The failed attempts
 */
void print_failed_attempts(vector<tuple<Data, vector<int>>>& failed_attempts)
{
	cerr << "----- There were some failed attempts!" << endl;

	for (tuple<Data, vector<int>>& t : failed_attempts) // O(0), since we never have any failed challenges according to our correctness analysis
	{
		Data data;
		vector<int> infected;
		tie (data, infected) = t;

		cerr << "----- Input:" << endl;
		print_data(data);
		cerr << "----- Infected" << endl;
		for (int& infected_person : infected) // O(n)
			cerr << infected_person << " ";
		cerr << endl << endl;
	}
}

/**
 *
 * Complexity = O(n^3)
 *
 * @param argc -
 * @param argv =
 * @return 0 if we had no failed tests
 */
int main(int argc, char* argv[])
{
	// Receive the problem count from the server
	int problem_count;
	cin >> problem_count;

	// List for keeping failed attempts
	vector<tuple<Data, vector<int>>> failed_attempts;

	// Loop through all problems
	for (int p = 0; p < problem_count; p++)
	{
		// Let the user know we're on a new run
		cerr << "----- starting run " << p+1 << " out of " << problem_count << endl;

		//Reset round test count
		round_tests_needed = 0;

		// Receive data
		Data data;
		receive_data(data); 																		// O(n)
		print_data(data); 																		// O(n)

		// Solve problem
		vector<int> infected;

		int group_size = optimal_group_size(data, data.people_count);							// O(1)

		//group_testing_solver(data, infected, 4);
		if(group_size > 4) //Only using the group solving when we can save sufficiently
		{
			group_testing_solver_with_optimal_group_size(data, infected);						// O(n^3)
		}
		else
		{
			//Some algorithm which tested everybody and then contact traces.
			group_testing_solver(data, infected, 4);								// O(n^3)
			//So not this!
			//group_testing_solver_with_optimal_group_size(data, infected);
		}

		// Submit answer
		bool success = submit_answer(infected);													// O(n)
		if (!success)
			failed_attempts.emplace_back(data, infected);

		cerr << "----- Tests need this round:" << round_tests_needed << " ("
			 << round(round_tests_needed/(float)data.people_count*100) <<"%)" << endl;
		cerr << "▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒" << endl;
	}

	// Check if there were any failed attempts
	if (!failed_attempts.empty())
		print_failed_attempts(failed_attempts);													// O(1)

	cerr<< "----- Total needed tests:" << total_tests_needed << endl;

	// Return based on whether all answers were successful
	return !failed_attempts.empty();
}
