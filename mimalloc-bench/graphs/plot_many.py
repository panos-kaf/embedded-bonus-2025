import re
import sys
import collections

try:
    import numpy as np 
except ImportError:
    print('You need to install numpy.')
    sys.exit(1)
try:
    import plotly.express as px
except ImportError:
    print('You need to install plotly.express.')
    sys.exit(1)
try:
    import kaleido
except ImportError:
    print('You need to install kaleido.')
    sys.exit(1)

if len(sys.argv) != 2:
    print('Usage: %s results.txt' % sys.argv[0])
    print('Where results.txt is the output of the benchmark script.')
    sys.exit(1)

# Regex to parse the CSV lines
parse_line = re.compile('^([^ ]+) +([^ ]+) +([0-9:.]+) +([0-9]+)')
data = []
test_names = set()

print("Processing data...")

# read in the data
with open(sys.argv[1]) as f:
    for l in f.readlines():
        match = parse_line.search(l)
        if not match:
            continue
            
        test_name, alloc_name, time_string, memory = match.groups()
        
        # --- FIX START: robust time parsing ---
        try:
            time_split = time_string.split(':')
            time_taken = 0
            
            if len(time_split) == 2:
                # Handle "0:" case by checking if second part is empty
                if not time_split[1].strip():
                    raise ValueError("Incomplete time string")
                time_taken = int(time_split[0]) * 60 + float(time_split[1])
            else:
                # Handle cases with no seconds provided
                if not time_split[0].strip():
                    raise ValueError("Empty time string")
                time_taken = float(time_split[0])
                
            # Only add the test name if we successfully parsed the time
            test_names.add(test_name)
            data.append({
                "Benchmark": test_name, 
                "Allocator": alloc_name, 
                "Time": time_taken, 
                "Memory": int(memory)
            })
            
        except ValueError:
            # This catches the "0:" error from crashed runs
            print(f"Skipping crashed/failed run: {test_name} {alloc_name} (Time: {time_string})")
            continue
        # --- FIX END ---

if not data:
    print("No valid data found to graph.")
    sys.exit(1)

# create a dictionary of means
time_means = collections.defaultdict(float)
memory_means = collections.defaultdict(float)

for test_name in test_names:
    # calculate the mean
    relevant_times = [d['Time'] for d in data if d['Benchmark'] == test_name]
    relevant_memories = [d['Memory'] for d in data if d['Benchmark'] == test_name]
    
    if relevant_times:
        time_means[test_name] = np.mean(relevant_times)
        memory_means[test_name] = np.mean(relevant_memories)

# add normalised time and memory to each data point
for d in data:
    if time_means[d['Benchmark']] > 0:
        d['Normalised Time'] = d['Time'] / time_means[d['Benchmark']]
    else:
        d['Normalised Time'] = 0
        
    if memory_means[d['Benchmark']] > 0:
        d['Normalised Memory'] = d['Memory'] / memory_means[d['Benchmark']]
    else:
        d['Normalised Memory'] = 0

print("Generating Time Graph...")
# create the graph for time
##fig = px.box(data, x="Benchmark", y="Normalised Time", color="Allocator", log_y=True, points = "all")
#fig = px.strip(data, x="Benchmark", y="Normalised Time", color="Allocator", log_y=True)
#fig.update_xaxes(showgrid=True)
#fig.update_yaxes(title_text="Normalised Time (log(time/mean time))")
#fig.update_layout(boxgroupgap=0.6)
#fig.update_traces(line_width=0.5, marker_line_width=0.5, marker_size=8, marker_symbol='circle')
#fig.write_html("time.html")
#fig.write_image("time.png")
#fig.write_image("time.pdf")


print("Generating Time Graph...")
# We use histogram with histfunc='avg' to create a bar chart of averages
fig = px.histogram(data, x="Benchmark", y="Normalised Time", color="Allocator",
                   barmode="group", histfunc="avg", log_y=True)

fig.update_xaxes(showgrid=True)
fig.update_yaxes(title_text="Average Normalised Time (log scale)")
fig.update_layout(bargap=0.1)

# DELETE or COMMENT OUT the old marker line:
# fig.update_traces(...)

fig.write_html("time.html")
fig.write_image("time.png")
fig.write_image("time.pdf")

print("Generating Memory Graph...")
# create the graph for memory
##fig = px.box(data, x="Benchmark", y="Normalised Memory", color="Allocator", log_y=True, points = "all")
#fig = px.strip(data, x="Benchmark", y="Normalised Memory", color="Allocator", log_y=True)
#fig.update_xaxes(showgrid=True)
#fig.update_yaxes(title_text="Normalised Memory (log(memory/mean memory))")
#fig.update_layout(boxgroupgap=.6)
#fig.update_traces(line_width=0.5, marker_line_width=0.5, marker_size=8, marker_symbol='circle')
#fig.write_html("memory.html")
#fig.write_image("memory.png")
#fig.write_image("memory.pdf")

# --- REPLACE THE MEMORY GRAPH SECTION WITH THIS ---

print("Generating Memory Graph...")
fig = px.histogram(data, x="Benchmark", y="Normalised Memory", color="Allocator",
                   barmode="group", histfunc="avg", log_y=True)

fig.update_xaxes(showgrid=True)
fig.update_yaxes(title_text="Average Normalised Memory (log scale)")
fig.update_layout(bargap=0.1)

# DELETE or COMMENT OUT the old marker line:
# fig.update_traces(...)

fig.write_html("memory.html")
fig.write_image("memory.png")
fig.write_image("memory.pdf")

print("Done! Files saved.")
