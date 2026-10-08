#ifndef DECIDE_H
#define DECIDE_H

// Finds the cheapest-to-reach known cell (by driveTo's cost, so diagonal
// shortcuts count) that unexploredTiles() says has unexplored neighbours.
// Writes its coordinates to targetX/targetY and returns true, or returns
// false if there is no such cell.
bool floodFillTarget(int &targetX, int &targetY){
    int cost[NUM_STATES];
    int prevState[NUM_STATES];
    int goalState;
    planCosts(-1, -1, false, cost, prevState, goalState);

    int bestCost = INF_COST;
    bool found = false;

    for(int s = 0; s < NUM_STATES; s++){
        if(cost[s] >= bestCost)
            continue;

        int y = (s / 8) % MAZE_HEIGHT;
        int x = s / (8 * MAZE_HEIGHT);

        if(x == currentX && y == currentY)
            continue;

        if(unexploredTiles(x - currentX, y - currentY) > 0){
            bestCost = cost[s];
            targetX = x;
            targetY = y;
            found = true;
        }
    }

    return found;
}

void finishRun(){
    if(checkpoint){
        driveTo(checkpointX, checkpointY);
        wait(1000, msec);
    }
    driveTo(0, 0);
    gState = STATE_IDLE;
}

// Choose the next cell using the information gathered so far.
void decide(){
    tLed.on(blue);

    if(!renderMap){
        Brain.Screen.print("think");
    }

    // Red tiles are hazards. Reverse the direction we entered from.
    if(maze[currentX][currentY].tileType == tile_red){
        if(heading == 0){
            heading = 180;
            if(validCell(currentX, currentY - 1))
                currentY--;
        } else if(heading == 90){
            heading = 270;
            if(validCell(currentX - 1, currentY))
                currentX--;
        } else if(heading == 180){
            heading = 0;
            if(validCell(currentX, currentY + 1))
                currentY++;
        } else if(heading == 270){
            heading = 90;
            if(validCell(currentX + 1, currentY))
                currentX++;
        }
    } else {

        // Once all required people have been found and a checkpoint exists,
        // return to the checkpoint and then return to the start.
        if(person >= REQUIRED_PEOPLE && checkpoint){
            finishRun();
            return;
        }

        // 1. Prefer an immediately adjacent unvisited tile.
        if(maze[currentX][currentY].north == absent &&
           visitedAt(currentX, currentY + 1) == 0){

            heading = 0;
            currentY++;

        } else if(maze[currentX][currentY].east == absent &&
                  visitedAt(currentX + 1, currentY) == 0){

            heading = 90;
            currentX++;

        } else if(maze[currentX][currentY].south == absent &&
                  visitedAt(currentX, currentY - 1) == 0){

            heading = 180;
            currentY--;

        } else if(maze[currentX][currentY].west == absent &&
                  visitedAt(currentX - 1, currentY) == 0){

            heading = 270;
            currentX--;

        } else {

            // 2. No unvisited adjacent tile. Pick the open neighbour that
            //    has the most unexplored tiles around it.
            int greatest = 0;
            int bestDirection = -1;

            int northAdj = 0;
            int eastAdj  = 0;
            int southAdj = 0;
            int westAdj  = 0;

            if(maze[currentX][currentY].north == absent)
                northAdj = unexploredTiles(0, 1);

            if(maze[currentX][currentY].east == absent)
                eastAdj = unexploredTiles(1, 0);

            if(maze[currentX][currentY].south == absent)
                southAdj = unexploredTiles(0, -1);

            if(maze[currentX][currentY].west == absent)
                westAdj = unexploredTiles(-1, 0);

            if(northAdj > greatest){
                greatest = northAdj;
                bestDirection = 0;
            }

            if(eastAdj > greatest){
                greatest = eastAdj;
                bestDirection = 90;
            }

            if(southAdj > greatest){
                greatest = southAdj;
                bestDirection = 180;
            }

            if(westAdj > greatest){
                greatest = westAdj;
                bestDirection = 270;
            }

            // 3. Every adjacent tile is a dead end: flood fill through the
            //    known maze to the nearest cell that still has unexplored
            //    neighbours, then let driveTo() take the whole route there.
            //    driveTo() moves the robot and updates currentX/currentY
            //    itself, so skip STATE_MOVE and go straight to gathering info.
            if(greatest == 0){
                int targetX, targetY;
                if(floodFillTarget(targetX, targetY) &&
                   driveTo(targetX, targetY)){
                    gState = STATE_GATHER_INFO;
                    return;
                }
                finishRun();
                return;
            }

            heading = bestDirection;

            if(heading == 0)
                currentY++;
            else if(heading == 90)
                currentX++;
            else if(heading == 180)
                currentY--;
            else if(heading == 270)
                currentX--;
        }
    }

    updateMap();
    gState = STATE_MOVE;
}

#endif
