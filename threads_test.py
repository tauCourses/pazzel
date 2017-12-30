from random import randint
from random import shuffle
from subprocess import Popen, PIPE
import timeit
import statistics
import sys
import matplotlib as mpl

sides = ['right', 'left', 'up', 'down']
side_direction = [(0, 1), (0, -1), (-1, 0), (1, 0)]
opposite_side = ['left', 'right', 'down', 'up']

rotation_sides = ['left', 'up', 'right', 'down']


def shift_array(key, array):
    return array[-key:] + array[:-key]


class Puzzle:
    def __init__(self, x, y):
        self.x = x
        self.y = y
        self.size = x * y
        self.pieces = [[None for y in range(y)] for x in range(x)]

    def __iter__(self):
        combinations = [(x, y) for x in range(self.x) for y in range(self.y)]
        for x, y in combinations:
            yield (x, y)

    def get_piece_by_index(self, index):
        return [self.pieces[x][y] for x, y in self if self.pieces[x][y]["index"] == index - 1][0]


def get_piece_side(puzzle, x, y, side):
    if 0 <= x < puzzle.x and 0 <= y < puzzle.y:
        if puzzle.pieces[x][y] is None:
            return None
        return puzzle.pieces[x][y][side]
    return 0


def set_piece(puzzle, x, y):
    puzzle.pieces[x][y] = {"x": x, "y": y}
    for i, side in enumerate(sides):
        type = get_piece_side(puzzle, x + side_direction[i][0], y + side_direction[i][1], opposite_side[i])
        if type is None:
            puzzle.pieces[x][y][side] = randint(-1, 1)
        else:
            puzzle.pieces[x][y][side] = -1 * type


def set_puzzle(puzzle_size):
    puzzle = Puzzle(puzzle_size[0], puzzle_size[1])
    indexes = [x for x in range(puzzle.size)]
    shuffle(indexes)
    for i, (x, y) in enumerate(puzzle):
        set_piece(puzzle, x, y)
        puzzle.pieces[x][y]["index"] = indexes[i]

    return puzzle


def export_puzzle(puzzle, file_name):
    with open(file_name, "w") as f:
        f.write("NumElements=%d\n" % puzzle.size)
        for x, y in puzzle:
            piece = puzzle.pieces[x][y]
            f.write(
                "%d %d %d %d %d\n" % (piece["index"] + 1, piece["left"], piece["up"], piece["right"], piece["down"]))


def read_puzzle_and_rotate_pieces_if_needed(puzzle):
    new_puzzle = []
    with open("b") as f:
        for line in f:
            line = line.split()  # to deal with blank
            if line:  # lines (ie skip them)
                puzzle_line = []
                for i in line:
                    if "[" in i:
                        rotate_by = 4 - int(int(i[1:-1]) / 90)
                        for x, y in puzzle:
                            piece = puzzle.pieces[x][y]
                            if piece["index"] + 1 == puzzle_line[-1]:
                                new_piece = {"index": piece["index"]}
                                new_sides = shift_array(rotate_by, rotation_sides)
                                for side, new_side in zip(rotation_sides, new_sides):
                                    new_piece[new_side] = piece[side]
                                puzzle.pieces[x][y] = new_piece
                    else:
                        puzzle_line.append(int(i))
                new_puzzle.append(puzzle_line)
    return new_puzzle


def check_all_pieces_exists(puzzle, new_puzzle):
    flattened = [val for sublist in new_puzzle for val in sublist]
    flattened.sort()
    for i in range(puzzle.size):
        assert (flattened[i] == i + 1)


def check_output_file(puzzle):
    new_puzzle = read_puzzle_and_rotate_pieces_if_needed(puzzle)
    check_all_pieces_exists(puzzle, new_puzzle)

    for x in range(len(new_puzzle)):
        for y in range(len(new_puzzle[x])):
            piece = puzzle.get_piece_by_index(new_puzzle[x][y])
            for i, side in enumerate(sides):
                new_x = x + side_direction[i][0]
                new_y = y + side_direction[i][1]
                if 0 <= new_x < len(new_puzzle) and 0 <= new_y < len(new_puzzle[x]):
                    side_piece = puzzle.get_piece_by_index(new_puzzle[new_x][new_y])
                    if -1 * piece[side] != side_piece[opposite_side[i]]:
                        print("failed!1")
                        return
                elif piece[side] != 0:
                    print("failed!2")
                    return


p1 = Popen(["make"], stdin=PIPE, stdout=PIPE, stderr=PIPE)
p1.wait()
print("start:")


def _run_single(puzzle_size, rotate, threads):
    puzzle = set_puzzle(puzzle_size)
    if rotate:
        _rotate_puzzle(puzzle)
    export_puzzle(puzzle, 'a')
    args = ['./ex3', 'a', 'b', '-threads', str(threads)]
    if rotate:
        args.append('-rotate')
    p = Popen(args, stdin=PIPE, stdout=PIPE, stderr=PIPE)
    run_time = timeit.timeit(p.communicate, number=1)
    check_output_file(puzzle)
    return puzzle, run_time


def _rotate_puzzle(puzzle):
    for x, y in puzzle:
        piece = puzzle.pieces[x][y]
        new_piece = {"index": piece["index"]}
        rotate_by = randint(0, 3)
        new_sides = shift_array(rotate_by, rotation_sides)
        for side, new_side in zip(rotation_sides, new_sides):
            new_piece[new_side] = piece[side]
        puzzle.pieces[x][y] = new_piece


def _print_puzzle(puzzle):
    for x, y in puzzle:
        piece = puzzle.pieces[x][y]
        print("%d %d %d %d %d" % (piece["index"] + 1, piece["left"], piece["up"], piece["right"], piece["down"]))


total = 100
puzzle_size = (8,4)
rotate = False
threads_sizes = [1,2,4]
save_to_file = True
timeout = 60
for arg in sys.argv:
    if 'runs=' in arg:
        total = int(arg.split('=')[1])
    if 'timeout=' in arg:
        timeout = float(arg.split('=')[1])
    if 'size=' in arg:
        sizes = arg.split('=')[1].split(',')
        puzzle_size = (int(sizes[0]), int(sizes[1]))
    if 'threads=' in arg:
        threads_sizes = [int(thread) for thread in arg.split('=')[1].split(',')]
    if '-rotate' in arg:
        rotate = True
    if '-show' in arg:
        save_to_file = False

if rotate:
    if puzzle_size[0] < puzzle_size[1]:
        puzzle_size = (puzzle_size[1], puzzle_size[0])

run_times = []
puzzles = []

threads_lists = [[] for thread_size in threads_sizes]
for x in range(10):
    for y in range(int(total / 10)):
        for threads, threads_list in zip(threads_sizes, threads_lists):
            puzzle, run_time = _run_single(puzzle_size, rotate, threads)
            threads_list.append(min(timeout,run_time))
            print('%f,' % run_time, end='')
        print('')

if save_to_file:
    mpl.use('Agg')

import matplotlib.pyplot as plt

fig = plt.figure()
ax = fig.add_subplot(111)
runs = [i for i in range(total)]
for threads, threads_list in zip(threads_sizes, threads_lists):
    print("for %d threads the average run time is %f and std is %f" % (threads,  statistics.mean(threads_list), statistics.stdev(threads_list)))
    ax.plot(runs, threads_list, label=str(threads))

plt.xlabel('runs', fontsize=18)
plt.ylabel('time', fontsize=16)
plt.legend()
if save_to_file:
    fig.savefig('threads.png')
else:
    plt.show()
