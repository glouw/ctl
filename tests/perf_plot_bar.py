import plotly.graph_objects as go
import sys

if __name__ == '__main__':
    filename = sys.argv[1]
    a = sys.argv[2]
    b = sys.argv[3]
    c = sys.argv[4]
    d = sys.argv[5]
    e = sys.argv[6]
    f = sys.argv[7]
    compiler=[e, f]
    y = [float(a), float(b)]
    sizes = [
        str(int(c) / 1000) + ' KB',
        str(int(d) / 1000) + ' KB',
    ]
    colors = ['crimson', 'darkslateblue']
    fig = go.Figure([go.Bar(x=compiler, y=y, text=sizes, textposition='auto', marker_color=colors)])
    fig.update_yaxes(title_text="Seconds")
    plot_bgcolor = "#F9F9F9"
    paper_bgcolor = "#F6F8FA"
    fig.update_layout(
            title_text="CTL vs STL Compile Time (-O3 -march=native)",
            plot_bgcolor=plot_bgcolor, paper_bgcolor=paper_bgcolor)
    fig.write_image("%s.png" % filename, width=1000, height=300)

