#ifndef DECIDE_H
#define DECIDE_H

// Flood fill (BFS) outward from the current cell through known, visited,
// non-red, wall-free cells. Finds the nearest cell that unexploredTiles()
// says has unexplored neighbours. Writes its coordinates to targetX/targetY
// and returns true, or returns false if there is no such cell.
bool floodFillTarget(int &targetX, int &targetY){
    bool seen[MAZE_WIDTH][MAZE_HEIGHT];
    int  queueX[MAZE_WIDTH * MAZE_HEIGHT];
    int  queueY[MAZE_WIDTH * MAZE_HEIGHT];
    int  head = 0, tail = 0;

    for(int x = 0; x < MAZE_WIDTH; x++){
        for(int y = 0; y < MAZE_HEIGHT; y++){
            seen[x][y] = false;
        }
    }

    seen[currentX][currentY] = true;
    queueX[tail] = currentX;
    queueY[tail] = currentY;
    tail++;

    while(head < tail){
        int x = queueX[head];
        int y = queueY[head];
        head++;

        // Found a frontier cell (skip the cell we're standing on).
        if(!(x == currentX && y == currentY) &&
           unexploredTiles(x - currentX, y - currentY) > 0){
            targetX = x;
            targetY = y;
            return true;
        }

        for(int d = 0; d < 4; d++){
            int nx = x + dirStep[d][0];
            int ny = y + dirStep[d][1];

            if(!validCell(nx, ny) || seen[nx][ny])
                continue;

            // Only travel through known cells (allowUnvisited = false).
            if(!canStep(x, y, d, false, -1, -1))
                continue;

            seen[nx][ny] = true;
            queueX[tail] = nx;
            queueY[tail] = ny;
            tail++;
        }
    }

    return false;
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
            driveTo(checkpointX, checkpointY);
            wait(1000, msec);
            driveTo(0,0);
            gState = STATE_IDLE;
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
            }

            // Move one step toward the best adjacent tile found above.
            if(greatest > 0){
                heading = bestDirection;

                if(heading == 0)
                    currentY++;
                else if(heading == 90)
                    currentX++;
                else if(heading == 180)
                    currentY--;
                else if(heading == 270)
                    currentX--;

            } else if(backup == true) {

                // 4. Nothing left to explore anywhere reachable: backtrack
                //    through the least-visited open neighbour.
                int lowestVisits = 999999999;

                int northVisits = visitedAt(currentX, currentY + 1);
                int eastVisits  = visitedAt(currentX + 1, currentY);
                int southVisits = visitedAt(currentX, currentY - 1);
                int westVisits  = visitedAt(currentX - 1, currentY);

                if(maze[currentX][currentY].north == absent &&
                   northVisits < lowestVisits){

                    lowestVisits = northVisits;
                    heading = 0;
                }

                if(maze[currentX][currentY].east == absent &&
                   eastVisits < lowestVisits){

                    lowestVisits = eastVisits;
                    heading = 90;
                }

                if(maze[currentX][currentY].south == absent &&
                   southVisits < lowestVisits){

                    lowestVisits = southVisits;
                    heading = 180;
                }

                if(maze[currentX][currentY].west == absent &&
                   westVisits < lowestVisits){

                    lowestVisits = westVisits;
                    heading = 270;
                }

                if(lowestVisits != 999999999){
                    if(heading == 0)
                        currentY++;
                    else if(heading == 90)
                        currentX++;
                    else if(heading == 180)
                        currentY--;
                    else if(heading == 270)
                        currentX--;
                }
            } else if(backup2 = true){
                turnFor(720);
                while(WAIT_TIME){
                    
                }
                
            }
        }
    }

    updateMap();
    gState = STATE_MOVE;
}

#endif
