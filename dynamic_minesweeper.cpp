#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm> // For std::shuffle

using namespace std;

// Forward declaration for the new method used in main
class Minesweeper;

struct Cell {
    bool hasMine = false;
    bool revealed = false;
    bool marked = false;
    int adjacentMines = 0;
};

class Minesweeper {
private:
    int width, height, mines, revealedCells;
    bool gameOver = false, won = false;
    vector< vector<Cell> > board;

    // --- NEW MEMBERS FOR DYNAMIC DIFFICULTY ---
    int lastRevealedAdjacency = -1; // Stores the adjacency count of the previously revealed cell
    int consecutiveSameAdjacencyCount = 0; // Tracks consecutive reveals with the same adjacency
    // ------------------------------------------

    // Helper directions array
    const int dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
    const int dy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};

    /**
     * @brief Places the initial set of mines randomly across the board.
     * This is only used in the constructor.
     */
    void placeMines() {
        int placed = 0;
        while (placed < mines) {
            int x = rand() % width;
            int y = rand() % height;
            if (!board[y][x].hasMine) {
                board[y][x].hasMine = true;
                placed++;
            }
        }
    }

    /**
     * @brief Calculates the adjacency count for every non-mine cell.
     * It explicitly resets the adjacency count for all cells first.
     */
    void calculateAdjacency() {
        // Reset all adjacency counts before recalculating
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                board[y][x].adjacentMines = 0;
            }
        }

        // Recalculate based on current mine placement
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (board[y][x].hasMine) continue;
                int count = 0;
                for (int i = 0; i < 8; ++i) {
                    int nx = x + dx[i], ny = y + dy[i];
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height)
                        if (board[ny][nx].hasMine)
                            count++;
                }
                board[y][x].adjacentMines = count;
            }
        }
    }

    /**
     * @brief Recursive helper to reveal cells starting at (x, y).
     */
    void revealRecursive(int x, int y) {
        if (x < 0 || x >= width || y < 0 || y >= height) return;
        Cell &cell = board[y][x];
        if (cell.revealed || cell.marked) return;

        cell.revealed = true;
        revealedCells++;

        // Continue revealing neighbors if it's a 0 cell
        if (cell.adjacentMines == 0 && !cell.hasMine) {
            for (int i = 0; i < 8; ++i) {
                revealRecursive(x + dx[i], y + dy[i]);
            }
        }
    }

    /**
     * @brief Handles the mine removal, board update, and win check when mines == 0.
     */
    void removeRandomMineAndHandleWin() {
        if (mines > 0) {
            // 1. Find all current mine locations
            vector<pair<int, int>> mineLocations;
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    if (board[y][x].hasMine) {
                        mineLocations.push_back({x, y});
                    }
                }
            }

            // 2. Decrement mine count and randomly remove one mine
            if (!mineLocations.empty()) {
                mines--;
                // Select a random mine index
                int targetIndex = rand() % mineLocations.size();
                pair<int, int> targetMine = mineLocations[targetIndex];
                
                // Remove the mine from the board (this is essential)
                board[targetMine.second][targetMine.first].hasMine = false;
                
                // Display feedback
                cout << "\n* * * 1 Mine Removed! Mines remaining: " << mines << " * * *\n";

                // 3. Check for win condition (mines = 0)
                if (mines == 0) {
                    won = true;
                    gameOver = true;
                    cout << "\n*** ALL MINES CLEARED! You win the Dynamic Challenge! ***\n";
                }
            }
        }
        
        // 4. Update the board (adjacency and reshuffle remaining mines)
        if (!gameOver) {
            reShuffleMines(); // reShuffleMines uses the new 'mines' count for floating mines
        }
    }


    /**
     * @brief Checks if the win condition is met.
     */
    void checkWin() {
        // Win condition 1: All non-mine cells are revealed
        if (revealedCells == width * height - mines) {
            won = true;
            gameOver = true;
        }
        // Win condition 2 (Handled in removeRandomMineAndHandleWin): Mines == 0
    }

    /**
     * @brief Helper to print a cell content
     */
    char cellDisplay(const Cell &c, bool revealAll) {
        if (revealAll) {
            if (c.hasMine) return '*';
            // Revealed cells show their count
            if (c.adjacentMines > 0) return '0' + c.adjacentMines;
            return '0'; // Empty cell
        }
        if (c.marked) return 'M';
        if (!c.revealed) return ' ';
        if (c.hasMine) return '*';
        if (c.adjacentMines == 0) return '0';
        return '0' + c.adjacentMines;
    }

public:
    Minesweeper(int w, int h, int m) : width(w), height(h), mines(m), revealedCells(0) {
        board.resize(height, vector<Cell>(width));
        // Seed the random number generator
        srand(time(0));
        placeMines();
        calculateAdjacency();
    }

    /**
     * @brief Implements the mandatory mine reshuffling logic.
     * Mines are moved such that the mine/safe status of any cell adjacent to a REVEALED cell is preserved.
     */
    void reShuffleMines() {
        vector<pair<int, int>> fixedMines; // Mines adjacent to revealed cells (must remain mines)
        vector<pair<int, int>> freeSlots;  // Unrevealed cells not adjacent to revealed cells (candidates for new mines)
        
        // 1. Identify fixed mines and free slots based on the current board state.
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                Cell& currentCell = board[y][x];

                // Check if cell is adjacent to any revealed cell
                bool is_neighbor_of_revealed = false;
                for (int i = 0; i < 8; ++i) {
                    int nx = x + dx[i], ny = y + dy[i];
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height && board[ny][nx].revealed) {
                        is_neighbor_of_revealed = true;
                        break;
                    }
                }

                if (is_neighbor_of_revealed) {
                    // RESTRICTED cell: Its current mine status MUST be preserved to maintain adjacency counts.
                    if (currentCell.hasMine) {
                        fixedMines.push_back({x, y});
                    }
                    // If it's safe (not a mine) and restricted, it must remain safe.
                } else if (!currentCell.revealed) {
                    // FREE cell: unrevealed and not restricted. Can host a mine.
                    freeSlots.push_back({x, y});
                }
                // Revealed cells must remain safe and revealed.
            }
        }

        // 2. Reset ALL mine flags
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                board[y][x].hasMine = false;
            }
        }

        // 3. Place Fixed Mines
        for (const auto& p : fixedMines) {
            board[p.second][p.first].hasMine = true;
        }

        // 4. Place Floating Mines (randomly in freeSlots)
        int remainingMinesToPlace = mines - fixedMines.size();

        if (remainingMinesToPlace > 0) {
            // Use rand() for a simple shuffling of the freeSlots vector
            for (size_t i = 0; i < freeSlots.size(); ++i) {
                swap(freeSlots[i], freeSlots[rand() % freeSlots.size()]);
            }

            // Place the remaining mines in the selected free slots
            int placedCount = 0;
            for (const auto& p : freeSlots) {
                if (placedCount >= remainingMinesToPlace) break;
                board[p.second][p.first].hasMine = true;
                placedCount++;
            }
        }

        // 5. Recalculate Adjacency based on the new mine placement
        calculateAdjacency();
    }


    /**
     * @brief Prints the board to the console.
     */
    void printBoard(bool revealAll = false) {
        // Print column numbers
        cout << "\n    ";
        for (int x = 0; x < width; x++)
            cout << x % 10 << "   ";
        cout << "\n";

        // Print top border
        cout << "   ";
        for (int x = 0; x < width; x++)
            cout << "+---";
        cout << "+\n";

        for (int y = 0; y < height; y++) {
            // Print cell content line
            cout << y % 10 << "  ";
            for (int x = 0; x < width; x++) {
                cout << "¦ " << cellDisplay(board[y][x], revealAll) << " ";
            }
            cout << "¦\n";

            // Print bottom of cells
            cout << "   ";
            for (int x = 0; x < width; x++)
                cout << "+---";
            cout << "+\n";
        }
    }

    /**
     * @brief Reveals a cell at (x, y).
     */
    void reveal(int x, int y) {
        if (x < 0 || x >= width || y < 0 || y >= height || board[y][x].marked) return;
        
        // If the cell is already revealed, we exit without affecting the streak
        if (board[y][x].revealed) return;

        if (board[y][x].hasMine) {
            // Mine hit, game over
            gameOver = true;
            won = false;
            return;
        }

        // --- New Logic: Check Adjacency Rule on the target cell before revealing ---
        int currentAdjacency = board[y][x].adjacentMines;
        
        if (currentAdjacency == lastRevealedAdjacency) {
            consecutiveSameAdjacencyCount++;
        } else {
            consecutiveSameAdjacencyCount = 1; // Start a new streak
        }
        lastRevealedAdjacency = currentAdjacency;

        // --- Perform the reveal (which might cascade) ---
        revealRecursive(x, y); 
        
        // --- Mine Reduction Logic ---
        if (consecutiveSameAdjacencyCount >= 2) {
            removeRandomMineAndHandleWin();
            // Reset streak after successful mine removal
            consecutiveSameAdjacencyCount = 0;
            lastRevealedAdjacency = -1; 
        }
        // --- End New Logic ---

        checkWin();
    }

    /**
     * @brief Marks or unmarks a cell at (x, y).
     */
    void mark(int x, int y) {
        if (x < 0 || x >= width || y < 0 || y >= height) return;
        if (!board[y][x].revealed)
            board[y][x].marked = !board[y][x].marked;
    }

    /**
     * @brief Implements the new 'a' command: shows all, then reshuffles.
     */
    void showAllAndShuffle() {
        printBoard(true); // revealAll = true
        cout << "\n[A] Mines revealed! Shuffling mines to unrevealed safe cells...\n";
        reShuffleMines();
        // The game should continue, so we don't set gameOver here.
    }

    bool isGameOver() const { return gameOver; }

    /**
     * @brief Ends the game by revealing the whole board and printing the outcome.
     */
    void endGame() {
        printBoard(true);
        if (won) cout << "\n*** You won! ***\n";
        else cout << "\n*** You hit a mine! Game Over. ***\n";
    }
};

int main() {
    // Note: srand is called in the constructor, but the initial seed is good here too.
    srand(time(0)); 

    int width = 9, height = 9, mines = 10;
    cout << "=== DYNAMIC MINESWEEPER ===\n";
    cout << "Enter width height mines (e.g., 9 9 10): ";
    if (!(cin >> width >> height >> mines)) {
        cerr << "Invalid input. Exiting.\n";
        return 1;
    }
    
    // Input validation
    if (width <= 0 || height <= 0 || mines <= 0 || mines >= width * height) {
        cerr << "Invalid board dimensions or mine count. Exiting.\n";
        return 1;
    }

    Minesweeper game(width, height, mines);

    while (!game.isGameOver()) {
        game.printBoard();
        cout << "\nCommand (r x y = reveal, m x y = mark, a = show all/shuffle, q = quit): ";
        char cmd; 
        
        if (!(cin >> cmd)) {
            cerr << "Input error. Exiting.\n";
            break;
        }

        if (cmd == 'q') break;
        
        int x = -1, y = -1; // Initialize to invalid values

        if (cmd == 'a') {
            game.showAllAndShuffle();
            continue; // Go straight to the next loop iteration (and board print)
        }

        // Read coordinates for 'r' and 'm' commands
        if (!(cin >> x >> y)) {
            cerr << "Missing coordinates for command. Try again.\n";
            // Clear bad state and ignore the rest of the line
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }


        if (cmd == 'r') {
            game.reveal(x, y);
            // If the game is still running (didn't hit a mine and didn't win), shuffle
            if (!game.isGameOver()) {
                game.reShuffleMines();
            }
        } else if (cmd == 'm') {
            game.mark(x, y);
            // Shuffle after mark/unmark command
            game.reShuffleMines();
        } else {
            cout << "Invalid command. Use r, m, a, or q.\n";
        }
    }

    game.endGame();
    return 0;
}
