import sys
import matplotlib.pyplot as plt
from collections import defaultdict

def parse_time(t):
    """
    Parse time in formats:
      M:SS
      M:SS.xx
      H:MM:SS
    Returns seconds (float) or None if invalid.
    """
    parts = t.split(":")
    try:
        if len(parts) == 2:
            m, s = parts
            return int(m) * 60 + float(s)
        elif len(parts) == 3:
            h, m, s = parts
            return int(h) * 3600 + int(m) * 60 + float(s)
    except ValueError:
        return None
    return None


def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <benchmark_file>")
        sys.exit(1)

    input_file = sys.argv[1]

    # (benchmark, allocator) -> list of (time_sec, memory)
    data = defaultdict(list)

    skip_next = False

    with open(input_file, "r") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue

            # Skip benchmark result following a crash/termination line
            if skip_next:
                skip_next = False
                continue

            if line.startswith("Command terminated") or line.startswith("Command exited"):
                skip_next = True
                continue

            parts = line.split()
            if len(parts) < 4:
                continue

            benchmark = parts[0]
            allocator = parts[1]

            time_sec = parse_time(parts[2])
            try:
                memory = float(parts[3])
            except ValueError:
                continue

            if time_sec is None:
                continue

            data[(benchmark, allocator)].append((time_sec, memory))

    if not data:
        print("No valid benchmark data found.")
        sys.exit(1)

    benchmarks = sorted({b for (b, _) in data.keys()})
    allocators = sorted({a for (_, a) in data.keys()})

    avg_time = defaultdict(dict)
    avg_memory = defaultdict(dict)

    for (bench, alloc), values in data.items():
        avg_time[alloc][bench] = sum(v[0] for v in values) / len(values)
        avg_memory[alloc][bench] = sum(v[1] for v in values) / len(values)

    # One color per allocator
    color_map = dict(zip(allocators, plt.cm.tab10(range(len(allocators)))))

    x = range(len(benchmarks))
    width = 0.8 / len(allocators)

    # ---------- Average Time ----------
    plt.figure(figsize=(18, 6))

    for i, alloc in enumerate(allocators):
        times = []
        x_pos = []

        for p, b in zip(x, benchmarks):
            if b in avg_time[alloc]:
                times.append(avg_time[alloc][b])
                x_pos.append(p + i * width)

        plt.bar(x_pos, times, width=width, label=alloc, color=color_map[alloc])

    plt.yscale("log")
    plt.ylabel("Average Time (seconds, log scale)")
    plt.title("Average Execution Time per Benchmark")
    plt.xticks(
        [p + width * (len(allocators) / 2) for p in x],
        benchmarks,
        rotation=45,
        ha="right",
    )
    plt.legend(
        loc="center left",
        bbox_to_anchor=(1.02, 0.5),
        borderaxespad=0.0,
    )
    plt.tight_layout(rect=[0, 0, 0.85, 1])

    plt.savefig("time.svg")

    # ---------- Average Memory ----------
    plt.figure(figsize=(18, 6))

    for i, alloc in enumerate(allocators):
        mems = []
        x_pos = []

        for p, b in zip(x, benchmarks):
            if b in avg_memory[alloc]:
                mems.append(avg_memory[alloc][b])
                x_pos.append(p + i * width)

        plt.bar(x_pos, mems, width=width, label=alloc, color=color_map[alloc])

    plt.yscale("log")
    plt.ylabel("Average Memory (KB, log scale)")
    plt.title("Average Memory Usage per Benchmark")
    plt.xticks(
        [p + width * (len(allocators) / 2) for p in x],
        benchmarks,
        rotation=45,
        ha="right",
    )
    plt.legend(
        loc="center left",
        bbox_to_anchor=(1.02, 0.5),
        borderaxespad=0.0,
    )
    plt.tight_layout(rect=[0, 0, 0.85, 1])

    plt.savefig("memory.svg")


if __name__ == "__main__":
    main()

