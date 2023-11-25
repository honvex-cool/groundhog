import os
import pathlib
import random
import sys

INVALID_USAGE = 1

USAGE = 'usage: python3 generate.py [seed] [path] [z] [n] [d]'


def main():
    if len(sys.argv) != 6:
        sys.stderr.write(USAGE + '\n')
        sys.exit(INVALID_USAGE)
    seed = int(sys.argv[1])
    random.seed(seed)
    path = sys.argv[2]
    z, n, d = map(int, sys.argv[3:])
    generate_file(path, z, n, d)


def generate_file(path, z, n, d):
    tests = '\n'.join(generate_test(n, d) for _ in range(z))
    problem = f'{z}\n{tests}\n'
    pathlib.Path(os.path.dirname(path)).mkdir(parents=True, exist_ok=True)
    with open(path, 'w') as f:
        f.write(problem)


def generate_test(n, d):
    d = max(d, n + 1)
    cuts = set()
    while len(cuts) < n:
        cuts.add(random.randint(1, d - 1))
    ordered_cuts = list(cuts)
    random.shuffle(ordered_cuts)
    return f'{d} {n}\n' + ' '.join(map(str, ordered_cuts))


if __name__ == '__main__':
    main()
