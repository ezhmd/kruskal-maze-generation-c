//
//  main.c
//  maze
//
//  Created by Ezzat Chamudi on 11/29/19.
//  Copyright © 2019 Ezzat Chamudi. All rights reserved.
//

#include <stdio.h>

#include <stdlib.h>

#include <sys/time.h>

#include <stdbool.h>

const int TOTAL_DIRECTIONS = 12;

const int TOP_LEFT = 0;
const int TOP = 1;
const int TOP_RIGHT = 2;

const int RIGHT_TOP  = 3;
const int RIGHT = 4;
const int RIGHT_BOTTOM = 5;

const int BOTTOM_RIGHT = 6;
const int BOTTOM = 7;
const int BOTTOM_LEFT = 8;

const int LEFT_BOTTOM = 9;
const int LEFT = 10;
const int LEFT_TOP = 11;

const int MAZE_SIZE = 4;

int ** maze_draft;

bool three_equality(int x, int y, int z) {
    bool result = (x == y) && (y == z) && (z == x);
    return result;
}

unsigned char * available_directions(int x, int y) {
    int legality_counter = 0;

    bool legality[TOTAL_DIRECTIONS] = {false};
    
    bool near_top_border = y == 0;
    bool near_right_border = x == (MAZE_SIZE - 1);
    bool near_bottom_border = y == (MAZE_SIZE - 1);
    bool near_left_border = x == 0;

    // Check top left
    if (!near_top_border
        && !near_left_border
        && three_equality(maze_draft[x][y], maze_draft[x][y - 1], maze_draft[x - 1][y - 1])) {
        legality[TOP_LEFT] = true;
        legality_counter++;
    }

    // Check top
    if (!near_top_border && (maze_draft[x][y] != maze_draft[x][y - 1])) {
        legality[TOP] = true;
        legality_counter++;
    }
    
    // Check top right
    if (!near_top_border
        && !near_right_border
        && three_equality(maze_draft[x][y], maze_draft[x][y - 1], maze_draft[x + 1][y - 1])) {
        legality[TOP_RIGHT] = true;
        legality_counter++;
    }

    // Check right top
    if (!near_right_border
        && !near_top_border
        && three_equality(maze_draft[x][y], maze_draft[x + 1][y], maze_draft[x + 1][y - 1])) {
        legality[RIGHT_TOP] = true;
        legality_counter++;
    }

    // Check right
    if (!near_right_border && (maze_draft[x][y] != maze_draft[x + 1][y])) {
        legality[RIGHT] = true;
        legality_counter++;
    }

    // Check right bottom
    if (!near_right_border
        && !near_bottom_border
        && three_equality(maze_draft[x][y], maze_draft[x + 1][y], maze_draft[x + 1][y + 1])) {
        legality[RIGHT_BOTTOM] = true;
        legality_counter++;
    }

    // Check bottom right
    if (!near_bottom_border
        && !near_right_border
        && three_equality(maze_draft[x][y], maze_draft[x][y + 1], maze_draft[x + 1][y + 1])) {
        legality[BOTTOM_RIGHT] = true;
        legality_counter++;
    }

    // Check bottom
    if (!near_bottom_border && (maze_draft[x][y] != maze_draft[x][y + 1])) {
        legality[BOTTOM] = true;
        legality_counter++;
    }
    
    // Check bottom left
    if (!near_bottom_border
        && !near_left_border
        && three_equality(maze_draft[x][y], maze_draft[x][y + 1], maze_draft[x - 1][y + 1])) {
        legality[BOTTOM_LEFT] = true;
        legality_counter++;
    }

    // Check left bottom
    if (!near_left_border
        && !near_bottom_border
        && three_equality(maze_draft[x][y], maze_draft[x - 1][y], maze_draft[x - 1][y + 1])) {
        legality[LEFT_BOTTOM] = true;
        legality_counter++;
    }

    // Check left
    if (!near_left_border && (maze_draft[x][y] != maze_draft[x - 1][y])) {
        legality[LEFT] = true;
        legality_counter++;
    }

    // Check left top
    if (!near_left_border
        && !near_top_border
        && three_equality(maze_draft[x][y], maze_draft[x - 1][y], maze_draft[x - 1][y - 1])) {
        legality[LEFT_TOP] = true;
        legality_counter++;
    }


    unsigned char * array = (unsigned char * ) calloc(legality_counter + 1, sizeof(unsigned char));
    
    array[0] = legality_counter; // First element shows how many items are legal
    
    int index = 1;
    for (int i = 0; i < TOTAL_DIRECTIONS; i++) {
        if (legality[i]) {
            array[index] = i;
            index++;
        };
    }
    
    return array;
}

int ** maze() {
    // Initialize randomizer
    srand((unsigned) time(NULL));
    
    const int total_nodes = MAZE_SIZE * MAZE_SIZE;
    
    // Create maze draft
    maze_draft = (int ** ) calloc(MAZE_SIZE, sizeof(int * ));
    for (int i = 0; i < MAZE_SIZE; i++) {
        maze_draft[i] = (int * ) calloc(MAZE_SIZE, sizeof(int));
    }
    
    // Assign each cell in draft an integer identifier
    for (int x = 0; x < MAZE_SIZE; x++) {
        for (int y = 0; y < MAZE_SIZE; y++) {
            maze_draft[x][y] = x * MAZE_SIZE + y;
        }
    }
    
    // Print maze_draft
    printf("Initial maze draft:\n");
    for (int x = 0; x < MAZE_SIZE; x++) {
        for (int y = 0; y < MAZE_SIZE; y++) {
            printf("%d\t", maze_draft[y][x]);
        }
        printf("\n");
    }
    printf("\n");
    
    // Create graph
    int ** graph = (int ** ) calloc(total_nodes, sizeof(int * ));
    for (int i = 0; i < total_nodes; i++) {
        graph[i] = (int * ) calloc(total_nodes, sizeof(int));
    }
    
    int pass_number = 0;
    
    while (1) {
        pass_number++;

        int x1 = -1;
        int y1 = -1;
        int x2 = -1;
        int y2 = -1;
        int x3 = -1;
        int y3 = -1;

        x1 = rand() % MAZE_SIZE;
        y1 = rand() % MAZE_SIZE;
        
        unsigned char * directions = available_directions(x1, y1);
        unsigned char total_available_directions = directions[0];
        
        // If the selected node has no available direction, re-random
        if (total_available_directions == 0) {
            free(directions);
            continue;
        };

        printf("Pass: %d;\n");

        int selected_index = (rand() % total_available_directions) + 1;
        int selected_direction = directions[selected_index];
    
        printf("Available directions:\n");
        for(int i = 0; i < directions[0] + 1; i++) {
            printf("  -> %d\n", directions[i]);
        }
        
        free(directions);
        
        // diagonal flag
        bool is_diagonal;
        if(selected_direction != TOP
           && selected_direction != RIGHT
           && selected_direction != BOTTOM
           && selected_direction != LEFT) {
            is_diagonal = true;
        } else {
            is_diagonal = false;
        }
        
        // get second node coordinate
        switch (selected_direction) {
            case TOP_LEFT:
            case TOP:
            case TOP_RIGHT:
                x2 = x1;
                y2 = y1 - 1;
                break;
            case RIGHT_TOP:
            case RIGHT:
            case RIGHT_BOTTOM:
                x2 = x1 + 1;
                y2 = y1;
                break;
            case BOTTOM_RIGHT:
            case BOTTOM:
            case BOTTOM_LEFT:
                x2 = x1;
                y2 = y1 + 1;
                break;
            case LEFT_BOTTOM:
            case LEFT:
            case LEFT_TOP:
                x2 = x1 - 1;
                y2 = y1;
                break;
            default:
                printf("ERROR: Wrong direction code.");
                return NULL;
        }
        
        // get third node coordinate
        if(is_diagonal) {
            switch (selected_direction) {
                case TOP_LEFT:
                case LEFT_TOP:
                    x3 = x1 - 1;
                    y3 = y1 - 1;
                    break;
                case TOP_RIGHT:
                case RIGHT_TOP:
                    x3 = x1 + 1;
                    y3 = y1 - 1;
                    break;
                case BOTTOM_RIGHT:
                case RIGHT_BOTTOM:
                    x3 = x1 + 1;
                    y3 = y1 + 1;
                    break;
                case BOTTOM_LEFT:
                case LEFT_BOTTOM:
                    x3 = x1 - 1;
                    y3 = y1 + 1;
                    break;
                default:
                    printf("ERROR: Wrong diagonal direction code.");
                    return NULL;
            }
        }

        printf("Selected direction: %d;\n", selected_direction);
        if (is_diagonal) {
            printf("Selected nodes: (%d, %d), (%d, %d), and (%d, %d);\n", x1, y1, x2, y2, x3, y3);
        } else {
            printf("Selected nodes: (%d, %d) and (%d, %d);\n", x1, y1, x2, y2);
        }
        
        // Add link in the graph
        int node1_id = x1 * MAZE_SIZE + y1;
        int node2_id = x2 * MAZE_SIZE + y2;
        int node3_id = -1;
        
        if (is_diagonal) {
            node3_id = x3 * MAZE_SIZE + y3;
        }
        
        graph[node1_id][node2_id] = 1;
        graph[node2_id][node1_id] = 1;

        if (is_diagonal) {
            graph[node2_id][node3_id] = 1;
            graph[node3_id][node2_id] = 1;
        }

        // Unify rooms
        int node1_room = maze_draft[x1][y1];
        int node2_room = maze_draft[x2][y2];
        int node3_room = -1;
    
        if (is_diagonal) {
            node3_room = maze_draft[x3][y3];
        }

        for (int x = 0; x < MAZE_SIZE; x++) {
            for (int y = 0; y < MAZE_SIZE; y++) {
                if (maze_draft[x][y] == node1_room || maze_draft[x][y] == node2_room || maze_draft[x][y] == node3_room)
                    maze_draft[x][y] = node1_room;
            }
        }
        
        // Print maze_draft
        printf("Current maze draft:\n");
        for (int x = 0; x < MAZE_SIZE; x++) {
            for (int y = 0; y < MAZE_SIZE; y++) {
                printf("%d\t", maze_draft[y][x]);
            }
            printf("\n");
        }
        
        // Check if all nodes already in the same room
        bool already_in_the_same_room = true;
        int room_sample = maze_draft[0][0];
        
        for (int x = 0; x < MAZE_SIZE; x++) {
            for (int y = 0; y < MAZE_SIZE; y++) {
                int room_check = maze_draft[x][y];
                if (room_check != room_sample) {
                    printf("Still not in one room. Continuing...\n");
                    already_in_the_same_room = false;
                    x = total_nodes; // For breaking the outer loop
                    break;
                }
            }
        }
        
        // All done!!!
        if (already_in_the_same_room == true) {
            printf("DONE!\n\n");

            // Print graph
            printf("Final graph:\n");
            printf("\t");
            for (int i = 0; i < total_nodes; i++) {
                printf("%d\t", i);
            }
            printf("\n");

            for (int x = 0; x < total_nodes; x++) {
                printf("%d\t", x);

                for (int y = 0; y < total_nodes; y++) {
                    printf("%d\t", graph[x][y]);
                }
                printf("\n");
            }
            printf("\n");

            // Print all connections
            printf("All connections:\n");
            for (int i = 0; i < total_nodes; i++)
            {
                printf("Node [%d] -> ", i);
                
                for (int j = 0; j < total_nodes; j++) {
                    if (graph[i][j] == 1) {
                        printf("[%d]", j);
                    }
                }
                printf("\n");
            }
            printf("\n");

            break;
        }
        
        printf("\n");
    }
    
    free(maze_draft);
    
    return graph;
}

int main(int argc, const char * argv[]) {
    printf("Kruskal's Maze Generation!\n");
    maze();
    
    return 0;
}
