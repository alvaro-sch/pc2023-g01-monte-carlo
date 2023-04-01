import sys
import subprocess

CCS = ["gcc", "clang", "icc"]
DEFS = "-DNDEBUG -DRAND_XOSHIROF"

# common
CFLAGS = "-O2 -march=native -ffast-math -funroll-loops"
LDFLAGS = ""

# exclusive
EXC_CFLAGS = {
    "gcc": "",
    "clang": "-flto=thin",
    "icc": "",
}
EXC_LDFLAGS = {
    "gcc": "",
    "clang": "-fuse-ld=lld",
    "icc": "",
}

def make():
    for cc in CCS:
        subprocess.run([
            'make',
            f'CC={cc}',
            f'DEFS={DEFS}',
            f'EXT_CFLAGS={CFLAGS} {EXC_CFLAGS[cc]}',
            f'EXT_LDFLAGS={LDFLAGS} {EXC_LDFLAGS[cc]}',
        ])

DEFAULT_PARAMS = ["101", "32768", "2.0", "20.0", "50", "0"]

def bench():
    run = lambda prog: float(subprocess.run(
        [f'./{prog}.out'] + DEFAULT_PARAMS,
        capture_output=True
    ).stdout.split(b'\n')[0])

    avg = lambda f, x, n: sum(f(x) for i in range(n)) / n

    averages = dict((cc, avg(run, cc, 10) * 1.3) for cc in CCS[:2])

    for cc, time in averages.items():
        print(f"{cc}, {time}")

def main(argv):
    if len(argv) < 2:
        print(f"usage: {argv[0]} <make | bench>")

    match argv[1]:
        case "make":
            make()

        case "bench":
            bench()

if __name__ == "__main__":
    main(sys.argv)
