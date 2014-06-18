/**
*	@file
*
*		@author sushil kumar meena (sushilm.iitb@gmail.com)
*		@author mudit malpani (cooldudemd.malpani@gmail.com)
*		@author palak dalal (palak.dalal@gmail.com)
*		@author hemant noval (novalhemant@gmail.com)
*
*	Description:
*		This code is to be used after each individual junctions are extracted by Matlab file map_detection.m. Input files are to be placed in the same directory. Following routines does the task of grouping the junctions of same node and then establishing the road network in terms of these junctions.
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/** Number of lane junctions */
int NUM_NODES; 
#define xTh 100 
#define yTh 100
#define thresh 40 /**< Maximum variation in te x/y coordinates of any two lane junctions on the same lane */

/**
    returns whether the coordinates(a,b) of the nodes lie on same lane
 */
int almost_same(int a, int b) {
    return (abs(a - b) < thresh);
}

/**
    x - array of x coordinates of the lane junctions
    y - array of y coordiantes of the lane junctions
    fx - x coordinate of the front side of the bot
    fy - y coordinate of the front side of the bot
    bx - x coordinate of the back side of the bot
    by - y coordinate of the back side of the bot
    returns the lane junction nearest to the bot in the direction of the bot
 */
int find_nearest_junction(int* x, int* y, int fx, int fy, int bx, int by) {
    //determines if the bot is aligned with vertical or horizontal lane
    //y_same = 1 => bot aligned with horizontal lane
    //y_same = 0 => bot aligned with vertical lane
    int y_same = -1;
    float y_comm, x_comm;
    int greater;
    int diff_coor;
    if (abs(fx - bx) < abs(fy - by)) {
        diff_coor = fy;
        y_same = 0; //x coordinates same => bot aligned with vertical lane
        x_comm = (fx + bx) / 2;
        if (fy > by) {
            greater = 1;
        } else {
            greater = 0;
        }
    } else {
        printf("after 2\n");
        diff_coor = fx;
        y_same = 1; // y coordinates same => bot aligned with horizontal lane
        y_comm = (fy + by) / 2;
        if (fx > bx) {
            greater = 1;

        } else {
            greater = 0;
        }

    }

    int i;
    int nearest_junct = -1;
    int min_val = 100000;
    for (i = 0; i < NUM_NODES; i++) {
        if (y_same) {
            if (almost_same(y_comm, y[i])) {
                if (greater && x[i] > fx && x[i] - fx < min_val) {
                    printf("%d is greater than %d\n", x[i], fx);
                    nearest_junct = i;
                    min_val = x[i] - fx;
                }
            }
            if (greater == 0 && x[i] < fx && fx - x[i] < min_val) {
                nearest_junct = i;
                min_val = fx - x[i];
            }
        } else {
            if (almost_same(x_comm, x[i])) {
                if (greater && y[i] > fy && y[i] - fy < min_val) {
                    printf("%d is greater than %d\n", y[i], fy);
                    nearest_junct = i;
                    min_val = y[i] - fy;
                }
            }
            if (greater == 0 && y[i] < fy && fy - y[i] < min_val) {
                nearest_junct = i;
                min_val = fy - y[i];
            }
        }
    }
    printf("nears %d\n", nearest_junct);
    return nearest_junct;

}

/**
	Combines lane junctions into road junctions and creates the files of x&y coordinates on road junctions and their adjacency matrix
*/
int main() {
    FILE *nodeFile; /**< File containing number of lane junctions and their x & y coordinates. */
    nodeFile = fopen("node", "r");
    FILE *edgeFile; /**< File containing adjacency matrix of graph with nodes as lane junctions and edges as lanes. */
    edgeFile = fopen("edge", "r");

    fscanf(nodeFile, "%d", &NUM_NODES);

    int* xCords = malloc(NUM_NODES * sizeof (int)); /**< x coordinates of lane junctions */
    int* yCords = malloc(NUM_NODES * sizeof (int)); /**< y coordinates of lane junctions */

    int* minX = malloc(NUM_NODES * sizeof (int)); /**< minimum x coordinates of road junctions */
    int* minY = malloc(NUM_NODES * sizeof (int)); /**< minimum y coordinates of road junctions */
    int* maxX = malloc(NUM_NODES * sizeof (int)); /**< maximum x coordinates of road junctions */
    int* maxY = malloc(NUM_NODES * sizeof (int)); /**< maximum y coordinates of road junctions */

    int* hash = malloc(NUM_NODES * sizeof (int)); /**< The road junction for eac of the lane junction */

    int actualNum = 0; /**< Total number of road junctions */


    int i, j;

    for (i = 0; i < NUM_NODES; i++) { // read x & y coordinates from the files
        fscanf(nodeFile, "%d", &xCords[i]);
        fscanf(nodeFile, "%d", &yCords[i]);
    }

    minX[0] = xCords[0];
    minY[0] = yCords[0];
    maxX[0] = xCords[0];
    maxY[0] = yCords[0];
    hash[0] = 0;

    actualNum++;
    int flag = 0;

    for (i = 1; i < NUM_NODES; i++) {
        flag = 0;
        int x = xCords[i];
        int y = yCords[i];
        for (j = 0; j < actualNum; j++) {
            printf("%d: %d %d %d %d %d %d %d %d\n", j, x, y, minX[j], maxX[j], minY[j], maxY[j], abs(x - minX[j]), abs(y - minY[j]));
			// if the lane junction i is tha part of road junction j
            if ((abs(x - minX[j]) < xTh && abs(y - minY[j]) < yTh) || (abs(x - minX[j]) < xTh && abs(y - maxY[j]) < yTh) || (abs(x - maxX[j]) < xTh && abs(y - maxY[j]) < yTh) || (abs(x - maxX[j]) < xTh && abs(y - minY[j]) < yTh)) {
                if (x < minX[j]) minX[j] = x;
                if (y < minY[j]) minY[j] = y;
                if (x > maxX[j]) maxX[j] = x;
                if (y > maxY[j]) maxY[j] = y;
                hash[i] = j;
                flag = 1;
                break;
            }
        }
        if (flag == 0) { // if the lane junction is not the part of the existing road junctions. Create a new road juntion
            minX[actualNum] = x;
            minY[actualNum] = y;
            maxX[actualNum] = x;
            maxY[actualNum] = y;
            hash[i] = actualNum;
            actualNum++;
        }
    }

    int* actualX = malloc(actualNum * sizeof (int));/**< x coordinates of the mid points of the road junctions */
    int* actualY = malloc(actualNum * sizeof (int));/**< y coordinates of the mid points of the road junctions */

    for (i = 0; i < actualNum; i++) {
        actualX[i] = (minX[i] + maxX[i]) / 2;
        actualY[i] = (minY[i] + maxY[i]) / 2;
    }

    FILE *actNodeFile; /**< file containing the number of road junctions and their x and y coordinates */
    actNodeFile = fopen("nodeAct", "w");

    fprintf(actNodeFile, "%d ", actualNum);
    for (i = 0; i < actualNum; i++) {
        //write x & y coordinates to file
        fprintf(actNodeFile, "%d ", actualX[i]);
        fprintf(actNodeFile, "%d ", actualY[i]);
    }

    fclose(actNodeFile);
    int binary;
    int graph[actualNum][actualNum];/**< adjacency matrix for road junctions where each edge contains number of lanes in the road*/
    FILE *actEdgeFile;
    actEdgeFile = fopen("edgesAct", "w");
    for (i = 0; i < actualNum; i++) {
        for (j = 0; j < actualNum; j++) {
            graph[i][j] = 0;
        }
    }

    for (i = 0; i < NUM_NODES; i++) {
        for (j = 0; j < NUM_NODES; j++) {
            fscanf(edgeFile, "%d", &binary);
            if (binary == 1) {
                if (hash[i] != hash[j]) {
                    graph[hash[i]][hash[j]]++;
                }
            }
        }
    }

    for (i = 0; i < actualNum; i++) {
        for (j = 0; j < actualNum; j++) {
            //write number of lanes in each edge to file
            fprintf(actEdgeFile, "%d ", graph[i][j]);
        }
    }

    fclose(actEdgeFile);
    FILE *botFile;
    botFile = fopen("botPos", "r");

    int fx, fy, bx, by;

    fscanf(botFile, "%d", &fx);
    fscanf(botFile, "%d", &fy);
    fscanf(botFile, "%d", &bx);
    fscanf(botFile, "%d", &by);
    printf("here\n");
    int junc = hash[find_nearest_junction(xCords, yCords, fx, fy, bx, by)];/**< nearest node to the bot. This will serve as the source node */
    printf("here\n");
    printf("nodeOR %d\n", junc);

    FILE *startFile;
    startFile = fopen("start", "w");

    fprintf(startFile, "%d", junc);

}
