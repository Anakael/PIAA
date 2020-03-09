import math
from datetime import datetime
import os
from typing import Dict, Tuple, List


class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def __str__(self):
        return f'{self.x + 1} {self.y + 1}'


class Square:
    def __init__(self, left_corner, length):
        self.left_corner = left_corner
        self.length = length

    def __str__(self):
        return f'{self.left_corner} {self.length}'


def make_fragmentation_for_div_2(n: int) -> List[Square]:
    """
    Make fragmentation for N divided by 2
    """
    squares = []
    half = int(n / 2)
    _range = range(0, n, half)
    for i in _range:
        for j in _range:
            s = Square(Point(j, i), half)
            squares.append(s)

    return squares


def make_fragmentation_for_div_3(n: int) -> List[Square]:
    """
    Make fragmentation for N divided by 3
    """
    squares = []
    small_inner_n = int(n / 3)
    big_inner_n = small_inner_n * 2
    squares.append(Square(Point(0, 0), big_inner_n))
    # Bottom
    for i in range(0, n, small_inner_n):
        s = Square(Point(i, n - small_inner_n), small_inner_n)
        squares.append(s)

    # Side
    for i in range(0, n - small_inner_n, small_inner_n):
        s = Square(Point(n - small_inner_n, i), small_inner_n)
        squares.append(s)

    return squares


def make_fragmentation_for_div_5(n: int) -> List[Square]:
    """
    Make fragmentation for N divided by 5
    """
    squares = []
    half = int(n / 5 * 2)
    third = int(n / 5 * 3)
    fifth = int(n / 5)
    squares.append(Square(Point(0, 0), third))

    # Big in corners
    for a, b in [(0, 1), (1, 0), (1, 1)]:
        squares.append(Square(Point(a * third, b * third), half))

    # Small in gaps
    for i in range(2):
        squares.append(Square(Point(half, third + i * fifth), fifth))
        squares.append(Square(Point(third + i * fifth, half), fifth))

    return squares


def make_base_fragmentation(n: int) -> List[Square]:
    """
    Make base fragmentation for backtracking with 3 big square
    """
    half = math.ceil(n / 2)
    return [Square(Point(0, 0), half),
            Square(Point(half, 0), half - 1),
            Square(Point(0, half), half - 1)]


def make_backtracking_fragmentation(n: int) -> List[Square]:
    """
    Make backtracking fragmentation
    """
    squares = make_base_fragmentation(n)
    min_squares = []
    center = int(n / 2)
    for diag_i in range(2, int(n / 2) - 1):
        left_corner_shit = n - diag_i
        best_fragmentation_for_diag = [Square(Point(left_corner_shit, left_corner_shit), diag_i)]

        best_fragmentation_for_diag.extend(find_next_fragmentation_for_diag(n, center, left_corner_shit))

        if not min_squares or len(best_fragmentation_for_diag) < len(min_squares):
            min_squares = list(best_fragmentation_for_diag)

    squares.extend(min_squares)
    return squares


def find_next_fragmentation_for_diag(n: int, center: int, left_corner_shift: int) -> List[Square]:
    def squize_square(square: Square) -> None:
        # Bottom
        for i in range(square.left_corner.x + square.length - 1, square.left_corner.x - 1, -1):
            field[(i, square.left_corner.y + square.length - 1)] = 0

        # Side
        for j in range(square.left_corner.y + square.length - 2, square.left_corner.y - 1, -1):
            field[(square.left_corner.x + square.length - 1, j)] = 0
        square.length -= 1

    field = {}
    for i in range(center, n):
        for j in range(center, n):
            is_empty = (i != j or (i != center and j != center)) and (i < left_corner_shift or j < left_corner_shift)
            field[(i, j)] = 0 if is_empty else 1

    ret_stack = []
    tmp_stack = []

    while tmp_stack or not ret_stack:
        if not ret_stack or not tmp_stack or len(tmp_stack) < len(ret_stack):
            backtracking_iter(n, center, field, tmp_stack)
            if len(tmp_stack) < len(ret_stack) or len(ret_stack) == 0:
                ret_stack = [Square(Point(x.left_corner.x, x.left_corner.y), x.length) for x in tmp_stack]

        while tmp_stack and tmp_stack[len(tmp_stack) - 1].length == 1:
            deleted = tmp_stack[len(tmp_stack) - 1]
            field[(deleted.left_corner.x, deleted.left_corner.y)] = 0
            tmp_stack.pop()

        if tmp_stack and tmp_stack[len(tmp_stack) - 1].length > 1:
            squize_square(tmp_stack[len(tmp_stack) - 1])

    return ret_stack


def backtracking_iter(n: int, center: int, field: Dict[Tuple[int, int], int], current_stack: List[Square]):
    def print_field():
        print('-' * 20)
        for i in range(center, n):
            for j in range(center, n):
                print(field[(i, j)], end=' ')
            print()

    def can_growth(square: Square) -> bool:
        # Check bottom
        if square.left_corner.x + square.length + 1 <= n and square.left_corner.y + square.length + 1 <= n:
            for i in range(square.left_corner.x, square.left_corner.x + square.length + 1):
                t = square.left_corner.y + square.length
                if field[(i, square.left_corner.y + square.length)] != 0:
                    return False
        else:
            return False
        # Check side
        for i in range(square.left_corner.y, square.left_corner.y + square.length + 1):
            if field[(square.left_corner.x + square.length, i)] != 0:
                return False

        return True

    def growth_square(square: Square) -> None:
        square.length += 1
        for i in range(square.left_corner.x, square.left_corner.x + square.length):
            for j in range(square.left_corner.y, square.left_corner.y + square.length):
                field[(i, j)] = 1

    for i in range(center, n):
        for j in range(center, n):
            if field[(i, j)] == 0:
                tmp_square = Square(Point(i, j), 1)
                field[(i, j)] = 1
                # print_field()
                while can_growth(tmp_square):
                    growth_square(tmp_square)
                    # print_field()

                current_stack.append(tmp_square)


def print_squares(n: int, squares: List[Square]):
    print(len(squares))
    for square in squares:
        print(square)

    if os.environ.get('DEBUG', False):
        pass
        # print("=" * 20)
        # print_debug_squares(n, squares)


def print_debug_squares(n: int, squares: List[Square]):
    field = []
    for i in range(n):
        field.append([])
        for j in range(n):
            field[i].append([])

    for i, square in enumerate(squares):
        for y in range(square.left_corner.y, square.left_corner.y + square.length):
            for x in range(square.left_corner.x, square.left_corner.x + square.length):
                field[y][x].append(str(i))

    for i in range(n):
        for j in range(n):
            print('/'.join(field[i][j]) if field[i][j] else ' ', end=' ')
        print()


def main(n: int) -> None:
    """
    Main function for call
    """
    fun = make_backtracking_fragmentation
    if n % 2 == 0:
        fun = make_fragmentation_for_div_2
    elif n % 3 == 0:
        fun = make_fragmentation_for_div_3
    elif n % 5 == 0:
        fun = make_fragmentation_for_div_5
    else:
        fun = make_backtracking_fragmentation

    res = fun(n)
    print_squares(n, res)


if __name__ == "__main__":
    _n = int(input())
    start = datetime.now()
    main(_n)
    print(f'{datetime.now() - start} ms')
