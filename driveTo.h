#ifndef DRIVETO_H
#define DRIVETO_H
#ifndef CELL_SIZE
#define CELL_SIZE 400 // millimetres per maze cell
#endif
#ifndef MOVE_COST
#define MOVE_COST 10  // routing cost of driving one cell
#endif
#ifndef TURN_COST
#define TURN_COST 2   // routing cost of one 90 degree turn (turn() is slow: two-stage + wall squaring)
#endif
#ifndef NUM_STATES
#define NUM_STATES (MAZE_WIDTH * MAZE_HEIGHT * 4) // (x, y, facing)
#endif
#ifndef INF_COST
#define INF_COST 999999
#endif
#ifndef STATE_INDEX
#define STATE_INDEX(x, y, d) ((((x) * MAZE_HEIGHT) + (y)) * 4 + (d))
#endif

int dirStep[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}}; // N, E, S, W as {dx, dy}

//calls the function to check if there is a wall at the appropriate coordinate. 
wall wallOnSide(int x, int y, int d){
    if(d == north) return maze[x][y].north;
    if(d == east)  return maze[x][y].east;
    if(d == south) return maze[x][y].south;
    return maze[x][y].west;
}


//this functions checks if it is possible to go from the current position to the appropriate square. unvisited cells only work if the bool is set. 
bool canStep(int x, int y, int d, bool allowUnvisited, int targetX, int targetY){
    int nx = x + dirStep[d][0];
    int ny = y + dirStep[d][1];

    if(!validCell(nx, ny))
        return false;
    if(wallOnSide(x, y, d) != absent)                 // wall present or still unknown
        return false;
    if(wallOnSide(nx, ny, (d + 2) % 4) == present)    // neighbour disagrees
        return false;
    if(maze[nx][ny].tileType == tile_red)             // never drive onto a red tile
        return false;
    if(!allowUnvisited && maze[nx][ny].visited == 0 &&
       !(nx == targetX && ny == targetY))             // colour unknown: could be red
        return false;
    return true;

}


// allowUnvisited set to falsse means that it will never go to an unvisited tile and essentially back track through known stuff

// allowUnvisited set to true will allow the bot to go to unvisited tiles that may be faster.

bool driveTo(int targetX, int targetY, bool allowUnvisited = false){
    if(!validCell(targetX, targetY))
        return false;
    if(maze[targetX][targetY].tileType == tile_red)
        return false;
    if(targetX == currentX && targetY == currentY)
        return true;

    int cost[NUM_STATES];
    int prevState[NUM_STATES];
    bool settled[NUM_STATES];
    for(int i = 0; i < NUM_STATES; i++){
        cost[i] = INF_COST;
        prevState[i] = -1;
        settled[i] = false;
    }

    // Start from the way the robot is physically facing right now.
    int startDir = ((int)((heading + 45) / 90)) % 4;
    cost[STATE_INDEX(currentX, currentY, startDir)] = 0;

    int goalState = -1;

    while(true){
        // Pick the cheapest unsettled state.
        int best = -1;
        int bestCost = INF_COST;
        for(int s = 0; s < NUM_STATES; s++){
            if(!settled[s] && cost[s] < bestCost){
                bestCost = cost[s];
                best = s;
            }
        }
        if(best == -1)
            break; // nothing reachable is left

        settled[best] = true;

        int d = best % 4;
        int y = (best / 4) % MAZE_HEIGHT;
        int x = best / (4 * MAZE_HEIGHT);

        if(x == targetX && y == targetY){
            goalState = best;
            break;
        }

        // Turn left / right by 90 degrees (a U-turn is two of these).
        for(int t = 1; t <= 3; t += 2){
            int ns = STATE_INDEX(x, y, (d + t) % 4);
            if(!settled[ns] && bestCost + TURN_COST < cost[ns]){
                cost[ns] = bestCost + TURN_COST;
                prevState[ns] = best;
            }
        }

        // Drive forward one cell.
        if(canStep(x, y, d, allowUnvisited, targetX, targetY)){
            int ns = STATE_INDEX(x + dirStep[d][0], y + dirStep[d][1], d);
            if(!settled[ns] && bestCost + MOVE_COST < cost[ns]){
                cost[ns] = bestCost + MOVE_COST;
                prevState[ns] = best;
            }
        }
    }

    if(goalState == -1)
        return false; // no known route

    // Walk back from the goal to get the path, then read off the cell moves.
    int path[NUM_STATES];
    int pathLen = 0;
    for(int s = goalState; s != -1; s = prevState[s])
        path[pathLen++] = s;

    int moveDir[NUM_STATES];
    int moveCount = 0;
    for(int i = pathLen - 1; i > 0; i--){
        int cur = path[i];
        int nxt = path[i - 1];
        if(cur / 4 != nxt / 4) // different cell -> this was a drive step
            moveDir[moveCount++] = nxt % 4;
    }

    // Execute: merge consecutive steps in the same direction into one drive.
    drive.setDriveVelocity(DRIVE_SPEED, percent);
    int k = 0;
    while(k < moveCount){
        int d = moveDir[k];
        int run = 0;
        while(k < moveCount && moveDir[k] == d){
            run++;
            k++;
        }

        heading = d * 90;
        turn(heading);
        drive.driveFor(forward, CELL_SIZE * run, mm);

        currentX += dirStep[d][0] * run;
        currentY += dirStep[d][1] * run;
    }

    updateMap();
    return true;
}

#endif // DRIVETO_H
