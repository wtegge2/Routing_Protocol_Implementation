#include <cstdio>

#include <iostream>
#include <list>
#include <fstream>
#include <iterator>
#include <string>
#include <queue>
#include <vector>
#include <limits>
#include <utility>
#include <algorithm>

using namespace std;

// link struct determining the cost between two nodes
struct link {

    int own_ID;
    int neighbor_ID;
    int cost;

    // determine if the right link has the same nodes
    bool operator==(const link& r) {
        return ((own_ID == r.own_ID) && (neighbor_ID == r.neighbor_ID));
    }
};

struct node {

    int ID;
    int num_neighbors;
    list<link> neighbors;
    list<string> messages;
    vector<int> shortest_paths_costs;
    vector<vector<int>> shortest_paths;
    vector<pair<int, int>> forwarding_table;
};

// global variables
int num_nodes = 0;
vector<link> links;



// dijkstra's algorithm
pair<vector<int>, vector<vector<int>>> dijkstra(int source) {

//    std::cout << "MADE IT TO DIJKSTRA " << std::endl;

    // 2D list for the graph
    vector<vector<pair<int, int>>> graph(num_nodes + 1);

    // push each link for own_ID
    for(const link temp : links){

        graph[temp.own_ID].push_back({temp.neighbor_ID, temp.cost});

        //graph[temp.neighbor_ID].push_back({temp.own_ID, temp.cost})

    }

    // distance vector for each node (node 0 is ignored in general)
    vector<int> distance(num_nodes + 1, numeric_limits<int>::max());
    distance[source] = 0;

    // init vector with all -1
    vector<int> predecessors(num_nodes + 1, -1);

    // vector to keep track of visited nodes
    vector<bool> visited(num_nodes + 1, false);

    // priority queue to keep track of closest pairs
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> priority_q;

    // start the algorithm by looking at the starting node, and finding all of its neighbors
    priority_q.push({0, source});


    /* 
        algorithm: while the queue is not empty, get next unvisited node
        for each node, check the cost to get to it. if it's less than current,
        update it
    */
    while(!priority_q.empty()){

        int i = priority_q.top().second;

        priority_q.pop();


        if(visited[i]){
            continue;
        }

        visited[i] = true;

        // for each path available now, update distances and push new distances
        for(const pair<int, int>& edge : graph[i]){

            int v = edge.first;
            int weight = edge.second;

            if(distance[i] + weight < distance[v]){

                distance[v] = distance[i] + weight;
                predecessors[v] = i;
                priority_q.push({distance[v], v});

            } else if (distance[i] + weight == distance[v]) {
                if ((predecessors[v] != -1) && (predecessors[v] > i)) {
                    distance[v] = distance[i] + weight;
                    predecessors[v] = i;
                    priority_q.push({distance[v], v});
                }

            }
        }
    }

    /*
        for each node, find the shortest path it'll take to get there
    */
    vector<vector<int>> shortest_paths(num_nodes + 1);

    for(int j = 1; j <= num_nodes; j++){

        // TODO: find a way to make sure that if the shortest path doesn't exist. reorder it so that
        // we don't push j until after sorting, that  

        int curr = j;

        // base case - if base node
        if (curr == source) {
            shortest_paths[j].push_back(curr);
            continue;

        // next case - if no path exists
        } else if (predecessors[curr] == -1) {
            shortest_paths[j].push_back(-1);
            shortest_paths[j].push_back(-1);
            continue;
        // else, explore shortest path
        } else {
            while(curr != -1){

            shortest_paths[j].push_back(curr);
            curr = predecessors[curr];

        }

        reverse(shortest_paths[j].begin(), shortest_paths[j].end());

        }
    }

    return {distance, shortest_paths};

}


/* 
    void function to create a forwarding table, given the node
    to make the table for 

    forwarding table in format of Node?, Next_Path, Cost

*/

void create_forwarding_table(node* node) {
    
    vector<int> costs = node->shortest_paths_costs;
    vector<vector<int>> paths = node->shortest_paths;
    vector<pair<int, int>> table(num_nodes + 1);

    /*
    for(int j = 1; j < num_nodes + 1; j++){
        cout << "Cost to node " << j << " from node " << node->ID << " is " << costs[j] << endl;
    }
    */
        

    for(int i = 1; i < num_nodes+1; i++){
        
        if(i == node->ID){
            table[i].first = i;
            table[i].second = 0;
            continue;
        }
        
        table[i].first = paths[i][1];
        table[i].second = costs[i];

    }

    node->forwarding_table = table;

    /*
    for(int k = 1; k < num_nodes+1; k++){
        cout << "Next hop is " << table[k].first << " with cost of " << table[k].second << endl;
    }
    */


}


/*

    function that parses a string into a vector of ints to determine links

*/

vector<int> stringToInts(const string s) {
    vector<int> splitString;
    
    bool is_neg = false;
    int num = 0;
    
    for (char c : s) {
        // base case, there's a space
            
        if ((c == ' ')) {
            if (is_neg) {
                num *= -1;
            }
            splitString.push_back(num);
            is_neg = false;
            num = 0;
        } else if (c == '-') {
            is_neg = -1;
        } else if ((c >= '0') && (c <= '9')) {
            num = num * 10 + (c - '0');
        } else {
            num = 0;
            splitString.push_back(num);
            break;
        }
    }
    
    if (is_neg) {
        num *= -1;
    }
    splitString.push_back(num);
    
    return splitString;
    
}

// void function that runs dijkstras on all nodes with whatever neighbors are at that time
// updates forwarding table accordingly
void runDijkstras(int max_nodes, node* nodes, int* ID_list) {

    int temp_ID;
    vector<int> temp_distance;
    pair<vector<int>, vector<vector<int>>> dijkstra_result;
    node* temp_node;
    vector<pair<int, int>> table(num_nodes+1);


    // find shortest paths for each node

    for(int i = 1; i < max_nodes + 1; i++) {

        temp_ID = nodes[i].ID;

        if(ID_list[temp_ID] == 0) {
            // std::cout << "this node doesnt exist" << std::endl;
            continue;
        }

        dijkstra_result = dijkstra(temp_ID);

        nodes[temp_ID].shortest_paths_costs = dijkstra_result.first;
        nodes[temp_ID].shortest_paths = dijkstra_result.second;

        temp_node = &nodes[i];

        create_forwarding_table(temp_node);

        // cout << "Forwarding table for node " << i << ": " << endl;
        table = nodes[i].forwarding_table;
        for(int j = 1; j < num_nodes+1; j++){
            // cout << "Next hop is " << table[j].first << " with cost of " << table[j].second << endl;
            // cout << j << " " << table[j].first << " " << table[j].second << endl;
        }

    }

}

void sendMessages(ofstream* fpOut, node* nodes, char* message) {

    // cout << "Reading and writing messages" << endl;

    string m;
    
    if (fpOut == NULL) {
        // cout << "Wrong file" << endl;
        return;
    }

    if (nodes == NULL) {
        // cout << "Wrong node format" << endl;
        return;
    }

    ifstream message_file(message);


    if (message_file.is_open()) {
        string line;
        while(getline(message_file, line)) {
            int counter = 0;
            vector<int> nums = {0, 0};
            string store_message = "";
            int cost = 0;
            string paths = "";


            for (char s: line) {

                if (counter < 2) {
                    if (s == ' ') {
                        counter++;
                    } else {

                        nums[counter] = nums[counter] * 10 + (s - '0');
                    }
                } else {
                    store_message += s;
                }

            }

            // cout << line << endl;

            // TODO : find the shortest path if it exists.

            cost = nodes[nums[0]].shortest_paths_costs[nums[1]];
            bool is_unreachable = false;

            for (int path : nodes[nums[0]].shortest_paths[nums[1]]) {
                if (path == -1) {
                    *fpOut << "from " << nums[0] << " to " << nums[1] << " cost infinite hops unreachable message " <<  store_message << endl;
                    is_unreachable = true;
                    break;
                } else {

                    if (path !=  nums[1]) {
                        paths = paths + to_string(path) + " ";
                    }
                }
            }            

            if (!is_unreachable) {
                *fpOut << "from " << nums[0] << " to " << nums[1] << " cost " << cost << " hops " << paths << "message " << store_message << endl;
            }
        }

    }

    message_file.close();

}


int main(int argc, char** argv) {


    //printf("Number of arguments: %d", argc);
    if (argc != 4) {
        printf("Usage: ./linkstate topofile messagefile changesfile\n");
        return -1;
    }

    // variables to store current link's information
    int node_ID = 0;
    int node_neighbor_ID = 0;
    int link_cost = 0;

    int max_nodes = 0;


    /* Part 1 - Find number of nodes */
    std::ifstream topology_file(argv[1]);


    if (topology_file.is_open()) {

        // std::cout << "File is open" << std::endl;

        std::string line;
        vector<int> changing_link;

        while(std::getline(topology_file, line)) {
            changing_link = stringToInts(line);

            // case shouldn't happen but make sure size is 2 nodes + 1 cost
            if (changing_link.size() != 3) {
                continue;
            }
            node_ID = changing_link[0];

            if(node_ID > max_nodes){
                max_nodes = node_ID;
            }

            node_ID = changing_link[1];

            if(node_ID > max_nodes){
                max_nodes = node_ID;
            }
        }
    }

    // std::cout << "number of nodes: " << max_nodes << std::endl;

    topology_file.close();

    /* Part 2 - Find neighbors and costs for each node */

    // set up node pointers and nodes

    node* nodes = new node[max_nodes + 1];

    int ID_list[max_nodes + 1] = {0};


    // second loop to start getting node links
    std::ifstream topology_file2(argv[1]);

    if(topology_file2.is_open()) {

        // cout << "Opened file a second time." << endl;

        string line;
        vector<int> changing_link;

        while(getline(topology_file2, line)) {
            // std::cout << line << std::endl;

            changing_link = stringToInts(line);

            // shouldn't happen but in case numbers aren't formatted properly
            if (changing_link.size() != 3) {
                continue;
            } 

            node_ID = changing_link[0];
            node_neighbor_ID = changing_link[1];
            link_cost = changing_link[2];

            // check if either node is new, init if so

            if (ID_list[node_ID] == 0) {
                ID_list[node_ID] = 1;
                nodes[node_ID].ID = node_ID;
                nodes[node_ID].num_neighbors = 0;

                num_nodes++;
            }

            if (ID_list[node_neighbor_ID] == 0) {
                ID_list[node_neighbor_ID] = 1;
                nodes[node_neighbor_ID].ID = node_neighbor_ID;
                nodes[node_neighbor_ID].num_neighbors = 0;

                num_nodes++;
            }

            // update neighbor counts and lists for both

            link new_link;
            new_link.own_ID = node_ID;
            new_link.neighbor_ID = node_neighbor_ID;
            new_link.cost = link_cost;

            links.push_back(new_link);

            nodes[node_ID].neighbors.push_back(new_link);
            nodes[node_ID].num_neighbors++;


            link new_link2;
            new_link2.own_ID = node_neighbor_ID;
            new_link2.neighbor_ID = node_ID;
            new_link2.cost = link_cost;

            links.push_back(new_link2);

            nodes[node_neighbor_ID].neighbors.push_back(new_link2);
            nodes[node_neighbor_ID].num_neighbors++;

        }
    }

    // sanity check

    // for (link l : links) {
    //     cout << "Node " << l.own_ID << " costs " << l.cost << " to Node " << l.neighbor_ID << endl;
    // }

    // for (int i = 1; i < max_nodes + 1; i++) {

    //     cout << "Node " << nodes[i].ID << " has " << nodes[i].num_neighbors << " neighbor(s): " << endl;

    //     // print out all of it's neighbors

    //     for (link l : nodes[i].neighbors) {
    //         cout << "- Node " << l.own_ID << " costs " << l.cost << " to Node " << l.neighbor_ID << endl;
    //     }

    // }

    // clean up
    topology_file2.close();

    /* Part 3 - Run Dijkstras and create forwarding tables before updates */

    node* node_ptr = &nodes[0];
    int* ID_list_ptr = &ID_list[0];

    runDijkstras(max_nodes, node_ptr, ID_list_ptr);

    /* Part 3.5 - Output forwarding tables before any changes */

    // FILE *fpOut;
    // fpOut = fopen("output.txt", "w");
    // fclose(fpOut);
    

    std::ofstream fpOut("output.txt");
    vector<pair<int, int>> table(num_nodes+1);

    for (int i = 1; i < max_nodes + 1; i++) {

        table = nodes[i].forwarding_table;
        for(int j = 1; j < num_nodes+1; j++){

            if (table[j].first != -1) {
                fpOut << j << " " << table[j].first << " " << table[j].second << endl;
            }

        }

        fpOut << endl;
    }

    sendMessages(&fpOut, nodes, argv[2]);


    fpOut << endl;

    /* Part 4 - Change forwarding table with each node change */

    // read the changesfile and update nodes.

    ifstream changesFile(argv[3]);

    if (changesFile.is_open()) {

        // cout << "Opening Change File" << endl;

        string line;
        vector<int> changing_link;
        vector<link>::iterator it;
        vector<link>::iterator reverse_it;
        link copy_link;
        link reverse_copy;
        
        while (getline(changesFile, line)) {
            // std::cout << line << std::endl;

            // cout << endl;

            changing_link = stringToInts(line);

            copy_link.own_ID = changing_link[0];
            copy_link.neighbor_ID = changing_link[1];
            copy_link.cost = changing_link[2];

            reverse_copy.own_ID = changing_link[1];
            reverse_copy.neighbor_ID = changing_link[0];
            reverse_copy.cost = changing_link[2];


            // cout << "New connection: ";
            // for (int val : changing_link) {
            //     cout << val << " ";
            // }
            // cout << endl;

            it = find(links.begin(), links.end(), copy_link);

            if (it == links.end()) {

                // i think we can assume a negative cost won't be added first?

                // cout << "This link doesn't exist yet" << endl;
                links.push_back(copy_link);
                links.push_back(reverse_copy);

            } else {

                // find reverse node. in theory, this should exist


                if (changing_link[2] == -999) {
                    links.erase(it);

                    reverse_it = find(links.begin(), links.end(), reverse_copy);
                    links.erase(reverse_it);
                } else {
                    it->cost = changing_link[2];

                    reverse_it = find(links.begin(), links.end(), reverse_copy);
                    reverse_it->cost = changing_link[2];
                }

            }

            runDijkstras(max_nodes, node_ptr, ID_list_ptr);

            for (int i = 1; i < max_nodes + 1; i++) {

                table = nodes[i].forwarding_table;
                for(int j = 1; j < num_nodes+1; j++){
                    if (table[j].first != -1) {
                        fpOut << j << " " << table[j].first << " " << table[j].second << endl;
                    }
                }
                fpOut << endl;
            }

            sendMessages(&fpOut, nodes, argv[2]);

            // TODO: if new node gets added

            // TODO: add new path between existing nodes

            // TODO: update/remove paths between existing nodes

        }

    }

    delete[] nodes;

    return 0;

}