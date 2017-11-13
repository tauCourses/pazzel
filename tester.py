from random import randint
from random import shuffle

sides = ['right', 'left', 'up', 'down']
side_direction = [(0,1),(0,-1),(-1,0),(1,0)]
opposite_size = ['left', 'right', 'down', 'up']


class Puzzle:
    def __init__(self, x, y):
        self.x = x
        self.y = y
        self.size = x*y
        self.pieces = [[None for y in range(y)] for x in range(x)]

    def __iter__(self):
        combinations = [(x,y) for x in range(self.x) for y in range(self.y)]
        for x,y in combinations:
            yield (x,y)


def get_piece_side(puzzle, x, y, side):
    if 0 <= x < puzzle.x and 0 <= y < puzzle.y:
        if puzzle.pieces[x][y] is None:
            return None
        return puzzle.pieces[x][y][side]
    return 0


def set_piece(puzzle, x, y):
    puzzle.pieces[x][y] = dict()
    for i, side in enumerate(sides):
        type = get_piece_side(puzzle, x+side_direction[i][0], y+side_direction[i][1], opposite_size[i])
        if type is None:
            puzzle.pieces[x][y][side] = randint(-1,1)
        else:
            puzzle.pieces[x][y][side] = -1 * type


def set_puzzle():
    puzzle = Puzzle(randint(1, 5), randint(1, 5))
    indexes = [x for x in range(puzzle.size)]
    shuffle(indexes)
    for i, (x, y) in enumerate(puzzle):
        set_piece(puzzle, x, y)
        puzzle.pieces[x][y]["index"] = indexes[i]

    return puzzle


def export_puzzle(puzzle):
    with open("a", "w") as f:
        f.write("NumElements=%d\n" % puzzle.size)
        for x,y in puzzle:
            piece = puzzle.pieces[x][y]
            f.write("%d %d %d %d %d\n" % (piece["index"]+1, piece["left"], piece["up"], piece["right"], piece["down"]))


puzzle = set_puzzle()
export_puzzle(puzzle)

for line in puzzle.pieces:
    for t in line:
        print("%d " % (t["index"]+1), end="")
    print()

