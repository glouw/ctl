import plotly.graph_objects as go
import sys

if __name__ == '__main__':
    filename = sys.argv[1]
    a = sys.argv[2]
    b = sys.argv[3]
    compiler=['gcc -O3 -march=native perf_compile_c99.c', 'g++ -O3 -march=native perf_compile_cc.cc']
    y = [float(a), float(b)]
    colors = ['crimson', 'darkslateblue']
    fig = go.Figure([go.Bar(x=compiler, y=y, text=y, textposition='auto', marker_color=colors)])
    fig.update_yaxes(title_text="Seconds")
    plot_bgcolor = "#F9F9F9"
    paper_bgcolor = "#F6F8FA"
    fig.update_layout(
            title_text="CTL vs STL Compile Times, T = int, All Containers",
            plot_bgcolor=plot_bgcolor, paper_bgcolor=paper_bgcolor)
    fig.write_image("%s.png" % filename, width=1000, height=300)

