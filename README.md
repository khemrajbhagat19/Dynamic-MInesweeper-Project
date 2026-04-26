The Dynamic Minesweeper project is an enhanced version of the classic logic-based puzzle
game Minesweeper, developed using the C++ programming language. Traditional Minesweeper
offers static gameplay, where the mine distribution remains unchanged throughout a session. This
project introduces a novel concept of Dynamic Mine Reshuffling and Adaptive Difficulty,
transforming the conventional game into an intelligent, responsive system that evolves based on
player actions.

The game operates on a two-dimensional grid of cells, some of which contain hidden mines.
Players interact with the board through commands to reveal or mark cells, aiming to uncover all
safe cells without triggering a mine. Unlike the standard version, this implementation integrates
a dynamic reshuffling algorithm that redistributes mines during gameplay while preserving
adjacency consistency around revealed zones. Additionally, when a player reveals cells with
identical adjacency counts consecutively, the system automatically removes one mine from the
board, progressively reducing difficulty and rewarding consistent logical play.

The project demonstrates core object-oriented programming principles such as encapsulation,
modularity, and abstraction through a class-based design. It employs recursive algorithms for
automatic expansion of zero-adjacency cells, robust randomization for mine placement, and
structured data handling using vectors for flexible board representation. These concepts
collectively make the system efficient, scalable, and easily extensible.

The program is implemented as a console-based application, ensuring platform independence
and simplicity. The user interacts via intuitive textual commands for revealing, marking,
reshuffling, or quitting the game. It also features comprehensive input validation, game state
tracking, and adaptive win/loss conditions.

Beyond entertainment, this project serves as a learning tool in computer science, illustrating
fundamental programming concepts, algorithmic problem-solving, and adaptive system design.
The resulting system provides a balance of challenge and fairness, demonstrating how traditional
gameplay can be enhanced through dynamic computational techniques and logical modeling.

Keywords: Minesweeper, C++, Dynamic Difficulty, Recursion, Object-Oriented Programming,
Algorithm Design, Game Logic, Adaptive System 
