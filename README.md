# Routing_Protocol_Implementation

This project is an implementation of the link state routing protocol. 

This implementation simulates what happens at each node after it has learned of the initial topology or a change has occured. The link state algorithm is run, and the routes are recomputed after each change. 

For ties in determining which paths are better, this implementation chooses the path whose last node before the destination has a smalled ID. 


## What is Link State Algorithm?

In the link state algorithm, nodes "gossip" with each other about the state of the topology of all the routers. Whenever a node detects that the topology has changed, it runs the shortest path algorithm (Dijkstra's Algorithm) to find the new routes. Then the new routes are sent to all other nodes until every node has a complete up-to-date idea of the topology after every change.  


## How To Run Link State Algorithm

1. Make sure you have setup the topology file for the simulation
2. 
3. 

