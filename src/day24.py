import pathlib
import argparse
import sympy

BASE_DIR = pathlib.Path(__file__).parents[1]

DEBUG_INPUT_FILE = f"{BASE_DIR}/data/examples/24.txt"
INPUT_FILE = f"{BASE_DIR}/data/24.txt"


class Line:
    x0: tuple[int, int, int]
    v:  tuple[int, int, int]

    def __init__(self, x0, y0, z0, vx, vy, vz) -> None:
        self.x0 = (x0, y0, z0)
        self.v = (vx, vy, vz)


    def __repr__(self) -> str:
        return f"{__class__.__name__}{{x0={self.x0}, v={self.v}}}"


def parse_inputs() -> list[Line]:
    parser = argparse.ArgumentParser()
    parser.add_argument("debug", nargs="?", default="RELEASE", type=str
                    , help="Debug run (\"DEBUG\" or \"RELEASE\")")
    args = parser.parse_args()

    if (args.debug.lower() not in ["debug", "release"]):
        print(f"Unexpected argument \"{args.debug}\". Expected \"DEBUG\" or \"RELEASE\".")
        return 1

    infile = INPUT_FILE
    if args.debug.lower() == "debug":
        infile = DEBUG_INPUT_FILE

    with open(infile) as f:
        return [Line(*map(int, line.replace("@", ",").split(","))) for line in f.readlines()]


def main() -> int:
    lines = parse_inputs()

    xr, yr, zr, vrx, vry, vrz = sympy.symbols("xr, yr, zr, vrx, vry, vrz")
    eqs = []
    for i, l in enumerate(lines[:4]):
        ti = sympy.Symbol(f"t{i}")
        eqs.append(xr + (vrx - l.v[0]) * ti - l.x0[0])
        eqs.append(yr + (vry - l.v[1]) * ti - l.x0[1])
        eqs.append(zr + (vrz - l.v[2]) * ti - l.x0[2])
    soln = sympy.solve(eqs)
    print(soln)

    res = soln[0][xr] + soln[0][yr] + soln[0][zr]

    print(f"Result = {res}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
