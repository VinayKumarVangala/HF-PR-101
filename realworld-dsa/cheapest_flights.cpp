#include <iostream>
#include <vector>
#include <queue>
#include <tuple>
#include <limits>
#include <map>
#include <string>

// A struct to represent an edge in the graph (a flight)
struct Edge {
    int to;    // The destination city (node) ID
    int cost;  // The cost of the flight (edge weight)
};

// Custom comparison for the priority queue to create a min-heap
// It prioritizes the flight with the lowest cost
using FlightTuple = std::tuple<int, int, int>; // {cost, city, stops}
struct CompareFlight {
    bool operator()(const FlightTuple& a, const FlightTuple& b) {
        return std::get<0>(a) > std::get<0>(b);
    }
};

/**
 * @brief Finds the cheapest price of a route with limited stops.
 * 
 * @param n The number of cities.
 * @param flights A list of flights represented as {from, to, cost}.
 * @param src The starting city.
 * @param dst The destination city.
 * @param k The maximum number of stops allowed.
 * @return int The cheapest cost to reach the destination, or -1 if not possible.
 */
int findCheapestPrice(int n, const std::vector<std::vector<int>>& flights, int src, int dst, int k) {
    // 1. Build the graph using an adjacency list.
    // The keys are the source cities, and the values are the destination cities and costs.
    std::vector<std::vector<Edge>> adj(n);
    for (const auto& flight : flights) {
        adj[flight[0]].push_back({flight[1], flight[2]});
    }

    // 2. Initialize data structures for the search.
    // A min-priority queue stores tuples of {cost, city, stops}.
    // This allows us to always process the path with the cheapest cost first.
    std::priority_queue<FlightTuple, std::vector<FlightTuple>, CompareFlight> pq;
    pq.push({0, src, 0}); // Push starting city with cost 0 and 0 stops.

    // A distance array stores the minimum cost to reach a city with `stops` number of stops.
    // Using a 2D vector for dp-like state: dist[city][stops]
    std::vector<std::vector<int>> dist(n, std::vector<int>(k + 2, std::numeric_limits<int>::max()));
    dist[src][0] = 0;

    // 3. Begin the modified BFS/Dijkstra search.
    while (!pq.empty()) {
        auto [current_cost, current_city, current_stops] = pq.top();
        pq.pop();

        // If we have reached the destination, return the cost.
        if (current_city == dst) {
            return current_cost;
        }

        // If we have exceeded the maximum number of stops, this path is invalid.
        if (current_stops > k) {
            continue;
        }

        // 4. Explore neighbors (flights from the current city).
        for (const auto& edge : adj[current_city]) {
            int next_city = edge.to;
            int flight_cost = edge.cost;
            int new_cost = current_cost + flight_cost;

            // Relaxation step: If a cheaper path is found to the neighbor with one more stop.
            if (new_cost < dist[next_city][current_stops + 1]) {
                dist[next_city][current_stops + 1] = new_cost;
                pq.push({new_cost, next_city, current_stops + 1});
            }
        }
    }

    // If the loop completes without reaching the destination, no valid path exists.
    return -1;
}

int main() {
    // Example: A map to convert city names to integer IDs for easier graph representation.
    std::map<std::string, int> city_to_id;
    std::vector<std::string> id_to_city;
    auto get_id = [&](const std::string& city) {
        if (city_to_id.find(city) == city_to_id.end()) {
            city_to_id[city] = id_to_city.size();
            id_to_city.push_back(city);
        }
        return city_to_id[city];
    };
    
    // Example flight data: {source, destination, cost}
    std::vector<std::vector<int>> flights_list = {
        {get_id("New York"), get_id("London"), 500},
        {get_id("New York"), get_id("Paris"), 600},
        {get_id("London"), get_id("Paris"), 150},
        {get_id("London"), get_id("Berlin"), 200},
        {get_id("Paris"), get_id("Berlin"), 180},
        {get_id("Paris"), get_id("Rome"), 250},
        {get_id("Berlin"), get_id("Rome"), 300},
        {get_id("Rome"), get_id("New York"), 700}
    };

    int num_cities = id_to_city.size();
    int start_node = get_id("New York");
    int end_node = get_id("Rome");
    int max_stops = 1;

    int cheapest_price = findCheapestPrice(num_cities, flights_list, start_node, end_node, max_stops);

    if (cheapest_price == -1) {
        std::cout << "No route found from New York to Rome with at most " << max_stops << " stop(s)." << std::endl;
    } else {
        std::cout << "Cheapest price from New York to Rome with at most " << max_stops << " stop(s): $" << cheapest_price << std::endl;
    }

    // Example with more stops
    max_stops = 2;
    cheapest_price = findCheapestPrice(num_cities, flights_list, start_node, end_node, max_stops);
    if (cheapest_price != -1) {
        std::cout << "Cheapest price from New York to Rome with at most " << max_stops << " stop(s): $" << cheapest_price << std::endl;
    }

    return 0;
}
