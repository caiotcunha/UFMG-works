import heapq
import numpy as np
import sys
import pdb

class PuzzleNode:
    def __init__(self, state, parent, move, cost):
        self.state = state
        self.parent = parent
        self.move = move
        self.cost = cost

    def __lt__(self, other):
        return self.cost < other.cost

    def __eq__(self, other):
        return np.array_equal(self.state, other.state)


def print_solution(node):
    if node is None:
        return
    print_solution(node.parent)
    if node.move:
        print(node.move)
    for row in node.state:
        print(" ".join(str(cell) for cell in row))
    print("\n")

def get_blank_position(state):
    for i in range(3):
        for j in range(3):
            if state[i][j] == 0:
                return i, j

# os movimentos possíveis para o espaço em branco são:
def possible_moves(state):
    i, j = get_blank_position(state)
    moves = []
    if is_valid_move(i-1, j):
        moves.append("UP")
    if is_valid_move(i+1, j):
        moves.append("DOWN")
    if is_valid_move(i, j-1):
        moves.append("LEFT")
    if is_valid_move(i, j+1):
        moves.append("RIGHT")
    return moves

def is_valid_move(x, y):
    return 0 <= x < 3 and 0 <= y < 3

def accepted_state(state):
    goal_state = np.array([1,2,3,4,5,6,7,8,0], dtype=int).reshape(3, 3)
    if( np.array_equal(goal_state, state) ):
        return True

    return False


def make_move(state, move):
    i, j = get_blank_position(state)
    new_state = np.copy(state)
    if move == "UP":
        new_state[i][j], new_state[i-1][j] = new_state[i-1][j], new_state[i][j]
    elif move == "DOWN":
        new_state[i][j], new_state[i+1][j] = new_state[i+1][j], new_state[i][j]
    elif move == "LEFT":
        new_state[i][j], new_state[i][j-1] = new_state[i][j-1], new_state[i][j]
    elif move == "RIGHT":
        new_state[i][j], new_state[i][j+1] = new_state[i][j+1], new_state[i][j]
    return new_state


def bfs(initial_state, print_puzzle):
    #nodes created
    created_nodes = []
    # precisa dar um jeito de nao adicionar nós iguais

    count_moves = 0
    #nodes to expand
    tree = []
    root = PuzzleNode(initial_state, None, None, 0)

    tree.append(root)
    created_nodes.append(root)

    while tree:
        current_node = tree.pop(0)
        count_moves = count_moves + 1

        if accepted_state(current_node.state):
            print(count_moves)
            return count_moves

        moves = possible_moves(current_node.state)

        for move in moves:
            new_state = make_move(current_node.state, move)
            node = PuzzleNode(new_state, current_node.state, move, 1)
            tree.append(node)

    return "no solutions"

def get_input():
    n = len(sys.argv)
    algorithm = sys.argv[1]
    initial_state = np.array(sys.argv[2:11], dtype=int).reshape(3, 3)
    if n == 12:
        print_puzzle = True
    else:
        print_puzzle = False
    return algorithm, initial_state, print_puzzle

def define_solving_algorithm(algorithm, initial_state, print_puzzle):
    if algorithm == "B":
        bfs(initial_state, print_puzzle)
        print(algorithm)
    elif algorithm == "I":
        #solve_puzzle(initial_state, print_puzzle)
        print(algorithm)
    elif algorithm == "U":
        #solve_puzzle(initial_state, print_puzzle)
        print(algorithm)
    elif algorithm == "A":
        #solve_puzzle(initial_state, print_puzzle)
        print(algorithm)
    elif algorithm == "G":
        #solve_puzzle(initial_state, print_puzzle)
        print(algorithm)
    elif algorithm == "H":
        #solve_puzzle(initial_state, print_puzzle)
        print(algorithm)
    else:
        print("Invalid algorithm")
        sys.exit(1)

if __name__ == "__main__":
    goal_state = np.array([1,2,3,4,5,6,7,8,0], dtype=int).reshape(3, 3)
    #algorithm,initial_state,print_puzzle = get_input()
    algorithm = "B"
    initial_state = np.array([1,0,3,4,2,5,7,6,8], dtype=int).reshape(3, 3)
    print_puzzle = False

    define_solving_algorithm(algorithm, initial_state, print_puzzle)

