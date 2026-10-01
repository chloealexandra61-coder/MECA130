#ifndef DRIVETO_H
#define DRIVETO_H
#ifndef CELL_SIZE
#define CELL_SIZE 400 // millimetres per maze cell (cardinal move distance)
#endif
#ifndef MOVE_COST
#define MOVE_COST 10  // routing cost of driving one cardinal cell
#endif
#ifndef TURN_COST
#define TURN_COST 2   // routing cost of turning 90 degrees (turn() is slow: two-stage + wall squaring)
#endif
#ifndef NUM_STATES
#define NUM_STATES (MAZE_WIDTH * MAZE_HEIGHT * 8) // (x, y, facing) - 8 facings, 45 degrees apart
#endif
#ifndef INF_COST
#define INF_COST 999999
#endif
#ifndef STATE_INDEX
#define STATE_INDEX(x, y, d) ((((x) * MAZE_HEIGHT) + (y)) * 8 + (d))
#endif

// A diagonal cell is sqrt(2) times further than a cardinal one, and only needs
// a 45 degree turn instead of 90, so these are derived from your existing
// MOVE_COST / TURN_COST rather than being separate numbers to tune.
#define DIAGONAL_MOVE_COST ((int)(MOVE_COST * 1.41421356 + 0.5))
#define DIAGONAL_TURN_COST ((TURN_COST + 1) / 2)
#define DIAGONAL_CELL_SIZE ((int)(CELL_SIZE * 1.41421356 + 0.5))

// 8 compass directions, 45 degrees apart, matching heading = d * 45.
// Even d (0,2,4,6) are cardinal - N, E, S, W, same meaning as before.
// Odd d (1,3,5,7) are diagonal - NE, SE, SW, NW.
int dirStep[8][2] = {
    {0, 1},   // 0  N
    {1, 1},   // 1  NE
    {1, 0},   // 2  E
    {1, -1},  // 3  SE
    {0, -1},  // 4  S
    {-1, -1}, // 5  SW
    {-1, 0},  // 6  W
    {-1, 1}   // 7  NW
};

// calls the function to check if there is a wall at the appropriate coordinate.
// only defined for cardinal d (0,2,4,6) - canStepDiagonal checks the two
// cardinal walls either side of a corner instead of calling this with a
// diagonal d.
wall wallOnSide(int x, int y, int d){
    if(d == 0) return maze[x][y].north;
    if(d == 2) return maze[x][y].east;
    if(d == 4) return maze[x][y].south;
    return maze[x][y].west; // d == 6
}

// this function checks if it is possible to go from the current position to
// the appropriate square. unvisited cells only work if the bool is set.
bool canStep(int x, int y, int d, bool allowUnvisited, int targetX, int targetY){
    int nx = x + dirStep[d][0];
    int ny = y + dirStep[d][1];

    if(!validCell(nx, ny))
        return false;
    if(wallOnSide(x, y, d) != absent)                 // wall present or still unknown
        return false;
    if(wallOnSide(nx, ny, (d + 4) % 8) == present)    // opposite side, 180 degrees away, disagrees
        return false;
    if(maze[nx][ny].tileType == tile_red)             // never drive onto a red tile
        return false;
    if(!allowUnvisited && maze[nx][ny].visited == 0 &&
       !(nx == targetX && ny == targetY))             // colour unknown: could be red
        return false;
    return true;
}

// Can the robot cut the corner from (x, y) straight onto the diagonal cell at
// d? Only allowed when BOTH right-angle routes around the corner are open
// (for NE: north-then-east AND east-then-north) - if either route has a known
// wall, there's something sitting in the corner and cutting it would clip it.
bool canStepDiagonal(int x, int y, int d, bool allowUnvisited, int targetX, int targetY){
    int nx = x + dirStep[d][0];
    int ny = y + dirStep[d][1];

    if(!validCell(nx, ny))
        return false;

    int compA = (d + 7) % 8; // cardinal side just one step back from d
    int compB = (d + 1) % 8; // cardinal side just one step forward from d

    int midAx = x + dirStep[compA][0], midAy = y + dirStep[compA][1];
    int midBx = x + dirStep[compB][0], midBy = y + dirStep[compB][1];

    bool routeA = wallOnSide(x, y, compA) == absent &&
                  validCell(midAx, midAy) &&
                  wallOnSide(midAx, midAy, compB) == absent;

    bool routeB = wallOnSide(x, y, compB) == absent &&
                  validCell(midBx, midBy) &&
                  wallOnSide(midBx, midBy, compA) == absent;

    if(!routeA || !routeB)
        return false;

    if(maze[nx][ny].tileType == tile_red)
        return false;
    if(!allowUnvisited && maze[nx][ny].visited == 0 &&
       !(nx == targetX && ny == targetY))
        return false;
    return true;
}

// allowUnvisited set to false means that it will never go to an unvisited
// tile and essentially back track through known stuff.
// allowUnvisited set to true will allow the bot to go to unvisited tiles that
// may be faster.
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

    // Start from the way the robot is physically facing right now, rounded to
    // the nearest 45 degrees.
    int startDir = (((int)((heading + 22.5) / 45)) % 8 + 8) % 8;
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

        int d = best % 8;
        int y = (best / 8) % MAZE_HEIGHT;
        int x = best / (8 * MAZE_HEIGHT);

        if(x == targetX && y == targetY){
            goalState = best;
            break;
        }

        // Turn 45 degrees either way (any bigger turn is just several of these).
        for(int t = 1; t <= 7; t += 6){ // t = 1 or 7 (i.e. +1 or -1 mod 8)
            int nd = (d + t) % 8;
            int ns = STATE_INDEX(x, y, nd);
            if(!settled[ns] && bestCost + DIAGONAL_TURN_COST < cost[ns]){
                cost[ns] = bestCost + DIAGONAL_TURN_COST;
                prevState[ns] = best;
            }
        }

        // Drive forward one cell: cardinal or diagonal depending on facing.
        if(d % 2 == 0){
            if(canStep(x, y, d, allowUnvisited, targetX, targetY)){
                int ns = STATE_INDEX(x + dirStep[d][0], y + dirStep[d][1], d);
                if(!settled[ns] && bestCost + MOVE_COST < cost[ns]){
                    cost[ns] = bestCost + MOVE_COST;
                    prevState[ns] = best;
                }
            }
        } else {
            if(canStepDiagonal(x, y, d, allowUnvisited, targetX, targetY)){
                int ns = STATE_INDEX(x + dirStep[d][0], y + dirStep[d][1], d);
                if(!settled[ns] && bestCost + DIAGONAL_MOVE_COST < cost[ns]){
                    cost[ns] = bestCost + DIAGONAL_MOVE_COST;
                    prevState[ns] = best;
                }
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
        if(cur / 8 != nxt / 8) // different cell -> this was a drive step
            moveDir[moveCount++] = nxt % 8;
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

        heading = d * 45;
        turn(heading);
        int cellDist = (d % 2 == 0) ? CELL_SIZE : DIAGONAL_CELL_SIZE;
        drive.driveFor(forward, cellDist * run, mm);

        currentX += dirStep[d][0] * run;
        currentY += dirStep[d][1] * run;
    }

    updateMap();
    return true;
}

#endif // DRIVETO_H
