/**
*	@file
*
*		@author sushil kumar meena (sushilm.iitb@gmail.com)
		@author mudit malpani (cooldudemd.malpani@gmail.com)
		@author palak dalal (palak.dalal@gmail.com)
		@author hemant noval (novalhemant@gmail.com)
*
*	Description: This file contains routines that can be used to find the shortest distance path from the source node to the destination node on the road map, using the famous Dijkstra's algorithm. Input files are to be placed in the same folder. It takes terminal input for destination. Can be integrated with graphical interface for taking destination input.
*
*	SPECIFICATIONS:
*
*	1. All the directions b/w nodes(e,w,n,s) will be based on a reference.
*
*	2. Overhead camera must be fixed w.r.t. our map
*
*	3. The bot will always maintain a state containing current node & direction(w.r.t. same reference)
*
*	4. The bot's turning direction will depend on its state direction.
*
*
 *******************************/



#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int NUM_NODES;/**< number of road junctions */
int pathLength = 0;/**< number of nodes in the path calculated */
#define INFINITE 1000000
#define roadThresh 80/**<maximum difference in the x/y coordinate of te nodes on same road */

/**
	defines te directions to turn
*/
typedef enum {
    F = 0, L = 1, B = 2, R = 3
} turn; // turnActual = [TurnOrig + (4-direc)]%4;

/**
	the state of bot i.e. in which direction to turn, lane junctions to be skipped before ant after the turn.
*/
typedef struct state {
    turn direc;// direction to turn
    int nodeNum;//node to turn
    int turningLane;// number of lane junction to be skipped before turning
    int skipLane;// number of lane junction to be skipped after turning
} botState;

/**
	structure defining length of a road and number of lanes on it
*/
typedef struct junction {
    int dist;
    int num_lanes;
} node;

/**
	calculates the node nearest to the source of all the nodes in 'dist'
*/
int getNearest(int* dist, int *selected) {
    int i = 0;
    int minDist = INFINITE;
    int minNode;
    for (i = 0; i < NUM_NODES; i++) {
        if (selected[i] != 1) {
            //printf("nearNode: %d %d %d %d\n", i, dist[i], minDist, minNode);
            if (dist[i] < minDist) {
                minDist = dist[i];
                minNode = i;
            }
        }
    }
    return minNode;
}

/**
	calculates shortest path between 'source' and 'dest' using dijkstra and returns te ordered array of the nodes in the path
*/
int* path(node **graph, int source, int dest) {
    int *dist = malloc(NUM_NODES * sizeof (int));// distance of nodes from source
    int *prev = malloc(NUM_NODES * sizeof (int));// parent node
    int *selected = malloc(NUM_NODES * sizeof (int));// nodes to which shortest path has been found
    int i = 0;
    for (i = 0; i < NUM_NODES; i++) {
        dist[i] = INFINITE;
        prev[i] = -1;
        selected[i] = 0;
    }
    dist[source] = 0;
    //selected[source]=1;
    while (selected[dest] != 1) {
        int curr = getNearest(dist, selected);// get nearest node
        //printf("curr:%d\n", curr);
        if (dist[curr] == INFINITE) return NULL;// no path to destination
        selected[curr] = 1;// mark the node as selected
        for (i = 0; i < NUM_NODES; i++) {
            //printf("",i,selected);
            if (selected[i] != 1 && graph[curr][i].dist != -1) {
                int newDist = dist[curr] + graph[curr][i].dist;// calculate shortest distance from source
                //printf("%d,%d,%d ",i,newDist,dist[i]);
                if (newDist < dist[i]) {
                    //printf("updated");
                    dist[i] = newDist;
                    prev[i] = curr;// update parent
                }
                printf("\n");
            }
        }
    }
    int *reversePath = malloc(NUM_NODES * sizeof (int));//path from destination to source
    int parent = dest;
    while (parent != source) {
        parent = prev[parent];
        printf("%d,", parent);
        reversePath[pathLength] = parent;
        pathLength++;
    }
    printf("length: %d\n", pathLength);
    int *actualPath = malloc((NUM_NODES + 1) * sizeof (int));//path from source to destination
    for (i = 0; i < pathLength; i++) {
        actualPath[i] = reversePath[pathLength - 1 - i];
        printf("->%d", actualPath[i]);
    }
    actualPath[i] = dest;
    printf("->%d\n", dest);
    return actualPath;
}

/**
	returns a node adjacent to 'node1' except 'node2'
*/
getAnotherAdjNode(int node1,int node2, node **graph){
	int i;
	for(i = 0; i < NUM_NODES; i++){
		if(graph[node1][i].dist!=-1 && i!=node2){
			return i;
		}
	}
}

/**Main File*/
int main() {
    FILE *nodeFile;//file containing number of nodes and their x & y coordinates
    nodeFile = fopen("nodeAct", "r");
    FILE *edgeFile;//< file containing number of lanes on each road in te form of an adjavcency matrix
    edgeFile = fopen("edgesAct", "r");

    fscanf(nodeFile, "%d", &NUM_NODES);

    node **graph = malloc(NUM_NODES * sizeof (node*));//< adjacecny matrix with each edge containg number of lanes

    int* xCords = malloc(NUM_NODES * sizeof (int));//< x coordinates of the nodes
    int* yCords = malloc(NUM_NODES * sizeof (int));//< y coordinates of the nodes

    int i;
    for (i = 0; i < NUM_NODES; i++) {
        graph[i] = malloc(NUM_NODES * sizeof (node));
        //graph[i][i].dist = -1;
    }

    for (i = 0; i < NUM_NODES; i++) {
        fscanf(nodeFile, "%d", &xCords[i]);
        fscanf(nodeFile, "%d", &yCords[i]);
    }

	// writing number of lanes and length of roads to the graph
    int binary;
    int j, x1, x2, y1, y2, distance;
    for (i = 0; i < NUM_NODES; i++) {
        for (j = 0; j < NUM_NODES; j++) {
            fscanf(edgeFile, "%d", &binary);
            if (binary == 0) graph[i][j].dist = -1;
            else {
                x1 = xCords[i];
                y1 = yCords[i];
                x2 = xCords[j];
                y2 = yCords[j];
                distance = ((y2 - y1)*(y2 - y1) + (x2 - x1)*(x2 - x1));
                graph[i][j].dist = distance;
                graph[i][j].num_lanes = binary;
            }
	    printf("%d,%d : %d\n",i,j,binary);
        }
    }

    int bot_fx,bot_fy,bot_bx,bot_by;

	FILE *botPos = fopen("botPos","r");

    fscanf(botPos,"%d",&bot_fx);
    fscanf(botPos,"%d",&bot_fy);
    fscanf(botPos,"%d",&bot_bx);
    fscanf(botPos,"%d",&bot_by);

    int source;

    FILE *startFile;// source node
    startFile = fopen("start", "r");

    fscanf(startFile, "%d", &source);

    int dest;// destination node from user input
	printf("Please specify the destination node : ");
	scanf("%d",&dest);
    int* actualPath = path(graph, source, dest);// path from source to destination


    i = 1;
    int prevX = bot_fx;
    int prevY = bot_fy;
    int currX, currY, nextX, nextY, prevDelX, prevDelY, delX, delY;
    float slope, prevSlope;
    botState *path = malloc(pathLength*sizeof(botState));
    currX = xCords[source];
    currY = yCords[source];
    path[0].direc = F;
    path[0].nodeNum = source;
    path[0].turningLane = 0;
    while (1) {
        nextX = xCords[actualPath[i]];
        nextY = yCords[actualPath[i]];
        delX = abs(nextX - currX);
        delY = abs(nextY - currY);
        prevDelX = abs(currX - prevX);
        prevDelY = abs(currY - prevY);
       
		//colculate bot state after each turn
		int node1 = actualPath[i-1];
		int node2 = actualPath[i];
		if((prevDelX < roadThresh && delX < roadThresh) || (prevDelY < roadThresh && delY < roadThresh)) {
			path[i].direc = F;
			int nodeExtra = getAnotherAdjNode(node1,node2,graph);
			path[i].turningLane = graph[node1][nodeExtra].num_lanes;
		}
		else if(prevDelX < roadThresh && delY < roadThresh){
			if(prevY < currY){
				if(currX > nextX) {
					path[i].direc = L;
					path[i].turningLane = 1;
				}
				else {
					path[i].direc = R;
					path[i].turningLane = graph[node1][node2].num_lanes;
				}
			}
			else{
				if(currY < nextY) {
					path[i].direc = L;
					path[i].turningLane = 1;
				}
				else{
					path[i].direc = R;
					path[i].turningLane = graph[node1][node2].num_lanes;
				}
			}
		}
		else if(prevDelY < roadThresh && delX < roadThresh){
			if(prevX < currX){
				if(currY < nextY) {
					path[i].direc = L;
					path[i].turningLane = 1;
				}
				else {
					path[i].direc = R;
					path[i].turningLane = graph[node1][node2].num_lanes;
				}
			}
			else{
				if(currY > nextY){
					path[i].direc = L;
					path[i].turningLane = 1;
				}
				else {
					path[i].direc = R;
					path[i].turningLane = graph[node1][node2].num_lanes;
				}
			}
		}

        path[i].nodeNum = actualPath[i];
        if (actualPath[i] == dest) break;
        prevX = currX;
        prevY = currY;
        currX = nextX;
        currY = nextY;
        i++;
    }
	
    fclose(startFile);
    FILE *starFile = fopen("pathD", "w");

	int as=0;
    printf("Path\n");
    for(i=0;i<=pathLength;i++){// writing path to file
        printf("%d(%d,%d)->\n",path[i].nodeNum,path[i].direc,path[i].turningLane);
	fprintf(starFile,"%d\n",path[i].direc);
	fprintf(starFile,"%d\n",path[i].turningLane);
	fprintf(starFile,"%d\n",as);
    }
    printf("\n");
	fclose(starFile);
}


