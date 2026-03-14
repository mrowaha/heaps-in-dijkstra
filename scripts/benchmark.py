import subprocess
import os
import re
import json
import glob

EXECUTABLE = "./dijkstrahw1"
GRAPHS_DIR = "./mm/generated"
HEAP_TYPES = ["binary", "binomial", "fibonacci"]

# Pattern: {density}_graph{size}.mtx
GRAPH_PATTERN = re.compile(r"^(sparse|mediumdense|dense)_graph(\d+)\.mtx$")
TIME_PATTERN = re.compile(r"Dijkstra elapsed time:\s+([\d.]+) seconds")

results = {}  # heap_type -> density -> size -> time

mtx_files = sorted(glob.glob(os.path.join(GRAPHS_DIR, "*.mtx")))

for heap in HEAP_TYPES:
    results[heap] = {}
    for mtx_path in mtx_files:
        filename = os.path.basename(mtx_path)
        m = GRAPH_PATTERN.match(filename)
        if not m:
            print(f"Skipping unrecognized file: {filename}")
            continue
        density, size = m.group(1), m.group(2)

        print(f"Running {heap} heap on {filename}...")
        proc = subprocess.run(
            [EXECUTABLE, mtx_path, heap],
            capture_output=True,
            text=True
        )
        output = proc.stdout + proc.stderr

        match = TIME_PATTERN.search(output)
        if not match:
            print(f"  WARNING: could not parse time from output:\n{output}")
            continue

        elapsed = float(match.group(1))
        print(f"  {elapsed:.6f} seconds")

        results[heap].setdefault(density, {})[size] = elapsed

json_output = [
    {"type": heap, "results": results[heap]}
    for heap in HEAP_TYPES
]

output_path = "./times.json"
with open(output_path, "w") as f:
    json.dump(json_output, f, indent=2)

print(f"\nResults written to {output_path}")
