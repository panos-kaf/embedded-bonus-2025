import sys
import matplotlib.pyplot as plt
from collections import defaultdict

def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <benchmark_file>")
        sys.exit(1)

    input_file = sys.argv[1]

    # (benchmark, allocator) -> list of (time, memory)
    data = defaultdict(list)

    skip_next = False

    with open(input_file, "r") as f:
        for line in f:
            line = line.strip()

            # If previous line indicated termination, skip this benchmark line
            if skip_next:
                skip_next = False
                continue

            # Detect early termination
            if line.startswith("Command terminated") or line.startswith("Command exited"):
                skip_next = True
                continue

            parts = line.split()
            if len(parts) < 7:
                continue

            benchmark = parts[0]
            allocator = parts[1]

            try:
                time = float(parts[2])
                memory = float(parts[3])
            except ValueError:
                continue

            data[(benchmark, allocator)].append((time, memory))

    if not data:
        print("No valid benchmark data found.")
        sys.exit(1)

    benchmarks = sorted({b for (b, _) in data.keys()})
    allocators = sorted({a for (_, a) in data.keys()})

    avg_time = defaultdict(dict)
    avg_memory = defaultdict(dict)

    for (bench, alloc), values in data.items():
        if not values:
            continue
        avg_time[alloc][bench] = sum(v[0] for v in values) / len(values)
        avg_memory[alloc][bench] = sum(v[1] for v in values) / len(values)

    # Assign one color per allocator
    color_map = dict(zip(allocators, plt.cm.tab10(range(len(allocators)))))

    x = range(len(benchmarks))
    width = 0.8 / len(allocators)

    # -------- Plot: Average Time --------
    plt.figure(figsize=(14, 6))

    for i, alloc in enumerate(allocators):
        times = [
            avg_time[alloc][b]
            for b in benchmarks
            if b in avg_time[alloc]
        ]
        x_pos = [
            p + i * width
            for p, b in zip(x, benchmarks)
            if b in avg_time[alloc]
        ]

        plt.bar(
            x_pos,
            times,
            width=width,
            label=alloc,
            color=color_map[alloc],
        )

    plt.yscale("log")
    plt.ylabel("Average Time (log scale)")
    plt.title("Average Execution Time per Benchmark")
    plt.xticks(
        [p + width * (len(allocators) / 2) for p in x],
        benchmarks,
        rotation=45,
        ha="right",
    )
    plt.legend()
    plt.tight_layout()
#    plt.show()
    plt.savefig('time.svg')

    # -------- Plot: Average Memory --------
    plt.figure(figsize=(14, 6))

    for i, alloc in enumerate(allocators):
        mems = [
            avg_memory[alloc][b]
            for b in benchmarks
            if b in avg_memory[alloc]
        ]
        x_pos = [
            p + i * width
            for p, b in zip(x, benchmarks)
            if b in avg_memory[alloc]
        ]

        plt.bar(
            x_pos,
            mems,
            width=width,
            label=alloc,
            color=color_map[alloc],
        )

    plt.yscale("log")
    plt.ylabel("Average Memory (log scale)")
    plt.title("Average Memory Usage per Benchmark")
    plt.xticks(
        [p + width * (len(allocators) / 2) for p in x],
        benchmarks,
        rotation=45,
        ha="right",
    )
    plt.legend()
    plt.tight_layout()
#   plt.show()
    plt.savefig('memory.svg')


if __name__ == "__main__":
    main()

