import math
import os
import sys
import subprocess

LABEL = os.getenv("LABEL") or "unnamed"
PHOTON_COUNT = os.getenv("PHOTON_COUNT") or "32768"

CCS = ["gcc", "clang", "icx", "icc"]
DEFS = "-DNDEBUG -DRAND_XOSHIROF"

# common
CFLAGS = "-O2 -march=native -ffast-math -funroll-loops"
LDFLAGS = ""

# exclusive
EXC_CFLAGS = {
    "gcc": "",
    "clang": "-flto=thin",
    "icx": "-flto=thin",
    "icc": "-diag-disable=10441",
}
EXC_LDFLAGS = {
    "gcc": "",
    "clang": "-fuse-ld=lld",
    "icx": "-fuse-ld=lld",
    "icc": "-diag-disable=10441",
}

def make():
    for cc in CCS:
        subprocess.run([
            'make',
            'headless',
            f'CC={cc}',
            f'DEFS={DEFS}',
            f'EXT_CFLAGS={CFLAGS} {EXC_CFLAGS[cc]}',
            f'EXT_LDFLAGS={LDFLAGS} {EXC_LDFLAGS[cc]}',
        ])

DEFAULT_PARAMS = [PHOTON_COUNT, "0"]

def bench():
    run = lambda prog: float(subprocess.run(
        [f'./{prog}_headless.out'] + DEFAULT_PARAMS,
        capture_output=True
    ).stdout.split(b'\n')[0])

    avg = lambda xs: sum(x for x in xs) / len(xs)
    var = lambda xs, mean: sum((x - mean)**2 for x in xs) / len(xs)

    for cc in CCS:
        samples = list(run(cc) for _ in range(100))

        mean_time = avg(samples)
        mean_photon_time = mean_time / float(PHOTON_COUNT)
        std_dev = math.sqrt(var(samples, mean_time))

        print(f"{LABEL},{cc},{mean_time},{mean_photon_time},{std_dev}")

def main(argv):
    if len(argv) < 2:
        print(f"usage: {argv[0]} <make | bench>")
        exit(1)

    match argv[1]:
        case "make":
            make()

        case "bench":
            bench()

if __name__ == "__main__":
    main(sys.argv)
