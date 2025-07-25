#include <string>
#include <random>
#include <iostream>
#include <chrono> // For seeding with time

void reveal_all_mines(std::string& board){ //i just need the string here.
    for(int i = 1; i<board.length();i=i+2){
        board[i-1] = 'r';
    }
}

bool is_this_near_that(int this_index, int that_index, int num_col){
    //this is a helper function that determines if a slot is adjacent to another slot, will hopefully be the cornerstone to chording AND number assignment on random board genereation.
    int horizontal_ref = that_index%num_col;
    int vertical_ref = that_index/num_col;
    int current_horizontal = this_index%num_col;
    int current_vertical = this_index/num_col;

    if (current_horizontal == horizontal_ref-1 || current_horizontal == horizontal_ref+1){
        if (current_vertical ==vertical_ref-1 || current_vertical ==vertical_ref+1 || current_vertical ==vertical_ref){
            return true;
        }
    }
    if (current_horizontal == horizontal_ref){
        if (current_vertical ==vertical_ref-1 || current_vertical ==vertical_ref+1){
            return true;
        }
    }
    return false;
}

int random_between(int min, int max) { //just google how to do RNG in c++, it's pretty much identical to the example i saw :)
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(min, max);
    return dis(gen);
}

// Helper functions handle the silly string data - 
char get_visible_state(int x, int y, std::string& board, int cols) { //n
    int index = (y * cols + x) * 2;
    return board[index];
}


char get_slot_value(int x, int y, std::string& board, int cols) { //see #pragma region MAP_TEXTURES in main.cpp for slot states
    int index = (y * cols + x) * 2;
    return board[index + 1];
}

char set_visible_state(int x, int y, std::string& board, int cols, char state) {//0-8 or "m" for mine
    int index = (y * cols + x) * 2;
    board[index] = state;
    return state;
}


void set_slot_value(int x, int y, std::string& board, int cols, char value){ //this will probably be used when generating board randomly.
    int index = (y * cols + x) * 2;
    board[index + 1] = value;
}

int get_index_from_xy(int x, int y, int cols){
    return y * cols + x;
}

std::string generate_random_board(int row, int col, int mine_count){

    std::string values = "";
    std::string board = "";
    int num_slots = row*col;
     // Gives 0 to (total_slots-1)

    for (int i=0;i < num_slots; i++){ //init board values
           values.append("0");
    }

    // Now place exactly mine_count mines in random positions
    int mines_placed = 0;
    while (mines_placed < mine_count) {
        int random_index = random_between(0, num_slots - 1);   // Random index 0 to total_slots-1
        
        if (values[random_index] == '0') {  // If this spot isn't already a mine
            values[random_index] = 'm';
            mines_placed++;
        }
        // If it's already a mine, the while loop continues and tries again
    }

    // After generating the values string with mines and '0's
    for (int i = 0; i < values.length(); i++) {
        if (values[i] == '0') {  // If it's not a mine
            int mine_count_around = 0; 
        
            for (int j = 0; j < values.length(); j++) { // Check all other slots to see if they're adjacent mines
                if (values[j] == 'm' && is_this_near_that(i, j, col)) {
                    mine_count_around++;
                }
            }
            if (mine_count_around > 0) { // Replace the '0' with the actual count
            values[i] = '0' + mine_count_around;  // Convert int to char ('1', '2', etc.)
            }
        
        }
    }


    for (int i = 0; i < values.length(); i++){ //generate the actual board with the values string added in
        board.append("n");
        board += values[i];
    }

    return board;

}

void flood_reveal(int index, std::string& board, int cols){ //this is a recursive function that handles clicking on a slot with a value of "0"

    int index_x = index % cols;
    int index_y = index / cols;

    if (get_slot_value(index_x,index_y,board,cols) == '0' && get_visible_state(index_x,index_y,board,cols) != 'r'){ // only do anything with 0 value slots that have not been revealed yet.
        
        set_visible_state(index_x, index_y, board, cols, 'r');
        
        for (int i=0; i < board.length()/2;i++){ //loop over slot index numbers 

            int x = i % cols;
            int y = i / cols;

            if (is_this_near_that(i,index,cols) && get_visible_state(x,y,board,cols) != 'r'){
                
                if (get_slot_value(x,y,board,cols) == '0'){
                    flood_reveal(i,board,cols); //i flood reveal before revealing the original slot because if i don't, i get inifinite loop of misery
                }
                set_visible_state(x,y,board,cols,'r');
            }
        }
    } 

}

int count_flags_around_index(int index, std::string& board, int cols){

    int count = 0;

    for (int i = 0; i < board.length() / 2; i++){//loop over slot index numbers 

        if (is_this_near_that(i, index, cols)) {
            char state = board[i * 2];  // Get visible state directly - i could use get_visible_state but that's just an unneccesary wrap.
            if (state == 'N' || state == 'H') {  // Flagged
                count++;
            }
        }

    } 

    return count;
}

int count_unrevealed_neighbors(int index, std::string& board, int cols){

    int count = 0;

    for (int i = 0; i < board.length() / 2; i++){//loop over slot index numbers 

        if (is_this_near_that(i, index, cols)) {
            char state = board[i * 2];  // Get visible state directly - i could use get_visible_state but that's just an unneccesary wrap.
            if (state != 'r' && state != 'N' && state != 'H') {  // not revealed/flagged
                count++;
            }
        }

    } 

    return count;
}


void chord_reveal(int index, std::string& board, int cols){
    for (int i=0; i < board.length()/2;i++){ //loop over slot index numbers 

        if (is_this_near_that(i,index,cols)){
            int x = i % cols;
            int y = i / cols;
            if (get_visible_state(x,y,board,cols) != 'N' && get_visible_state(x,y,board,cols) != 'H'){ //if the slot isn't flagged
                flood_reveal (i,board,cols);
                set_visible_state(x, y, board, cols, 'r');            
            }
        }
    }
}

void flag_chord(int index, std::string& board, int cols){
    
    for (int i = 0; i < board.length() / 2; i++){
        if (is_this_near_that(i, index, cols)) {
            int x = i % cols;
            int y = i / cols;
            char state = get_visible_state(x, y, board, cols);
            
            if (state != 'r' && state != 'N' && state != 'H') {  // Not revealed and not flagged
                set_visible_state(x, y, board, cols, 'N');  // Flag it
            }
        }
    }
}


bool detect_win(std::string& board){
    /*
    the logic here is - if the number of non revealed slots is the same as the number of mines, return true. that is unless one of those revealed slots is actually a mine.
    */
    int minecount = 0;
    int hiddencount = board.length()/2;
    for (int i = 1;i < board.length();i=i+2){
        if (board[i] == 'm'){
            minecount++; //num mines without passing it. passing only the board is cooler.
        }
        else if (board[i] == 'x'){ //this is a loss so it should never trigger a win.
            return false;
        }
        else if (board[i-1] == 'r'){
            hiddencount--;
        }
    }
    //std::cout << hiddencount << std::endl;
    if(hiddencount != minecount){
        return false;
    }
    return true;
}

int calc_active_flags(std::string& board){
    int count = 0;
    for (int i = 0;i < board.length();i=i+2){
        if (board[i]== 'N' || board[i] == 'H'){
            count ++;
        }
    }
    return count;
  }
