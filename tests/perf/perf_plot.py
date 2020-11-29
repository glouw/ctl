import plotly.graph_objs as go
import sys
import string

def lines_from_file(log_fname):
    with open(log_fname, "r") as f:
        file_raw = f.read()
    file_lines = file_raw.split("\n")
    return file_lines

def plot_data_from_file(file_lines):
    name_plot_hash = {}
    fname_base = None
    fname_ext = None
    for line in file_lines:
        if "." in line:
            plot_name = line
            name_plot_hash[plot_name] = {
                "num_of_ints": [],
                "time": []
            }
        elif line != "":
            half_line_len = int(len(line)/2)
            x = int(line[:half_line_len])
            y = int(line[half_line_len:])
            name_plot_hash[plot_name]["num_of_ints"].append(x)
            name_plot_hash[plot_name]["time"].append(y / 1e6)
    return name_plot_hash

def plot_from_data(name_plot_hash, title):
   color_for_c = "#333"
   color_for_cpp = "#888"
   grid_color = "#C9C9C9"
   plot_bgcolor = "#F9F9F9"
   paper_bgcolor = "#F6F8FA"
   color_list = [
       "#0099E5",
       "#0099E5",
       "#A98AEE",
       "#A98AEE",
       "#FF70BF",
       "#FF70BF",
       "#FF766E",
       "#FF766E",
       "#FFA600",
       "#FFA600",
   ]
   used_basenames = []
   trace_list = []
   for idx, name in enumerate(name_plot_hash.keys()):
       trace_color = color_list[idx]
       trace = go.Scatter(
           x=name_plot_hash[name]["num_of_ints"],
           y=name_plot_hash[name]["time"],
           name=name,
           mode="lines",
           marker=dict(color=trace_color),
           line=dict(width=2, dash=("dot" if name.endswith(".cc") else "solid"))
       )
       trace_list.append(trace)
   fig = go.Figure(data=trace_list)
   fig.update_layout(
       plot_bgcolor=plot_bgcolor,
       paper_bgcolor=paper_bgcolor,
       xaxis=dict(title="Size", nticks=40, showgrid=True, gridcolor=grid_color),
       yaxis=dict(title="Seconds", showgrid=True, gridcolor=grid_color),
       title=title
   )
   return fig

if __name__ == "__main__":
    filename = sys.argv[1]
    title = sys.argv[2]
    file_lines = lines_from_file(filename)
    name_plot_hash = plot_data_from_file(file_lines)
    fig = plot_from_data(name_plot_hash, title)
    fig.write_image("%s.png" % filename, width=1000, height=500)
