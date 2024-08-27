# Routing_Protocol_Implementation

This project is an implementation of the link state routing protocol. 

This implementation simulates what happens at each node after it has learned of the initial topology or a change has occured. The link state algorithm is run, and the routes are recomputed after each change. 

For ties in determining which paths are better, this implementation chooses the path whose last node before the destination has a smaller ID. 


## What is Link State Algorithm?

In the link state algorithm, nodes "gossip" with each other about the state of the topology of all the routers. Whenever a node detects that the topology has changed, it runs the shortest path algorithm (Dijkstra's Algorithm) to find the new routes. Then the new routes are sent to all other nodes until every node has a complete up-to-date idea of the topology after every change.  


## How To Run Link State Algorithm

1. Setup the topology file for the simulation
2. Setup the changes file for the simulation
3. Setup the messages file for the simulation
4. Run the linkstate.cpp file:```./linkstate topofile messagefile changesfile```


## Topology File

The topology file is a representation of the initial topology in the simulation. Each line represents a link between two nodes. 

Ex: 1 4 8

In this example, 1 is a node, and 4 is the other node. 8 is the cost of the link between the two nodes. 


## Changes File

The changes file represents a sequence of changes that happen to the topology in the simulation. The changes happen one by one, and they happen in order from top to bottom. Each line has the same structure as the lines in the topology file. A cost of -999 means that the link is broken/non-existent. Actual costs are never 0 or negative. 

Ex:
```
1 4 10
1 4 -999
```

In this example, the link cost between nodes 1 and 4 changes to 10. Then the link between 1 and 4 is broken/removed. 


## Messages File

The messages file shows which nodes send data/messages to other nodes after the routing tables converge in the simulation. In the code, the routing tables converge before any of the topology changes and again after each change. All messages in this file are sent every time the tables converge. 

Ex: 1 4 here is a message from 1 to 4

This message file would cause "here is a message from 1 to 4" to be sent from 1->4



