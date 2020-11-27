import string
import sys

import plotly
import plotly.express as px
import plotly.graph_objs as go


def lines_from_file(log_fname):
    with open(log_fname, "r") as f:
        file_raw = f.read()
    file_lines = file_raw.split("\n")
    return file_lines

def toggle_vec_and_vector_in_string(string):
    """If a string contains `vector_`, replace with `vec_`.
       If a string contains `vec_`, replace with `vec_`.
    """
    if "vec_" in string:
        return string.replace("vec_", "vector_")
    elif "vector_" in string:
        return string.replace("vector_", "vec_")
    else:
        return string

def plot_data_from_file(file_lines):
    """Returns a dict of plots"""
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
            name_plot_hash[plot_name]["time"].append(y)
    return name_plot_hash

def plot_from_data(name_plot_hash):

   # variables for the plot
   color_for_c = "#333"
   color_for_cpp = "#888"
   grid_color = "#c9c9c9"
   plot_bgcolor = "#f9f9f9"
   paper_bgcolor = "#f1f1f1"
   color_list = [
       "#120078",
       "#120078",
       "#ec5858",
       "#ec5858",
       "#5c6e91",
       "#5c6e91",
   ]

   used_basenames = []
   trace_list = []
   for name in name_plot_hash.keys():
       name_base = name.split(".")[0]
       alt_name_base = toggle_vec_and_vector_in_string(name_base)
       # ensure the stored name doesn't have `vec_`
       stored_name = name_base.replace("vec_", "vector_")  

       if (name_base not in used_basenames or
           alt_name_base not in used_basenames):
           used_basenames.append(stored_name)

       color_idx = used_basenames.index(stored_name)
       trace_color = color_list[color_idx]

       trace = go.Scatter(
           x=name_plot_hash[name]["num_of_ints"],
           y=name_plot_hash[name]["time"],
           name=name,
           mode="lines",
           marker=dict(color=trace_color),
           line=dict(
               width=2,
               dash=("dashdot" if name.endswith(".cc") else "solid")
           )
       )
       trace_list.append(trace)

   fig = go.Figure(
       data=trace_list
   )

   fig.update_layout(
       plot_bgcolor=plot_bgcolor,
       paper_bgcolor=paper_bgcolor,
       xaxis=dict(
           title="Time",
           nticks=40,
           showgrid=True,
           gridcolor=grid_color
       ),
       yaxis=dict(
           title="Number of Integers",
           showgrid=True,
           gridcolor=grid_color
       ),
       title="C++ vs C Performance",
   )
   return fig

if __name__ == "__main__":
    if len(sys.argv) <= 1:
        raise Warning(
            "You forgot the 2nd argument. Eg. `python plot.py vec.log`"
        )

    filename = sys.argv[1]
    print(f"plotting {filename}...")
    file_lines = lines_from_file(filename)
    name_plot_hash = plot_data_from_file(file_lines)
    fig = plot_from_data(name_plot_hash)
    plotly.offline.plot(fig)
