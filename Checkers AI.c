/* Program to print and play checker games.

  Skeleton program written by Artem Polyvyanyy, artem.polyvyanyy@unimelb.edu.au,
  September 2021, with the intention that it be modified by students
  to add functionality, as required by the assignment specification.

  Student Authorship Declaration:

  (1) I certify that except for the code provided in the initial skeleton file,
  the program contained in this submission is completely my own individual
  work, except where explicitly noted by further comments that provide details
  otherwise. I understand that work that has been developed by another student,
  or by me in collaboration with other students, or by non-students as a result
  of request, solicitation, or payment, may not be submitted for assessment in
  this subject. I understand that submitting for assessment work developed by
  or in collaboration with other students or non-students constitutes Academic
  Misconduct, and may be penalized by mark deductions, or by other penalties
  determined via the University of Melbourne Academic Honesty Policy, as
  described at https://academicintegrity.unimelb.edu.au.

  (2) I also certify that I have not provided a copy of this work in either
  softcopy or hardcopy or any other form to any other student, and nor will I
  do so until after the marks are released. I understand that providing my work
  to other students, regardless of my intention or any undertakings made to me
  by that other student, is also Academic Misconduct.

  (3) I further understand that providing a copy of the assignment specification
  to any form of code authoring or assignment tutoring service, or drawing the
  attention of others to such services and code that may have been made
  available via such a service, may be regarded as Student General Misconduct
  (interfering with the teaching activities of the University and/or inciting
  others to commit Academic Misconduct). I understand that an allegation of
  Student General Misconduct may arise regardless of whether or not I personally
  make use of such solutions or sought benefit from such actions.

  Signed by: Neel Prakashkumar Patel SN:1268914
  Dated:     06/10/2021

*/

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

/* some #define's from my sample solution ------------------------------------*/
#define BOARD_SIZE          8       // board size
#define ROWS_WITH_PIECES    3       // number of initial rows with pieces
#define CELL_EMPTY          '.'     // empty cell character
#define CELL_BPIECE         'b'     // black piece character
#define CELL_WPIECE         'w'     // white piece character
#define CELL_BTOWER         'B'     // black tower character
#define CELL_WTOWER         'W'     // white tower character
#define COST_PIECE          1       // one piece cost
#define COST_TOWER          3       // one tower cost
#define TREE_DEPTH          3       // minimax tree depth
#define COMP_ACTIONS        10      // number of computed actions

#define GAP_BETWEEN_PIECES  2       // gap between peices on original board
#define ARRAY_FORMATING     1       // counteracts arrays starting at 0
#define OFFSET_UCA          65      // ASCII code of character 'A'
#define OFFSET_0            48      // ASCII code of character '0'
#define MAX_BRANCHES        100     // maz number of branches of a node

#define LENGTH_OF_ACTION    5       // character length of an action
#define SOURCE_CELL_COL     0       // first char of action equal source col
#define SOURCE_CELL_ROW     1       // second char of action equal source row
#define CELL_SPERATOR       2       // seperates source and target cell
#define TARGET_CELL_COL     3       // fourth char of action equal target col
#define TARGET_CELL_ROW     4       // fifth char of action equal target row

#define LENGTH_OF_SPECIAL   1       // length of special actions

#define LENGTH_OF_CELL      2       // length of a cell string
#define CELL_COL            0       // first char of cell equals col
#define CELL_ROW            1       // second char of cell equals row

#define MOVE_RIGHT          1       // moves 1 col right on board
#define MOVE_UP            -1       // moves 1 row up on board
#define TRUE                1       // true value
#define FALSE               0       // false value

/* type definition --------------------------- -------------------------------*/
typedef unsigned char board_t[BOARD_SIZE][BOARD_SIZE];  // board type

typedef struct node node_t;  // node type   

//node
struct node{
    board_t board;                      // board with action applied
    char action[LENGTH_OF_ACTION];      // action taken
    int cost;                           // cost of board
    node_t *ptr[MAX_BRANCHES];          // pointers to all next action nodes
};

/* function prototypes -------------------------------------------------------*/
// functions for working with node
node_t *create_node(board_t board, char action[]);
void create_children(node_t* parent, board_t board, int turn_count);
void propagate_costs(node_t *node, int turn_count, board_t board);
void free_node(node_t* node);

/* my function prototypes ----------------------------------------------------*/
void setup_board(board_t board);
void print_board(board_t board);
void stage_0(board_t board,char action[],int turn_count, int stage);
int check_cell_valid(char cell[]);
int board_cost(board_t board);
char* check_ne(int prow, int pcol, char piece, board_t board);
char* check_se(int prow, int pcol, char piece, board_t board);
char* check_nw(int prow, int pcol, char piece, board_t board);
char* check_sw(int prow, int pcol, char piece, board_t board);
void copy_board(board_t new_board, board_t board);
void stage_1(board_t board,int turn_count);
void stage_2(board_t board, int turn_count);

/* functions -----------------------------------------------------------------*/
int
main(int argc, char *argv[]) {
    board_t board;
    char action[LENGTH_OF_ACTION];          
    int turn_count=1;

    setup_board(board);

    while(scanf("%s", action)!=EOF){

        if(strlen(action)==LENGTH_OF_SPECIAL && action[0]=='A'){
            printf("=====================================\n");
            stage_1(board, turn_count);
        }

        else if(strlen(action)==LENGTH_OF_SPECIAL && action[0]=='P'){     
            stage_2(board, turn_count);
        }

        else{
            stage_0(board, action, turn_count, 0);
        }

        turn_count++;
    }

    return EXIT_SUCCESS;            
}

/* Sets up the board and prints the setup lines */
void
setup_board(board_t board){
    int i, j;

    // goes through the first three lines and assigns them to 'w'/'.'
    for(i=0;i<ROWS_WITH_PIECES;i++){

        for(j=0;j<BOARD_SIZE;j++){
            
            // checks if it is the right place
            if((i+j)%GAP_BETWEEN_PIECES==1){
                board[i][j]=CELL_WPIECE;
            }

            else{
                board[i][j]=CELL_EMPTY;
            }

        }
    }

    // goes through the middle two rows and assigns them to '.'
    for(i=3;i<BOARD_SIZE-ROWS_WITH_PIECES;i++){

        for(j=0;j<BOARD_SIZE;j++){
            board[i][j]=CELL_EMPTY;
        }

    }

    // goes through the last three rows and assigns them to 'b'\'.'
    for(i=BOARD_SIZE-ROWS_WITH_PIECES;i<BOARD_SIZE;i++){

        for(j=0;j<BOARD_SIZE;j++){

            // checks if it is the right place
            if((i+j)%GAP_BETWEEN_PIECES==1){
                board[i][j]=CELL_BPIECE;
            }

            else{
                board[i][j]=CELL_EMPTY;
            }

        }
    }

    // prints all of the neccesay information
    printf("BOARD SIZE: 8x8\n");
    printf("#BLACK PIECES: 12\n");
    printf("#WHITE PIECES: 12\n");
    print_board(board);
}

/* prints the board */
void
print_board(board_t board){
    int i, j;
    printf("     ");

    // prints the alphabetic column values at top
    for(i=0;i<BOARD_SIZE;i++){

        // checks if it is the last column
        if(i==(BOARD_SIZE-1)){
            printf("%c", OFFSET_UCA+i);
        }

        else{
            printf("%c   ", OFFSET_UCA+i);
        }

    }

    printf("\n");

    // prints the rest of the information
    for(i=0;i<BOARD_SIZE;i++){

        printf("   +---+---+---+---+---+---+---+---+\n"); 
        printf(" %d |",i+ARRAY_FORMATING);  // prints row numbers

        for(j=0;j<BOARD_SIZE;j++){
            printf(" %c |", board[i][j]);   // prints values
        }

        printf("\n");
    }
    printf("   +---+---+---+---+---+---+---+---+\n");
}

/* checks wether an action is valid, if so, applys the change to the board */
void 
stage_0(board_t board,char action[],int turn_count, int stage){
    char source_cell[LENGTH_OF_CELL], target_cell[LENGTH_OF_CELL];
    char source_value, target_value, middle_value;
    int source_col_value, source_row_value;
    int target_col_value, target_row_value;
    int middle_col_value, middle_row_value;
    int i;
    char player;

    // checks which player is playing
    if(turn_count%2==0){
        player=CELL_WPIECE;
    }

    else{
        player=CELL_BPIECE;
    }

    // splits the cells from action to source cell and target cell
    for(i=0;i<2;i++){
        source_cell[i]=action[i];
        target_cell[i]=action[i+LENGTH_OF_ACTION-LENGTH_OF_CELL];
    }

    // assigns null byte to the end of both cells
    source_cell[LENGTH_OF_CELL]='\0';
    target_cell[LENGTH_OF_CELL]='\0';

    // cehcks if source cell is on the board
    if(!check_cell_valid(source_cell)){
        printf("ERROR: Source cell is outside of the board.\n");
        exit(EXIT_FAILURE);
    }

    // converts char values to corresponding int values 
    source_col_value=source_cell[CELL_COL]-OFFSET_UCA;
    source_row_value=source_cell[CELL_ROW]-OFFSET_0-ARRAY_FORMATING;
    source_value=board[source_row_value][source_col_value];

    // checks the validity of the move
    if(source_value==CELL_EMPTY){
        printf("ERROR: Source cell is empty.\n");
        exit(EXIT_FAILURE);
    }
    if(tolower(source_value)!=player){
        printf("ERROR: Source cell holds opponent's piece/tower.\n");
        exit(EXIT_FAILURE);
    }

    // checks if target cell on board 
    if(!check_cell_valid(target_cell)){
        printf("ERROR: Target cell is outside of the board.\n");
        exit(EXIT_FAILURE);
    }

    // converts char values to corresponding int values 
    target_col_value=target_cell[CELL_COL]-OFFSET_UCA;
    target_row_value=target_cell[CELL_ROW]-OFFSET_0-ARRAY_FORMATING;
    target_value=board[target_row_value][target_col_value];

    // checks the validity of the move
    if(target_value!=CELL_EMPTY && stage==0){
        printf("ERROR: Target cell is not empty.\n");
        exit(EXIT_FAILURE);
    }

    // checks if actions were legal
    if(abs(target_col_value-source_col_value)==1 ||
       abs(target_col_value-source_col_value)==2);

    else{
        printf("ERROR: Illegal action.\n");
        exit(EXIT_FAILURE);
    }

    if(abs(target_row_value-source_row_value)==1 || 
       abs(target_row_value-source_row_value)==2);

    else {
        printf("ERROR: Illegal action.\n");
        exit(EXIT_FAILURE);
    }

    // checks the move was a capture
    if(abs(target_col_value-source_col_value)==2 && 
       abs(target_row_value-source_row_value)==2){

        // assigns middle values
        middle_col_value=(target_col_value+source_col_value)/2;
        middle_row_value=(target_row_value+source_row_value)/2;
        middle_value=board[middle_row_value][middle_col_value];

        // check validity of the move
        if(middle_value==CELL_EMPTY && stage==0){
            printf("ERROR: Illegal action.\n");
            exit(EXIT_FAILURE);
        }

        else if(tolower(middle_value)==tolower(source_value) && stage==0){
            printf("ERROR: Illegal action.\n");
            exit(EXIT_FAILURE);
        }

        else if(target_value==CELL_EMPTY){
            // reasigns board to match action
            board[target_row_value][target_col_value]=source_value;
            board[source_row_value][source_col_value]=CELL_EMPTY;
            board[middle_row_value][middle_col_value]=CELL_EMPTY;
        }

        else{
            printf("ERROR: Illegal action.\n");
            exit(EXIT_FAILURE);
        }
    }

    else if(target_value==CELL_EMPTY){
        // reasigns board to match action
        board[target_row_value][target_col_value]=source_value;
        board[source_row_value][source_col_value]=CELL_EMPTY;
    }

    else{
        printf("ERROR: Target cell is not empty.\n");
        exit(EXIT_FAILURE);
    }
    
    target_value=board[target_row_value][target_col_value];
    // checks if it is a tower move
    if(target_row_value==0 && target_value==CELL_BPIECE){
        board[target_row_value][target_col_value]=CELL_BTOWER;
    }

    else if(target_row_value==(BOARD_SIZE-ARRAY_FORMATING) && 
            target_value==CELL_WPIECE){
        board[target_row_value][target_col_value]=CELL_WTOWER;
    }

    // if called due to stage 0, prints out all the information
    if(stage==0){

        printf("=====================================\n");

        if(turn_count%2==0){
            printf("WHITE ACTION #%d: %s\n", turn_count, action);
        }

        else{
            printf("BLACK ACTION #%d: %s\n", turn_count, action);
        }

        printf("BOARD COST: %d\n", board_cost(board));
        print_board(board);
    }
}

/* checks if a cell is whitin the board */
int
check_cell_valid(char cell[]){

    if( ('A'<=cell[CELL_COL] && cell[CELL_COL]<='H') &&
        ('1'<=cell[CELL_ROW] && cell[CELL_ROW]<='8') ){

        return 1;
    }

    return 0;
}

/* calculates the cost of a board */
int 
board_cost(board_t board){
    int black_piece=0, white_piece=0, 
        black_tower=0, white_tower=0;
    int i,j, cost;

    // goes through the board and finds total of all pieces 
    for(i=0;i<BOARD_SIZE;i++){

        for(j=0;j<BOARD_SIZE;j++){

            if(board[i][j]==CELL_BPIECE){
                black_piece++;
            }

            else if(board[i][j]==CELL_BTOWER){
                black_tower++;
            }

            else if(board[i][j]==CELL_WPIECE){
                white_piece++;
            }

            else if(board[i][j]==CELL_WTOWER){
                white_tower++;
            }

        }
    }

    cost= black_piece+3*black_tower-white_piece-3*white_tower;

    return cost;
}

/* copys the value of one to another */
void
copy_board(board_t new_board, board_t board){
    int i, j;

    // goes through each position in a board
    for(i=0;i<BOARD_SIZE;i++){

        for(j=0;j<BOARD_SIZE;j++){
            
            // assigns the value to new board
            new_board[i][j]=board[i][j];
        }
    }
}

/* figures out the next move to take */
void
stage_1(board_t board, int turn_count){
    int i, j;
    node_t *parent_node= create_node(board, "First"); // creates the first node
    node_t *new_parent_node;

    // creates the next level of children to the first node
    create_children(parent_node, board, turn_count);

    // goes through each branch of the first level and creates children to them
    for(i=0;i<MAX_BRANCHES;i++){

        // checks if there are more nodes to create children for
        if(parent_node->ptr[i]==NULL){
            break;
        }

        new_parent_node=parent_node->ptr[i]; 

        // creates children to the new node
        create_children(new_parent_node, new_parent_node->board, turn_count+1);
    }

    // goes through each branch of the second level and creates children to them
    for(i=0;i<MAX_BRANCHES;i++){

        if(parent_node->ptr[i]==NULL){
            break;
        }

        for(j=0;j<MAX_BRANCHES;j++){

            if(parent_node->ptr[i]->ptr[j]==NULL){
                break;
            }

            new_parent_node=parent_node->ptr[i]->ptr[j];
            create_children(new_parent_node, new_parent_node->board, 
                            turn_count+2);
        }
    }

    // propogates the costs from level 3 to first node
    propagate_costs(parent_node, turn_count, board);

    // frees all the nodes from memory
    free_node(parent_node); 
}

/* creates a singular node and returns a pointer to it */
node_t
*create_node(board_t board, char action[]){
    int i;
    // board_t new_board;

    // // copies the board to a new_board so the original board is unaffected
    // copy_board(new_board, board); 

    // creates space in the heap for the node
    node_t *temp= (node_t*)malloc(sizeof(node_t));
    assert(temp);

    // copies the action to the node action
    strcpy(temp->action, action);

    // assigns the board cost to the node cost
    temp->cost=board_cost(board);
    copy_board(temp->board, board);

    //  assigns all the pointers to none
    for(i=0;i<MAX_BRANCHES;i++){
        temp->ptr[i]=NULL;
    }

    return temp;
}

/* creates the branches to a node based on next possible actions */
void 
create_children(node_t* parent, board_t board, int turn_count){
    int i, j, count=0;
    char player, piece;
    char *action;
    board_t new_board;
    node_t *child_node;

    // checks what player will take the action
    if(turn_count%2==0){
        player=CELL_WPIECE;
    }
    else{
        player=CELL_BPIECE;
    }

    // goes through all the pieces on the board and creates the next node with
    // all possible moves
    for(i=0;i<BOARD_SIZE;i++){

        for(j=0;j<BOARD_SIZE;j++){

            piece=board[i][j];

            // checks the piece which should be tested
            if(tolower(piece)==player){
                
                // if piece is tower, checks all direction
                if(piece==CELL_BTOWER || piece==CELL_WTOWER){

                    // checks north east move of a piece
                    action= check_ne(i, j, piece, board);

                    // if action not NULL, then creates a node based on the 
                    // action and adds it to the parent node
                    if(strlen(action)== LENGTH_OF_ACTION){

                        // copies current board to new board to be altered
                        copy_board(new_board, board);

                        // alters the new board based on the action
                        stage_0(new_board, action, turn_count, 1);

                        // creates the node based on the information
                        child_node=create_node(new_board, action);

                        // parent node points to the childnode
                        parent->ptr[count]=child_node;
                        count++;
                    }

                    // checks south east
                    action= check_se(i, j, piece, board);

                    if(strlen(action)== LENGTH_OF_ACTION){
                        copy_board(new_board, board);
                        stage_0(new_board, action, turn_count, 1);
                        child_node=create_node(new_board, action);
                        parent->ptr[count]=child_node;
                        count++;
                    }

                    // checks south west
                    action= check_sw(i, j, piece, board);

                    if(strlen(action)== LENGTH_OF_ACTION){
                        copy_board(new_board, board);
                        stage_0(new_board, action, turn_count, 1);
                        child_node=create_node(new_board, action);
                        parent->ptr[count]=child_node;
                        count++;
                    }

                    // checks north west
                    action= check_nw(i, j, piece, board);

                    if(strlen(action)== LENGTH_OF_ACTION){
                        copy_board(new_board, board);
                        stage_0(new_board, action, turn_count, 1);
                        child_node=create_node(new_board, action);
                        parent->ptr[count]=child_node;
                        count++;
                    }

                }

                else if(player==CELL_WPIECE){

                    // checks south east
                    action= check_se(i, j, piece, board);

                    if(strlen(action)== LENGTH_OF_ACTION){
                        copy_board(new_board, board);
                        stage_0(new_board, action, turn_count, 1);
                        child_node=create_node(new_board, action);
                        parent->ptr[count]=child_node;
                        count++;
                    }

                    // checks south west
                    action= check_sw(i, j, piece, board);

                    if(strlen(action)== LENGTH_OF_ACTION){
                        copy_board(new_board, board);
                        stage_0(new_board, action, turn_count, 1);
                        child_node=create_node(new_board, action);
                        parent->ptr[count]=child_node;
                        count++;
                    }
                }

                else{

                    // checks north east
                    action= check_ne(i, j, piece, board);
                    
                    if(strlen(action)== LENGTH_OF_ACTION){
                        copy_board(new_board, board);
                        stage_0(new_board, action, turn_count, 1);
                        child_node=create_node(new_board, action);
                        parent->ptr[count]=child_node;
                        count++;
                    }

                    // checks north west
                    action= check_nw(i, j, piece, board);

                    if(strlen(action)== LENGTH_OF_ACTION){
                        copy_board(new_board, board);
                        stage_0(new_board, action, turn_count, 1);
                        child_node=create_node(new_board, action);
                        parent->ptr[count]=child_node;
                        count++;
                    }
                }
            }   
        }
    }
}

/* propogates the costs from bottom level to the top level */
void
propagate_costs(node_t *node, int turn_count, board_t board){
    int i, j, k;
    int count1, count2, count3, win=FALSE;
    node_t *level1, *level2, *level3;
    node_t *next_node;

    // goes through each level of branch until the bottom
    for(i=0;i<MAX_BRANCHES;i++){

        // breaks if there are no more nodes being pointed to by a node
        if(node->ptr[i]==NULL){
            break;
        }

        level1=node->ptr[i];

        // goes through level 2 nodes
        for(j=0;j<MAX_BRANCHES;j++){

            if(level1->ptr[j]==NULL){
                break;
            }

            level2=level1->ptr[j];

            // goes through level 3 nodes
            for(k=0;k<MAX_BRANCHES;k++){

                if(level2->ptr[k]==NULL){
                    break;
                }

                level3=level2->ptr[k];

                // applies max-min rule if white turn
                if((turn_count%2)==0){
                    
                    // finds the lowest cost
                    if(k==0 || (level3->cost)<count3){
                        count3=level3->cost;
                    }

                }
                
                // applie min-max rule if black turn
                else{
                    
                    // finds the highest cost
                    if(k==0 || (level3->cost)>count3){
                        count3=level3->cost;
                    }

                }
            }

            // assigns the level 2 node cost to the level 3 
            level2->cost=count3;
        }
    }

    // goes through each level of branch until level 2 
    for(i=0;i<MAX_BRANCHES && win==FALSE;i++){

        if(node->ptr[i]==NULL){
            break;
        }

        level1=node->ptr[i];

        // goes through level 2 nodes
        for(j=0;j<MAX_BRANCHES && win==FALSE;j++){

            if(level1->ptr[j]==NULL){

                // if no actions can be taken by the player after,
                // then that action should be taken 
                if(j==0){
                    win=TRUE;

                    // next node is assigned to the level1 action 
                    // which wins the game
                    next_node=level1;
                }

                break;
            }

            level2=level1->ptr[j];

            if((turn_count%2)==0){

                //finds the highest cost
                if(j==0 || (level2->cost)>count2){
                    count2=level2->cost;
                }

            }

            else{

                //finds the lowest cost
                if(j==0||(level2->cost)<count2){
                    count2=level2->cost;
                } 

            }
        }

        // level 2 node cost is assigned to level1
        level1->cost=count2;
    }

    // goes through all level 1 nodes if winning action is not found
    for(i=0;i<MAX_BRANCHES && win==FALSE;i++){

        if(node->ptr[i]==NULL){
            break;
        }

        level1=node->ptr[i];

        if((turn_count%2)==0){

            //finds the lowest cost
            if(i==0 || (level1->cost)<count1){

                // assigns the level 1 node to the next node
                next_node=level1;
                count1=level1->cost;
            }

        }
        else{

            //finds the highest cost
            if(i==0 || (level1->cost)>count1){

                // assigns the level 1 node to the next node
                next_node=level1;
                count1=level1->cost;
            }

        }
    }

    if((turn_count%2)==0){
        printf("*** WHITE ACTION #%d: %s\n", turn_count, next_node->action); 
    }

    else{
        printf("*** BLACK ACTION #%d: %s\n", turn_count, next_node->action);
    }

    printf("BOARD COST: %d\n", board_cost(next_node->board));
    print_board(next_node->board);

    // assigns the new board after the action to the original board
    copy_board(board, next_node->board);

    // displays winning messages if a player wins
    if(win){

        if((turn_count%2)==0){
            printf("WHITE WIN!\n");
        }

        else{
            printf("BLACK WIN!\n");
        }

        exit(EXIT_SUCCESS);
    }
}

/* repeats stage1 10 times */
void
stage_2(board_t board, int turn_count){
    int i;

    // goes through loop 10 times
    for(i=0;i<COMP_ACTIONS;i++){

        printf("=====================================\n");

        // calls stage1 with current board state
        stage_1(board, turn_count);
        turn_count++;
    }
}

/* frees the memory from all nodes in the heap */
void
free_node(node_t *node){
    int i;

    // goes through each node that is connected to it
    for(i=0;i<MAX_BRANCHES;i++){

        // when there are no more node connected to it
        if(node->ptr[i]==NULL){

            free(node);

            // breaks the function call
            return;
        }

        // calls free node with the node that is connected
        free_node(node->ptr[i]);
    }
}

/* checks if there is north east move that can be taken, if so returns the 
   action */
char*
check_ne(int prow, int pcol, char piece, board_t board){
    char source_cell[LENGTH_OF_CELL], target_cell[LENGTH_OF_CELL], target_value;
    char action[LENGTH_OF_ACTION], *paction=malloc(LENGTH_OF_ACTION);
    assert(paction);
    
    // assigns the char value to the corresponding int values
    source_cell[CELL_COL]=pcol+OFFSET_UCA-OFFSET_0+'0';
    source_cell[CELL_ROW]=prow+ARRAY_FORMATING+'0';
    source_cell[LENGTH_OF_CELL]='\0';

    target_cell[CELL_COL]=pcol+OFFSET_UCA-OFFSET_0+MOVE_RIGHT+'0';
    target_cell[CELL_ROW]=prow+ARRAY_FORMATING+MOVE_UP+'0';
    target_cell[LENGTH_OF_CELL]='\0';

    // checks if target cell within the board
    if(!check_cell_valid(target_cell)){
        free(paction);
        return "NULL";
    }

    target_value=board[prow+MOVE_UP][pcol+MOVE_RIGHT];

    // checks validity of move
    if(tolower(target_value)==tolower(piece)){
        free(paction);
        return "NULL";
    }

    else if(target_value==CELL_EMPTY){

        // assigns the values to the action
        action[SOURCE_CELL_COL]=source_cell[CELL_COL];
        action[SOURCE_CELL_ROW]=source_cell[CELL_ROW];
        action[CELL_SPERATOR]='-';
        action[TARGET_CELL_COL]=target_cell[CELL_COL];
        action[TARGET_CELL_ROW]=target_cell[CELL_ROW];
        action[LENGTH_OF_ACTION]='\0';

        // copies the action values to a pointer variable
        strcpy(paction, action);
        return paction;
    }

    else{

        // moves the target value to capture position
        target_cell[CELL_COL]=target_cell[CELL_COL]+MOVE_RIGHT;
        target_cell[CELL_ROW]=target_cell[CELL_ROW]+MOVE_UP;
        target_cell[LENGTH_OF_CELL]='\0';

        // checks if new target cell within board
        if(!check_cell_valid(target_cell)){
            free(paction);
            return "NULL";
        }

        target_value=board[prow+(MOVE_UP*2)][pcol+(MOVE_RIGHT*2)];

        if(target_value==CELL_EMPTY){

            // assigns the values to the action
            action[SOURCE_CELL_COL]=source_cell[CELL_COL];
            action[SOURCE_CELL_ROW]=source_cell[CELL_ROW];
            action[CELL_SPERATOR]='-';
            action[TARGET_CELL_COL]=target_cell[CELL_COL];
            action[TARGET_CELL_ROW]=target_cell[CELL_ROW];
            action[LENGTH_OF_ACTION]='\0';

            // copies the action values to a pointer variable
            strcpy(paction, action);
            return paction;
        }
    }
    
    free(paction);
    return "NULL";
}

/* checks if there is south east move that can be taken, if so returns the
   action */
char*
check_se(int prow, int pcol, char piece, board_t board){
    char source_cell[LENGTH_OF_CELL], target_cell[LENGTH_OF_CELL], target_value;
    char action[LENGTH_OF_ACTION], *paction=malloc(LENGTH_OF_ACTION);
    assert(paction);

    // assigns the char value to the corresponding int values
    source_cell[CELL_COL]=pcol+OFFSET_UCA-OFFSET_0+'0';
    source_cell[CELL_ROW]=prow+ARRAY_FORMATING+'0';
    source_cell[LENGTH_OF_CELL]='\0';

    target_cell[CELL_COL]=pcol+OFFSET_UCA-OFFSET_0+MOVE_RIGHT+'0';
    target_cell[CELL_ROW]=prow+ARRAY_FORMATING-MOVE_UP+'0';
    target_cell[LENGTH_OF_CELL]='\0';

    // checks if target cell within the board
    if(!check_cell_valid(target_cell)){
        free(paction);
        return "NULL";
    }

    target_value=board[prow-MOVE_UP][pcol+MOVE_RIGHT];

    // checks validity of move
    if(tolower(target_value)==tolower(piece)){
        free(paction);
        return "NULL";
    }

    else if(target_value==CELL_EMPTY){

        // assigns the values to the action
        action[SOURCE_CELL_COL]=source_cell[CELL_COL];
        action[SOURCE_CELL_ROW]=source_cell[CELL_ROW];
        action[CELL_SPERATOR]='-';
        action[TARGET_CELL_COL]=target_cell[CELL_COL];
        action[TARGET_CELL_ROW]=target_cell[CELL_ROW];
        action[LENGTH_OF_ACTION]='\0';

        // copies the action values to a pointer variable
        strcpy(paction, action);
        return paction;
    }

    else{

        // moves the target value to capture position
        target_cell[CELL_COL]=target_cell[CELL_COL]+MOVE_RIGHT;
        target_cell[CELL_ROW]=target_cell[CELL_ROW]-MOVE_UP;
        target_cell[LENGTH_OF_CELL]='\0';

        // checks if new target cell within board
        if(!check_cell_valid(target_cell)){
            free(paction);
            return "NULL";
        }

        target_value=board[prow-(MOVE_UP*2)][pcol+(MOVE_RIGHT*2)];

        if(target_value==CELL_EMPTY){

            // assigns the values to the action
            action[SOURCE_CELL_COL]=source_cell[CELL_COL];
            action[SOURCE_CELL_ROW]=source_cell[CELL_ROW];
            action[CELL_SPERATOR]='-';
            action[TARGET_CELL_COL]=target_cell[CELL_COL];
            action[TARGET_CELL_ROW]=target_cell[CELL_ROW];
            action[LENGTH_OF_ACTION]='\0';

            // copies the action values to a pointer variable
            strcpy(paction, action);
            return paction;
        }
    }

    free(paction);
    return "NULL";
}

/* checks if there is south west move that can be taken, if so returns the 
   action */
char*
check_sw(int prow, int pcol, char piece, board_t board){
    char source_cell[LENGTH_OF_CELL], target_cell[LENGTH_OF_CELL], target_value;
    char action[LENGTH_OF_ACTION], *paction=malloc(LENGTH_OF_ACTION);
    assert(paction);

    // assigns the char value to the corresponding int values
    source_cell[CELL_COL]=pcol+OFFSET_UCA-OFFSET_0+'0';
    source_cell[CELL_ROW]=prow+ARRAY_FORMATING+'0';
    source_cell[LENGTH_OF_CELL]='\0';

    target_cell[CELL_COL]=pcol+OFFSET_UCA-OFFSET_0-MOVE_RIGHT+'0';
    target_cell[CELL_ROW]=prow+ARRAY_FORMATING-MOVE_UP+'0';
    target_cell[LENGTH_OF_CELL]='\0';

    // checks if target cell within the board
    if(!check_cell_valid(target_cell)){
        free(paction);
        return "NULL";
    }

    target_value=board[prow-MOVE_UP][pcol-MOVE_RIGHT];

    // checks validity of move
    if(tolower(target_value)==tolower(piece)){
        free(paction);
        return "NULL";
    }

    else if(target_value==CELL_EMPTY){

        // assigns the values to the action
        action[SOURCE_CELL_COL]=source_cell[CELL_COL];
        action[SOURCE_CELL_ROW]=source_cell[CELL_ROW];
        action[CELL_SPERATOR]='-';
        action[TARGET_CELL_COL]=target_cell[CELL_COL];
        action[TARGET_CELL_ROW]=target_cell[CELL_ROW];
        action[LENGTH_OF_ACTION]='\0';

        // copies the action values to a pointer variable
        strcpy(paction, action);
        return paction;
    }

    else{

        // moves the target value to capture position
        target_cell[CELL_COL]=target_cell[CELL_COL]-MOVE_RIGHT;
        target_cell[CELL_ROW]=target_cell[CELL_ROW]-MOVE_UP;
        target_cell[LENGTH_OF_CELL]='\0';

        // checks if new target cell within board
        if(!check_cell_valid(target_cell)){
            free(paction);
            return "NULL";
        }

        target_value=board[prow-(MOVE_UP*2)][pcol-(MOVE_RIGHT*2)];

        if(target_value==CELL_EMPTY){

            // assigns the values to the action
            action[SOURCE_CELL_COL]=source_cell[CELL_COL];
            action[SOURCE_CELL_ROW]=source_cell[CELL_ROW];
            action[CELL_SPERATOR]='-';
            action[TARGET_CELL_COL]=target_cell[CELL_COL];
            action[TARGET_CELL_ROW]=target_cell[CELL_ROW];
            action[LENGTH_OF_ACTION]='\0';

            // copies the action values to a pointer variable
            strcpy(paction, action);
            return paction;
        }
    }

    free(paction);
    return "NULL";
}

/* checks if there is north west move that can be taken, if so returns the 
   action */
char*
check_nw(int prow, int pcol, char piece, board_t board){
    char source_cell[LENGTH_OF_CELL], target_cell[LENGTH_OF_CELL], target_value;
    char action[LENGTH_OF_ACTION], *paction=malloc(LENGTH_OF_ACTION);
    assert(paction);

    // assigns the char value to the corresponding int values
    source_cell[CELL_COL]=pcol+OFFSET_UCA-OFFSET_0+'0';
    source_cell[CELL_ROW]=prow+ARRAY_FORMATING+'0';
    source_cell[LENGTH_OF_CELL]='\0';

    target_cell[CELL_COL]=pcol+OFFSET_UCA-OFFSET_0-MOVE_RIGHT+'0';
    target_cell[CELL_ROW]=prow+ARRAY_FORMATING+MOVE_UP+'0';
    target_cell[LENGTH_OF_CELL]='\0';

    // checks if target cell within the board
    if(!check_cell_valid(target_cell)){
        free(paction);
        return "NULL";
    }

    target_value=board[prow+MOVE_UP][pcol-MOVE_RIGHT];

    // checks validity of move
    if(tolower(target_value)==tolower(piece)){
        free(paction);
        return "NULL";
    }

    else if(target_value==CELL_EMPTY){

        // assigns the values to the action
        action[SOURCE_CELL_COL]=source_cell[CELL_COL];
        action[SOURCE_CELL_ROW]=source_cell[CELL_ROW];
        action[CELL_SPERATOR]='-';
        action[TARGET_CELL_COL]=target_cell[CELL_COL];
        action[TARGET_CELL_ROW]=target_cell[CELL_ROW];
        action[LENGTH_OF_ACTION]='\0';

        // copies the action values to a pointer variable
        strcpy(paction, action);
        return paction;
    }
    else{

        // moves the target value to capture position
        target_cell[CELL_COL]=target_cell[CELL_COL]-MOVE_RIGHT;
        target_cell[CELL_ROW]=target_cell[CELL_ROW]+MOVE_UP;
        target_cell[LENGTH_OF_CELL]='\0';

        // checks if new target cell within board
        if(!check_cell_valid(target_cell)){
            free(paction);
            return "NULL";
        }

        target_value=board[prow+(MOVE_UP*2)][pcol-(MOVE_RIGHT*2)];

        if(target_value==CELL_EMPTY){

            // assigns the values to the action
            action[SOURCE_CELL_COL]=source_cell[CELL_COL];
            action[SOURCE_CELL_ROW]=source_cell[CELL_ROW];
            action[CELL_SPERATOR]='-';
            action[TARGET_CELL_COL]=target_cell[CELL_COL];
            action[TARGET_CELL_ROW]=target_cell[CELL_ROW];
            action[LENGTH_OF_ACTION]='\0';

            // copies the action values to a pointer variable
            strcpy(paction, action);
            return paction;
        }
    }

    free(paction);
    return "NULL";
}
/* THE END -------------------------------------------------------------------*/
/* algorithms are fun */