import pathlib
import argparse
import networkx as nx

BASE_DIR = pathlib.Path(__file__).parents[1]

_DEBUG = False

DEBUG_INPUT_FILE = f"{BASE_DIR}/data/examples/25.txt"
INPUT_FILE = f"{BASE_DIR}/data/25.txt"


def parse_inputs() -> nx.Graph:
    global _DEBUG
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
        _DEBUG = True

    g = nx.Graph()
    with open(infile) as f:
        for line in f.readlines():
            node1, rest = line.strip().split(": ")
            for node2 in rest.split(" "):
                g.add_edge(node1, node2)
                g.add_edge(node2, node1)
    return g


def main() -> int:
    graph = parse_inputs()
    disconnected_nodes = nx.minimum_edge_cut(graph)
    graph.remove_edges_from(disconnected_nodes)
    disconnected_groups = [i for i in nx.connected_components(graph)]
    if _DEBUG:
        print(disconnected_nodes)
        print(disconnected_groups)
    print("-----PART 1-----")
    print(f"Result = {len(disconnected_groups[0])*len(disconnected_groups[1])}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
