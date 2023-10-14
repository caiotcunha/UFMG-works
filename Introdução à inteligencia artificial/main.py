import numpy as np
import sys
from collections import deque

class PuzzleNode:
    def __init__(self, state, parent = None, move = None, cost = 0):
        self.state = state
        self.parent = parent
        self.move = move
        self.cost = cost

    def __lt__(self, other):
        return self.cost < other.cost

    def __eq__(self, other):
        return np.array_equal(self.state, other.state)
    
    def __hash__(self):
        return hash(str(self.state))


def print_solution(node,print_puzzle):
    print(node.cost)
    print()
    if print_puzzle:
        steps = []
        while node:
            if node is None:
                break
            steps.insert(0, node)
            node = node.parent
        for step in steps:
            for i in step.state:
                for j in i:
                    print(j,end=" ")
                print()
            print()

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
    return np.array_equal(goal_state, state)

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

def get_neighbors(node):
    neighbors = []
    zero_position = np.argwhere(node.state == 0)[0]

    moves = [(1, 0), (-1, 0), (0, 1), (0, -1)]

    for dr, dc in moves:
        new_position = zero_position + np.array([dr, dc])

        if 0 <= new_position[0] < 3 and 0 <= new_position[1] < 3:
            new_state = np.copy(node.state)
            new_state[zero_position[0], zero_position[1]] = node.state[new_position[0], new_position[1]]
            new_state[new_position[0], new_position[1]] = 0
            neighbors.append(PuzzleNode(new_state, node, f"Move {node.state[new_position[0], new_position[1]]} to ({new_position[0]}, {new_position[1]})", node.cost + 1))

    return neighbors


def bfs(initial_state, print_puzzle):

    initial_node = PuzzleNode(initial_state, None, None, 0)
    tree = deque([initial_node])
    visited = set()

    while tree:
        current_node = tree.popleft()

        if accepted_state(current_node.state):
            print_solution(current_node,print_puzzle)
            break

        if str(current_node.state) in visited:
            continue

        visited.add(str(current_node.state))

        for neighbor in get_neighbors(current_node):
            tree.append(neighbor)

    return

#deep limited search
def dls(initial_state,depth_limit):
    initial_node = PuzzleNode(initial_state, None, None, 0)
    tree = [initial_node]
    visited = set()

    while tree:
        current_node = tree.pop()

        if accepted_state(current_node.state):
            return current_node

        if current_node.cost >= depth_limit:
            continue

        if str(current_node.state) in visited:
            continue

        visited.add(str(current_node.state))

        for neighbor in get_neighbors(current_node):
            tree.append(neighbor)


    return None

def iterative_deepening(initial_state,print_puzzle):
    depth_limit = 0

    while True:
        node = dls(initial_state, depth_limit)
        if node:
            print_solution(node,print_puzzle)
            return
        depth_limit += 2


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
    elif algorithm == "I":
        iterative_deepening(initial_state,print_puzzle)
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
    algorithm,initial_state,print_puzzle = get_input()

    define_solving_algorithm(algorithm, initial_state, print_puzzle)

