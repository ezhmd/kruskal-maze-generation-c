//
//  randomized_kruskal.h
//  kruskal-maze-generation-c
//
//  Created by Ezzat Chamudi on 12/3/19.
//  Copyright © 2019 Ezzat Chamudi. All rights reserved.
//

#include "util.h"
#include "definitions.h"
#include "print_direction.h"
#include "print_maze_draft.h"
#include "print_maze.h"

#ifndef randomized_kruskal_h
#define randomized_kruskal_h

unsigned char *available_directions(int x, int y, int **maze_draft, int size, unsigned int options)
{
    const unsigned int enable_standard = 0b00000001;
    const unsigned int enable_diagonal = 0b00000010;
    const unsigned int enable_letters  = 0b00000100;

    int legality_counter = 0;

    bool legality[TOTAL_DIRECTIONS] = {false};

    bool near_top_border = y == 0;
    bool near_right_border = x == (size - 1);
    bool near_bottom_border = y == (size - 1);
    bool near_left_border = x == 0;

    // Check top left
    if ((options & enable_diagonal)
        && !near_top_border
        && !near_left_border
        && all_unique(maze_draft[x][y], maze_draft[x][y - 1], maze_draft[x - 1][y - 1]))
    {
        legality[TOP_LEFT] = true;
        legality_counter++;
    }

    // Check top
    if ((options & enable_standard)
        && !near_top_border
        && (maze_draft[x][y] != maze_draft[x][y - 1]))
    {
        legality[TOP] = true;
        legality_counter++;
    }

    // Check top right
    if ((options & enable_diagonal)
        && !near_top_border
        && !near_right_border
        && all_unique(maze_draft[x][y], maze_draft[x][y - 1], maze_draft[x + 1][y - 1]))
    {
        legality[TOP_RIGHT] = true;
        legality_counter++;
    }

    // Check right top
    if ((options & enable_diagonal)
        && !near_right_border
        && !near_top_border
        && all_unique(maze_draft[x][y], maze_draft[x + 1][y], maze_draft[x + 1][y - 1]))
    {
        legality[RIGHT_TOP] = true;
        legality_counter++;
    }

    // Check right
    if ((options & enable_standard)
        && !near_right_border
        && (maze_draft[x][y] != maze_draft[x + 1][y]))
    {
        legality[RIGHT] = true;
        legality_counter++;
    }

    // Check right bottom
    if ((options & enable_diagonal)
        && !near_right_border
        && !near_bottom_border
        && all_unique(maze_draft[x][y], maze_draft[x + 1][y], maze_draft[x + 1][y + 1]))
    {
        legality[RIGHT_BOTTOM] = true;
        legality_counter++;
    }

    // Check bottom right
    if ((options & enable_diagonal)
        && !near_bottom_border
        && !near_right_border
        && all_unique(maze_draft[x][y], maze_draft[x][y + 1], maze_draft[x + 1][y + 1]))
    {
        legality[BOTTOM_RIGHT] = true;
        legality_counter++;
    }

    // Check bottom
    if ((options & enable_standard)
        && !near_bottom_border
        && (maze_draft[x][y] != maze_draft[x][y + 1]))
    {
        legality[BOTTOM] = true;
        legality_counter++;
    }

    // Check bottom left
    if ((options & enable_diagonal)
        && !near_bottom_border
        && !near_left_border
        && all_unique(maze_draft[x][y], maze_draft[x][y + 1], maze_draft[x - 1][y + 1]))
    {
        legality[BOTTOM_LEFT] = true;
        legality_counter++;
    }

    // Check left bottom
    if ((options & enable_diagonal)
        && !near_left_border
        && !near_bottom_border
        && all_unique(maze_draft[x][y], maze_draft[x - 1][y], maze_draft[x - 1][y + 1]))
    {
        legality[LEFT_BOTTOM] = true;
        legality_counter++;
    }

    // Check left
    if ((options & enable_standard)
        && !near_left_border
        && (maze_draft[x][y] != maze_draft[x - 1][y]))
    {
        legality[LEFT] = true;
        legality_counter++;
    }

    // Check left top
    if ((options & enable_diagonal)
        && !near_left_border
        && !near_top_border
        && all_unique(maze_draft[x][y], maze_draft[x - 1][y], maze_draft[x - 1][y - 1]))
    {
        legality[LEFT_TOP] = true;
        legality_counter++;
    }

    unsigned char *array = (unsigned char *)calloc(legality_counter + 1, sizeof(unsigned char));

    array[0] = legality_counter; // First element shows how many items are legal

    int index = 1;
    for (int i = 0; i < TOTAL_DIRECTIONS; i++)
    {
        if (legality[i])
        {
            array[index] = i;
            index++;
        };
    }

    return array;
}

struct maze randomized_kruskal(bool verbose, int size, unsigned int direction_options)
{
    struct maze result;

    const int total_nodes = size * size;

    // Create maze draft
    int **maze_draft = (int **)calloc(size, sizeof(int *));
    for (int i = 0; i < size; i++)
    {
        maze_draft[i] = (int *)calloc(size, sizeof(int));
    }

    // Assign each cell in draft an integer identifier
    for (int x = 0; x < size; x++)
    {
        for (int y = 0; y < size; y++)
        {
            maze_draft[x][y] = x * size + y;
        }
    }

    // Print maze_draft
    if (verbose)
    {
        print_maze_draft(maze_draft, size);
        printf("\n");
    }

    // Create graph
    int **graph = (int **)calloc(total_nodes, sizeof(int *));
    for (int i = 0; i < total_nodes; i++)
    {
        graph[i] = (int *)calloc(total_nodes, sizeof(int));
    }

    int pass_number = 0;
    int failed_pass_number = 0;
    int rooms_counter = total_nodes;
    int fail_streak = 0;
    const int max_fail_streak = size * size * 10;

    while (1)
    {
        pass_number++;

        int x1 = -1;
        int y1 = -1;
        int x2 = -1;
        int y2 = -1;
        int x3 = -1;
        int y3 = -1;

        x1 = rand() % size;
        y1 = rand() % size;

        unsigned char *directions = available_directions(x1, y1, maze_draft, size, direction_options);
        unsigned char total_available_directions = directions[0];

        // If the selected node has no available direction, re-random
        if (total_available_directions == 0)
        {
            free(directions);
            failed_pass_number++;
            fail_streak++;
            
            if (fail_streak > max_fail_streak) {
                printf("ERROR: Too much fail streak. Can't combine all rooms using legal directions.\n");
                print_maze(graph, size);
                exit(1);
            }

            continue;
        } else {
            fail_streak = 0;
        }

        if (verbose)
        {
            printf("Pass: %d;\n", pass_number);
            printf("Selected first node: (%d, %d);\n", x1, y1);
        }

        int selected_index = (rand() % total_available_directions) + 1;
        int selected_direction = directions[selected_index];

        if (verbose)
        {
            printf("Available directions:\n");
            for (int i = 1; i < directions[0] + 1; i++)
            {
                printf(" -> ");
                print_direction(directions[i]);
                printf("\n");
            }
        }

        free(directions);

        // diagonal flag
        bool is_diagonal;
        if (selected_direction != TOP && selected_direction != RIGHT && selected_direction != BOTTOM && selected_direction != LEFT)
        {
            is_diagonal = true;
        }
        else
        {
            is_diagonal = false;
        }

        // get second node coordinate
        switch (selected_direction)
        {
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
            exit(1);
        }

        // get third node coordinate (if exists)
        if (is_diagonal)
        {
            switch (selected_direction)
            {
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
                exit(1);
            }
        }

        if (verbose)
        {
            printf("Selected direction: ");
            print_direction(selected_direction);
            printf("\n");

            if (is_diagonal)
            {
                printf("All nodes: (%d, %d), (%d, %d), and (%d, %d);\n", x1, y1, x2, y2, x3, y3);
            }
            else
            {
                printf("All nodes: (%d, %d) and (%d, %d);\n", x1, y1, x2, y2);
            }
        }

        // Add link in the graph
        int node1_id = x1 * size + y1;
        int node2_id = x2 * size + y2;
        int node3_id = -1;

        if (is_diagonal)
        {
            node3_id = x3 * size + y3;
        }

        graph[node1_id][node2_id] = 1;
        graph[node2_id][node1_id] = 1;

        if (is_diagonal)
        {
            graph[node2_id][node3_id] = 1;
            graph[node3_id][node2_id] = 1;
        }

        // Unify rooms
        int node1_room = maze_draft[x1][y1];
        int node2_room = maze_draft[x2][y2];
        int node3_room = -1;

        if (is_diagonal)
        {
            node3_room = maze_draft[x3][y3];
        }

        for (int x = 0; x < size; x++)
        {
            for (int y = 0; y < size; y++)
            {
                if (maze_draft[x][y] == node1_room || maze_draft[x][y] == node2_room || maze_draft[x][y] == node3_room)
                    maze_draft[x][y] = node1_room;
            }
        }

        // Print maze_draft
        if (verbose)
            print_maze_draft(maze_draft, size);

        // Recalculate the total rooms
        if (is_diagonal)
        {
            rooms_counter -= 2;
        }
        else
        {
            rooms_counter -= 1;
        }
        if (verbose)
            printf("Total rooms: %d\n", rooms_counter);

        // All done!!!
        if (rooms_counter == 1) {
            break;
        } else {
            if (verbose) printf("\n");
        }
    }

    result.total_deg1_nodes = 0;
    result.total_deg2_nodes = 0;
    result.total_deg3_nodes = 0;
    result.total_deg4_nodes = 0;

    // Print all connections
    for (int i = 0; i < total_nodes; i++)
    {
        int current_degree = 0;
        for (int j = 0; j < total_nodes; j++)
        {
            if (graph[i][j] == 1)
                current_degree++;
        }

        switch (current_degree)
        {
        case 1:
            result.total_deg1_nodes++;
            break;
        case 2:
            result.total_deg2_nodes++;
            break;
        case 3:
            result.total_deg3_nodes++;
            break;
        case 4:
            result.total_deg4_nodes++;
            break;
        default:
            printf("ERROR: Bad degree.");
            exit(1);
        }
    }

    if (verbose)
    {
        printf("DONE!\n\n");

        // Print graph
        printf("Final graph:\n");
        printf("\t");
        for (int i = 0; i < total_nodes; i++)
        {
            printf("[%d]\t", i);
        }
        printf("\n");

        for (int x = 0; x < total_nodes; x++)
        {
            printf("[%d]\t", x);

            for (int y = 0; y < total_nodes; y++)
            {
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

            for (int j = 0; j < total_nodes; j++)
            {
                if (graph[i][j] == 1)
                {
                    printf("[%d]", j);
                }
            }
            printf("\n");
        }
        printf("\n");

        // Print total passes
        printf("Total passes: %d\n", pass_number);
        printf("Total failed passes: %d\n", failed_pass_number);

        printf("\n");
    }

    // Free mems
    for (int i = 0; i < size; i++)
        free(maze_draft[i]);
    free(maze_draft);

    result.total_passes = pass_number;
    result.total_failed_passes = failed_pass_number;
    result.size = size;
    result.graph = graph;
    result.total_nodes = total_nodes;

    return result;
}

#endif /* randomized_kruskal_h */