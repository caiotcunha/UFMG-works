import numpy as np
import sys
from collections import deque
import heapq

class PuzzleNode:
    def __init__(self, state, parent = None, move = None,cost = 0, depth = 0,heuristic = 0):
        self.state = state
        self.parent = parent
        self.move = move
        self.depth = depth
        self.cost = cost
        self.heuristic = heuristic

    def __lt__(self, other):
        return self.cost < other.cost

    def __eq__(self, other):
        return np.array_equal(self.state, other.state)
    
    def __hash__(self):
        return hash(str(self.state))

def is_cycle(node):
    p = node.parent
    while p:
        if p == node:
            return True
        p = p.parent
    return False

def print_solution(node,print_puzzle):
    # print(node.cost)
    # print()
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

def print_solution_greed(node,print_puzzle):
    # print(node.depth)
    # print()
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


def bfs(initial_state, print_puzzle = 0):
    
    number_of_expanded_nodes = 0
    initial_node = PuzzleNode(initial_state, None, None, 0,0)
    tree = deque([initial_node])
    visited = set()

    while tree:
        current_node = tree.popleft()

        if str(current_node.state) in visited:
            continue

        if accepted_state(current_node.state):
            print_solution(current_node,print_puzzle)
            return number_of_expanded_nodes,1

        number_of_expanded_nodes = number_of_expanded_nodes + 1

        visited.add(str(current_node.state))

        moves = possible_moves(current_node.state)
        for move in moves:
            new_state = make_move(current_node.state, move)
            new_node = PuzzleNode(new_state, current_node, move, current_node.depth + 1,current_node.depth + 1)
            tree.append(new_node)
    #retorna 0 se a solução não foi encontrada
    return number_of_expanded_nodes,0

#deep limited search
def dls(initial_state,depth_limit,    number_of_expanded_nodes):
    initial_node = PuzzleNode(initial_state, None, None, 0,0)
    tree = [initial_node]

    while tree:
        current_node = tree.pop()

        if accepted_state(current_node.state):
            return current_node, number_of_expanded_nodes,1
        
        if current_node.depth >= depth_limit:
            continue
        
        number_of_expanded_nodes = number_of_expanded_nodes + 1
        if not is_cycle(current_node):
            moves = possible_moves(current_node.state)
            for move in moves:
                new_state = make_move(current_node.state, move)
                new_node = PuzzleNode(new_state, current_node, move, current_node.depth + 1,current_node.depth + 1)
                tree.append(new_node)


    return None,number_of_expanded_nodes,0

def iterative_deepening(initial_state,print_puzzle = 0):
    depth_limit = 0
    number_of_expanded_nodes = 0

    while True:
        node,number,solution = dls(initial_state, depth_limit,number_of_expanded_nodes)
        number_of_expanded_nodes = number_of_expanded_nodes + number
        if node:
            print_solution(node,print_puzzle)
            return number_of_expanded_nodes,1
        if depth_limit > 31:
            return number_of_expanded_nodes,0
        depth_limit += 1

#uniform cost search
def ucs(initial_state, print_puzzle = 0):
    number_of_expanded_nodes = 0
    initial_node = PuzzleNode(initial_state, None, None, 0)
    tree = []
    heapq.heapify(tree)
    heapq.heappush(tree,initial_node)
    visited = set()

    while tree:
        
        current_node = heapq.heappop(tree)

        if accepted_state(current_node.state):
            print_solution(current_node,print_puzzle)
            return number_of_expanded_nodes,1
        
        visited.add(str(current_node.state))
        
        number_of_expanded_nodes = number_of_expanded_nodes + 1

        moves = possible_moves(current_node.state)
        for move in moves:
            in_tree = False
            new_state = make_move(current_node.state, move)
            new_node = PuzzleNode(new_state, current_node, move, current_node.depth + 1,current_node.cost + 1)
            for i, node in enumerate(tree):
                if np.array_equal(node.state, new_node.state):
                    in_tree = True
                    if node.cost > new_node.cost:
                        tree.pop(i)
                        heapq.heappush(tree, new_node)
                    break
            if str(new_node.state) not in visited and not in_tree:
                heapq.heappush(tree,new_node)
                
#heuristic funcction to A* search and hillClimbing
def manhattan_distance(state):
    goal_state = (1, 2, 3, 4, 5, 6, 7, 8, 0)
    state = state.reshape(9)
    distance = 0
    for i in range(9):
        if state[i] == 0:
            continue
        gx, gy = divmod(goal_state.index(state[i]), 3)
        x, y = divmod(i, 3)
        distance += abs(gx - x) + abs(gy - y)
    return distance

def A_search(initial_state, print_puzzle = 0):

    number_of_expanded_nodes = 0
    initial_node = PuzzleNode(initial_state, None, None, 0)
    tree = []
    heapq.heapify(tree)
    heapq.heappush(tree,initial_node)
    visited = set()

    while tree:
        current_node = heapq.heappop(tree)

        if accepted_state(current_node.state):
            print_solution(current_node,print_puzzle)
            return number_of_expanded_nodes,1

        if str(current_node.state) in visited:
            continue

        number_of_expanded_nodes = number_of_expanded_nodes + 1
        visited.add(str(current_node.state))

        moves = possible_moves(current_node.state)
        for move in moves:
            new_state = make_move(current_node.state, move)

            #calculating depth considering the heuristic function
            heuristic = manhattan_distance(new_state)
            depth = current_node.depth + 1
            cost = depth + heuristic
            new_node = PuzzleNode(new_state, current_node, move,cost, depth,heuristic)
            if str(new_node.state) not in visited:
                heapq.heappush(tree,new_node)
            elif new_node in tree:
                for node in tree:
                    if np.array_equal(node.state, new_node.state) and node.cost > new_node.cost:
                        tree.remove(node)
                        heapq.heappush(tree,new_node)
                        break

    return number_of_expanded_nodes,0

# Heuristic funcction to greedy search
def wrong_pieces(state):
    goal_state = [1, 2, 3, 4, 5, 6, 7, 8, 0]
    state = state.reshape(9)
    contador = 0
    for i in range(9):
        if state[i] != goal_state[i]:
            contador = contador + 1
    return contador

def greedy_search(initial_state, print_puzzle = 0):

    number_of_expanded_nodes = 0
    initial_node = PuzzleNode(initial_state, None, None, 0)
    tree = []
    heapq.heapify(tree)
    heapq.heappush(tree,initial_node)
    visited = set()

    while tree:
        current_node = heapq.heappop(tree)

        if str(current_node.state) in visited:
            continue

        if accepted_state(current_node.state):
            print_solution_greed(current_node,print_puzzle)
            return number_of_expanded_nodes,1

        number_of_expanded_nodes = number_of_expanded_nodes + 1
        visited.add(str(current_node.state))

        moves = possible_moves(current_node.state)
        for move in moves:
            new_state = make_move(current_node.state, move)

            #calculating depth considering the heuristic function
            heuristic = wrong_pieces(new_state)
            depth = current_node.depth + 1
            cost = heuristic
            new_node = PuzzleNode(new_state, current_node, move,cost, depth,heuristic)
            if new_node not in tree:
                heapq.heappush(tree,new_node)
            elif new_node in tree:
                for node in tree:
                    if np.array_equal(node.state, new_node.state) and node.cost > new_node.cost:
                        tree.remove(node)
                        heapq.heappush(tree,new_node)
                        break

    return number_of_expanded_nodes,0

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
        ucs(initial_state, print_puzzle)
    elif algorithm == "A":
        A_search(initial_state, print_puzzle)
    elif algorithm == "G":
        greedy_search(initial_state, print_puzzle)
    elif algorithm == "H":
        print("Hill Climbing")
    else:
        print("Invalid algorithm")
        sys.exit(1)

if __name__ == "__main__":
    goal_state = np.array([1,2,3,4,5,6,7,8,0], dtype=int).reshape(3, 3)
    algorithm,initial_state,print_puzzle = get_input()

    define_solving_algorithm(algorithm, initial_state, print_puzzle)

