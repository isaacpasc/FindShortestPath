#include <random>
#include <iostream>
#include <fstream>
#include <queue>
using std::ofstream; 
using std::cout; 
using std::min; 
using std::pair; 
using std::queue; 
using std::uniform_int_distribution;
using std::mt19937;
using std::random_device;

// function declarations
void PrintEnvironment(int** array, int width, int height);
int** impassible2DArray(int** array, int height, int width, int impassible);
int** create2DArray(int height, int width);
bool isValid(int** array, int x, int y, int height, int width);
void wave(int** array, int xGoal, int yGoal, int width, int height);
char** path(int** array, int width, int height, int startX, int startY);
void PrintCharEnvironment(char** array, int width, int height);
bool isValidChar(int** array, int x, int y, int height, int width);
void outputMatrix(int** array, int width, int height, ofstream& outFile);
void outputMatrixChar(char** array, int width, int height, ofstream& outFile);

int main(int argc, char** argv) {
  if (argc == 1) {
    // if the file is run with 0 arguments
    // prompt for height/width
    int width;
    int height;
    printf("Welcome to CSE240 WaveFront Pather\n\nPlease tell me about the grid you want to generate.\nEnter the width (at least 10): ");
    scanf("%d", &width);
    while (width < 10) {
      // if width is less than 10, repeat prompt
      printf("Please enter a width of at least 10: ");
      scanf("%d",&width);
    }
    printf("Enter the height (at least 10): ");
    scanf("%d", &height);
    while (height < 10) {
      // if height is less than 10, repeat prompt
      printf("Please enter a height of at least 10: ");
      scanf("%d",&height);
    }
    // initialize 2d array of 0's
    int** my2DArray = create2DArray(height, width);
    int impassible;
    // prompt for impassible %
    printf("Enter the percent of impassible terrain (0 – 100): ");
    scanf("%d", &impassible);
    if (impassible > 40) {
      // if impassible % < 40, ask for confirmation
      char prompt;
      printf("Having a value greater than 40% might create poor results, do you with to continue? (y/n): ");
      scanf("%c", &prompt);
      while (prompt != 'y' && prompt != 'n') {
	// ask again if y/n is not entered
	printf("Having a value greater than 40% might create poor results, do you with to continue? (y/n): ");
	scanf("%c", &prompt);
      }
      if (prompt == 'n') {
	// if no, reprompt for impassible %
	printf("Enter the percent of impassible terrain (0 – 100): ");
	scanf("%d", &impassible);
      }
    }
    // create number of impassible points given w, h, and impassible %
    int impassibleNum = impassible/100.0*width*height;
    // fill 2d matrix of 0's with random -1's
    my2DArray = impassible2DArray(my2DArray, height, width, impassibleNum);
    // print 2d matrix of 0's and -1's
    PrintEnvironment(my2DArray, width, height);
    // create and open wave.txt file
    ofstream outFile;
    outFile.open("wave.txt");
    if (outFile) {
      // write 2d matrix of 0's and -1's to wave.txt
      outputMatrix(my2DArray, width, height, outFile);
    }
    int xGoal;
    int yGoal;
    bool invalid = true;
    while (invalid) {
      // prompt for x goal and y goal
      printf("Please enter the Goal Position X: ");
      scanf("%d", &xGoal);
      printf("Please enter the Goal Position Y: ");
      scanf("%d", &yGoal);
      // check for invalid goal point
      if (xGoal < 0 || xGoal > width-1) {
	printf("Sorry, the X position is out of bounds\n");
      } else if (yGoal< 0 || yGoal > height-1) {
	printf("Sorry, the Y position is out of bounds\n");
      } else if (my2DArray[yGoal][xGoal] == -1) {
	printf("Sorry, that position is inside an obstacle\n");
      } else {
	invalid = false;
      }
    }
    // set goal point in 2d matrix to 1
    my2DArray[yGoal][xGoal] = 1;
    // write over the 2d matrix to create wave pattern using BFS
    wave(my2DArray, yGoal, xGoal, width, height);
    invalid = true;
    int startX;
    int startY;
    while (invalid) {
      // prompt for start x and start y
      printf("Please enter the Start Position X: ");
      scanf("%d", &startX);
      printf("Please enter the Start Position Y: ");
      scanf("%d", &startY);
      // check for invalid start point
      if (startX < 0 || startX >= width) {
	printf("Sorry, the X position is out of bounds\n");
      } else if (startY < 0 || startY >= height) {
	printf("Sorry, the Y position is out of bounds\n");
      } else if (my2DArray[startY][startX] == -1 || my2DArray[startY][startX] == 0) {
	printf("Sorry, that position is inside an obstacle\n");
      } else {
	invalid = false;
      }
    }
    // create new 2d matrix of char's with '$' '@' '#'
    // AND using wave pattern and '*' find short path
    char** myCharArray = path(my2DArray, width, height, startY, startX);
    // print char matrix to stdout
    PrintCharEnvironment(myCharArray, width, height);
    if (outFile) {
      // write char matrix to wave.txt
      outputMatrixChar(myCharArray, width, height, outFile);
    }
    // close wave.txt
    outFile.close();
  } else {
    // entered a line argument
    printf("INVALID ARGUMENTS\nSyntax: <exe>");
  }
  return 0;
}

// function: creates 2d array of 0's
int** create2DArray(int height, int width) {
  // initialize matrix
  int** array2D = 0;
  array2D = new int*[height];
    
  for (int h = 0; h < height; h++)
    {
      array2D[h] = new int[width];
    
      for (int w = 0; w < width; w++)
	{
	  // set all values to 0
	  array2D[h][w] = 0;
	}
    }
    
  return array2D;
}

// function: outputs 2d array of ints to file 
void outputMatrix(int** array, int width, int height, ofstream& outFile) {
  for (int h = 0; h < height; h++)
    for (int w = 0; w < width; w++)
      // loop through matrix
      // add new line when on last value
      // if value is 0 add a space to line up with -1
      if (array[h][w] == 0) {
        outFile << " " << array[h][w] << " \n"[w == width-1];
      }else {
        outFile << array[h][w] << " \n"[w == width-1];
      }
  // add two lines between int matrix and char matrix
  outFile << "\n\n";
}

// function: outputs 2d array of chars to file 
void outputMatrixChar(char** array, int width, int height, ofstream& outFile) {
  for (int h = 0; h < height; h++)
    for (int w = 0; w < width; w++)
      // loop through matrix
      // add new line when on last value
      outFile << array[h][w] << " \n"[w == width-1];
}

// function: randomly inserts impassible number of -1's in matrix
int** impassible2DArray(int** array, int height, int width, int impassible) {
  // loop until number of -1's to insert is 0
  while (impassible > 0) {
    for (int h = 0; h < height; h++)
      {
	for (int w = 0; w < width; w++)
	  {
	    // when all -1's have been inserted, break while loop 
	    if (impassible < 1) {
	      break;
	    }
	    // create random device to decide if -1 should be inserted at given point
	    random_device dev;
	    mt19937 rng(dev());
	    // 33% chance to place -1
	    // this is not 50% because given a large matrix
	    // and small impassible %, -1's will be clustered at upper half
	    uniform_int_distribution<mt19937::result_type> dist6(0,2);
	    if (dist6(rng) == 0 && array[h][w] == 0) {
	      // instert -1
	      array[h][w] = -1;
	      // decrease impassible as a -1 has been inserted
	      impassible = impassible - 1;
	    } 
	  }
      }
  }
  return array;
}

// function: prints int matrix to stdout
void PrintEnvironment(int** array, int width, int height) {
  for (int h = 0; h < height; h++)
    for (int w = 0; w < width; w++)
      // if given int in matrix has 1 digit, add space
      // to line up with other 2 digit int's
      if (array[h][w] > -1 && array[h][w] < 10) {
	// new line after row has finished
	cout << " " << array[h][w] << " \n"[w == width-1];
      }else {
	cout << array[h][w] << " \n"[w == width-1];
      }
}

// function: prints char matrix to stdout
void PrintCharEnvironment(char** array, int width, int height) {
  for (int h = 0; h < height; h++)
    for (int w = 0; w < width; w++)
      // output to stdout with new line after row has finished
      cout << array[h][w] << " \n"[w == width-1];
}

// function: checks if point in int matrix is valid
bool isValid(int** array, int x, int y, int height, int width) {
  if (x < 0 || y < 0 || y >= width || x >= height) {
    // invalid if out of bounds
    return false;
  }
  if (array[x][y] != 0) {
    // invalid if point is -1 or visited
    return false;
  }
  return true;
}

// function: uses BFS to rank each pont in int matrix based on distance from start point
void wave(int** array, int xGoal, int yGoal, int width, int height) {
  // create queue to contain pairs
  queue<pair<int, int> > q;
  // dRow and dCol represent valid direction
  int dRow[] = { -1, 0, 1, 0 };
  int dCol[] = { 0, 1, 0, -1 };
  // push starting point to queue
  q.push({xGoal, yGoal});
  // repeat while until queue is empty, no more valid points
  while (!q.empty()) {
    // create pair to equal front of queue
    pair<int, int> cell = q.front();
    int x = cell.first;
    int y = cell.second;
    // after pair is declared, pop the point off the queue
    // queue should now be empty
    q.pop();
    // loop 4 times for each direction posibility
    for (int i = 0; i < 4; i++) {
      int adjx = x + dRow[i];
      int adjy = y + dCol[i];
      
      // check if each point neighbor is valid
      if (isValid(array, adjx, adjy, height, width)) {
	// if valid push to queue 
	q.push({ adjx, adjy });
	// set point to plus one from starting point
	array[adjx][adjy] = array[x][y] + 1;
      }
    }
  }
  // print wave array after BFS
  PrintEnvironment(array, width, height);
}

// function: creates char matrix from int matrix and finds shortest path
char** path(int** array, int width, int height, int startX, int startY) {
  // initialize char matrix
  char** array2D = 0;
  array2D = new char*[height];

  for (int h = 0; h < height; h++)
    {
      array2D[h] = new char[width];

      for (int w = 0; w < width; w++)
        {
	  // based on int matrix, set values for char matrix
	  if (array[h][w] == -1) {
	    array2D[h][w] = '#';
	  } else if (array[h][w] == 1) {
            array2D[h][w] = '$';
          } else if (startX == h && startY == w) {
            array2D[h][w] = '@';
          } else {
	    array2D[h][w] = ' ';
	  }
	}
    }
  // create queue of int pairs
  queue<pair<int, int> > q;
  // while true loop to find path to point
  while(true) {
    //check each neighbor's validity
    if (isValidChar(array, startX+1, startY+1, height, width)) {
      q.push({startX+1, startY+1});
      if (array2D[startX+1][startY+1] == '$') {
	// break if goal point reached
	break;
      }
    }
    // REPEAT
    if (isValidChar(array, startX+1, startY, height, width)) {
      q.push({startX+1, startY});
      if (array2D[startX+1][startY] == '$') {
	break;
      }                                                                                                                                             
    }
    if (isValidChar(array, startX+1, startY-1, height, width)) {
      q.push({startX+1, startY-1});                                                                                                                                             
      if (array2D[startX+1][startY-1] == '$') {
	break;
      }
    }
    if (isValidChar(array, startX, startY-1, height, width)) {
      q.push({startX, startY-1});                                                                                                                                             
      if (array2D[startX][startY-1] == '$') {
	break;
      }
    }
    if (isValidChar(array, startX-1, startY-1, height, width)) {
      q.push({startX-1, startY-1});                                                                                                                                             
      if (array2D[startX-1][startY-1] == '$') {
	break;
      }
    }
    if (isValidChar(array, startX-1, startY, height, width)) {
      q.push({startX-1, startY});                                                                                                                                             
      if (array2D[startX-1][startY] == '$') {
	break;
      }
    }
    if (isValidChar(array, startX-1, startY+1, height, width)) {
      q.push({startX-1, startY+1});                                                                                                                                             
      if (array2D[startX-1][startY+1] == '$') {
	break;
      }
    }
    if (isValidChar(array, startX, startY+1, height, width)) {
      q.push({startX, startY+1});                                                                                                                                             
      if (array2D[startX][startY+1] == '$') {
	break;
      }
    }
    // now that queue has all valid neighbors
    // find smallest number and set the char array to '*'
    // then pop all neighbors and set starting point to '*' point
    // REPEAT
    if (q.size() == 1) {
      pair<int, int> cell1 = q.front();
      int x1 = cell1.first;
      int y1 = cell1.second;
      q.pop();
      array2D[x1][y1] = '*';
      startX = x1;
      startY = y1;
    } else if (q.size() == 2) {
      pair<int, int> cell1 = q.front();
      int x1 = cell1.first;
      int y1 = cell1.second;
      q.pop();
      pair<int, int> cell2 = q.front();
      int x2 = cell2.first;
      int y2 = cell2.second;
      q.pop();
      int smallest = min(array[x1][y1], array[x2][y2]);
      if (smallest == array[x1][y1]) {
	array2D[x1][y1] = '*';
	startX = x1;
	startY = y1;
      } else if (smallest == array[x2][y2]) {
	array2D[x2][y2] = '*';
        startX = x2;
        startY = y2;
      }
    } else if (q.size() == 3) {
      pair<int, int> cell1 = q.front();
      int x1 = cell1.first;
      int y1 = cell1.second;
      q.pop();
      pair<int, int> cell2 = q.front();
      int x2 = cell2.first;
      int y2 = cell2.second;
      q.pop();
      pair<int, int> cell3 = q.front();
      int x3 = cell3.first;
      int y3 = cell3.second;
      q.pop();
      int smallest = min(array[x1][y1], min(array[x2][y2], array[x3][y3]));
      if (smallest == array[x1][y1]) {
	array2D[x1][y1] = '*';
        startX = x1;
        startY = y1;
      } else if(smallest == array[x2][y2]) {
        array2D[x2][y2] = '*';
        startX = x2;
        startY = y2;
      } else if(smallest == array[x3][y3]) {
        array2D[x3][y3] = '*';
        startX = x3;
        startY = y3;
      }
    } else if (q.size() == 4) {
      pair<int, int> cell1 = q.front();
      int x1 = cell1.first;
      int y1 = cell1.second;
      q.pop();
      pair<int, int> cell2 = q.front();
      int x2 = cell2.first;
      int y2 = cell2.second;
      q.pop();
      pair<int, int> cell3 = q.front();
      int x3 = cell3.first;
      int y3 = cell3.second;
      q.pop();
      pair<int, int> cell4 = q.front();
      int x4 = cell4.first;
      int y4 = cell4.second;
      q.pop();
      int smallest = min(array[x1][y1], min(array[x2][y2], min(array[x3][y3], array[x4][y4])));
      if (smallest == array[x1][y1]) {
        array2D[x1][y1] = '*';
        startX = x1;
        startY = y1;
      } else if(smallest == array[x2][y2]) {
        array2D[x2][y2] = '*';
        startX = x2;
        startY = y2;
      } else if(smallest == array[x3][y3]) {
        array2D[x3][y3] = '*';
        startX = x3;
        startY = y3;
      } else if(smallest == array[x4][y4]) {
        array2D[x4][y4] = '*';
        startX = x4;
        startY = y4;
      }
    } else if (q.size() == 5) {
      pair<int, int> cell1 = q.front();
      int x1 = cell1.first;
      int y1 = cell1.second;
      q.pop();
      pair<int, int> cell2 = q.front();
      int x2 = cell2.first;
      int y2 = cell2.second;
      q.pop();
      pair<int, int> cell3 = q.front();
      int x3 = cell3.first;
      int y3 = cell3.second;
      q.pop();
      pair<int, int> cell4 = q.front();
      int x4 = cell4.first;
      int y4 = cell4.second;
      q.pop();
      pair<int, int> cell5 = q.front();
      int x5 = cell5.first;
      int y5 = cell5.second;
      q.pop();
      int smallest = min(array[x1][y1], min(array[x2][y2], min(array[x3][y3], min(array[x4][y4], array[x5][y5]))));
      if (smallest == array[x1][y1]) {
        array2D[x1][y1] = '*';
        startX = x1;
        startY = y1;
      } else if(smallest == array[x2][y2]) {
        array2D[x2][y2] = '*';
        startX = x2;
        startY = y2;
      } else if(smallest == array[x3][y3]) {
        array2D[x3][y3] = '*';
        startX = x3;
        startY = y3;
      } else if(smallest == array[x4][y4]) {
        array2D[x4][y4] = '*';
        startX = x4;
        startY = y4;
      } else if(smallest == array[x5][y5]) {
        array2D[x5][y5] = '*';
        startX = x5;
        startY = y5;
      }
    } else if (q.size() == 6) {
      pair<int, int> cell1 = q.front();
      int x1 = cell1.first;
      int y1 = cell1.second;
      q.pop();
      pair<int, int> cell2 = q.front();
      int x2 = cell2.first;
      int y2 = cell2.second;
      q.pop();
      pair<int, int> cell3 = q.front();
      int x3 = cell3.first;
      int y3 = cell3.second;
      q.pop();
      pair<int, int> cell4 = q.front();
      int x4 = cell4.first;
      int y4 = cell4.second;
      q.pop();
      pair<int, int> cell5 = q.front();
      int x5 = cell5.first;
      int y5 = cell5.second;
      q.pop();
      pair<int, int> cell6 = q.front();
      int x6 = cell6.first;
      int y6 = cell6.second;
      q.pop();
      int smallest = min(array[x1][y1], min(array[x2][y2], min(array[x3][y3], min(array[x4][y4], min(array[x5][y5], array[x6][y6])))));
      if (smallest == array[x1][y1]) {
        array2D[x1][y1] = '*';
        startX = x1;
        startY = y1;
      } else if(smallest == array[x2][y2]) {
        array2D[x2][y2] = '*';
        startX = x2;
        startY = y2;
      } else if(smallest == array[x3][y3]) {
        array2D[x3][y3] = '*';
        startX = x3;
        startY = y3;
      } else if(smallest == array[x4][y4]) {
        array2D[x4][y4] = '*';
        startX = x4;
        startY = y4;
      } else if(smallest == array[x5][y5]) {
        array2D[x5][y5] = '*';
        startX = x5;
        startY = y5;
      } else if(smallest == array[x6][y6]) {
        array2D[x6][y6] = '*';
        startX = x6;
        startY = y6;
      }
    } else if (q.size() == 7) {
      pair<int, int> cell1 = q.front();
      int x1 = cell1.first;
      int y1 = cell1.second;
      q.pop();
      pair<int, int> cell2 = q.front();
      int x2 = cell2.first;
      int y2 = cell2.second;
      q.pop();
      pair<int, int> cell3 = q.front();
      int x3 = cell3.first;
      int y3 = cell3.second;
      q.pop();
      pair<int, int> cell4 = q.front();
      int x4 = cell4.first;
      int y4 = cell4.second;
      q.pop();
      pair<int, int> cell5 = q.front();
      int x5 = cell5.first;
      int y5 = cell5.second;
      q.pop();
      pair<int, int> cell6 = q.front();
      int x6 = cell6.first;
      int y6 = cell6.second;
      q.pop();
      pair<int, int> cell7 = q.front();
      int x7 = cell7.first;
      int y7 = cell7.second;
      q.pop();
      int smallest = min(array[x1][y1], min(array[x2][y2], min(array[x3][y3], min(array[x4][y4], min(array[x5][y5], min(array[x6][y6], array[x7][y7]))))));
      if (smallest == array[x1][y1]) {
        array2D[x1][y1] = '*';
        startX = x1;
        startY = y1;
      } else if(smallest == array[x2][y2]) {
        array2D[x2][y2] = '*';
        startX = x2;
        startY = y2;
      } else if(smallest == array[x3][y3]) {
        array2D[x3][y3] = '*';
        startX = x3;
        startY = y3;
      } else if(smallest == array[x4][y4]) {
        array2D[x4][y4] = '*';
        startX = x4;
        startY = y4;
      } else if(smallest == array[x5][y5]) {
        array2D[x5][y5] = '*';
        startX = x5;
        startY = y5;
      } else if(smallest == array[x6][y6]) {
        array2D[x6][y6] = '*';
        startX = x6;
        startY = y6;
      } else if(smallest == array[x7][y7]) {
        array2D[x7][y7] = '*';
        startX = x7;
        startY = y7;
      }
    } else if (q.size() == 8) {
      pair<int, int> cell1 = q.front();
      int x1 = cell1.first;
      int y1 = cell1.second;
      q.pop();
      pair<int, int> cell2 = q.front();
      int x2 = cell2.first;
      int y2 = cell2.second;
      q.pop();
      pair<int, int> cell3 = q.front();
      int x3 = cell3.first;
      int y3 = cell3.second;
      q.pop();
      pair<int, int> cell4 = q.front();
      int x4 = cell4.first;
      int y4 = cell4.second;
      q.pop();
      pair<int, int> cell5 = q.front();
      int x5 = cell5.first;
      int y5 = cell5.second;
      q.pop();
      pair<int, int> cell6 = q.front();
      int x6 = cell6.first;
      int y6 = cell6.second;
      q.pop();
      pair<int, int> cell7 = q.front();
      int x7 = cell7.first;
      int y7 = cell7.second;
      q.pop();
      pair<int, int> cell8 = q.front();
      int x8 = cell8.first;
      int y8 = cell8.second;
      q.pop();
      int smallest = min(array[x1][y1], min(array[x2][y2], min(array[x3][y3], min(array[x4][y4], min(array[x5][y5], min(array[x6][y6], min(array[x7][y7], array[x8][y8])))))));
      if (smallest == array[x1][y1]) {
        array2D[x1][y1] = '*';
        startX = x1;
        startY = y1;
      } else if(smallest == array[x2][y2]) {
        array2D[x2][y2] = '*';
        startX = x2;
        startY = y2;
      } else if(smallest == array[x3][y3]) {
        array2D[x3][y3] = '*';
        startX = x3;
        startY = y3;
      } else if(smallest == array[x4][y4]) {
        array2D[x4][y4] = '*';
        startX = x4;
        startY = y4;
      } else if(smallest == array[x5][y5]) {
        array2D[x5][y5] = '*';
        startX = x5;
        startY = y5;
      } else if(smallest == array[x6][y6]) {
        array2D[x6][y6] = '*';
        startX = x6;
	startY = y6;
      } else if(smallest == array[x7][y7]) {
        array2D[x7][y7] = '*';
        startX = x7;
        startY = y7;
      } else if(smallest == array[x8][y8]) {
        array2D[x8][y8] = '*';
        startX = x8;
        startY = y8;
      }
    } else if (q.size() == 0) {
      // no valid neighbors, path was not possible
      printf("No path was found");
      exit(0);
    }
  }
  return array2D;
}

// function: check if int matrix is valid (in use with char matrix creations)
bool isValidChar(int** array, int x, int y, int height, int width) {
  if (x < 0 || y < 0 || y >= width || x >= height) {
    // invalid if out of bounds
    return false;
  }
  if (array[x][y] == 0 || array[x][y] == -1) {
    // invalid if -1, this is impasssible, 0, this is unreachable from goal
    return false;
  }
  return true;
}
