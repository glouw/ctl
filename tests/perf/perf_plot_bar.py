import plotly.graph_objects as go
import sys

if __name__ == '__main__':
    filename = sys.argv[1]
    title = sys.argv[2]
    time_a = sys.argv[3]
    time_b = sys.argv[4]
    size_a = sys.argv[5]
    size_b = sys.argv[6]
    name_a = sys.argv[7]
    name_b = sys.argv[8]
    compiler=[name_a, name_b]
    y = [float(time_a), float(time_b)]
    sizes = [
        str(int(size_a) / 1000) + ' KB',
        str(int(size_b) / 1000) + ' KB',
    ]
    colors = ['crimson', 'darkslateblue']
    fig = go.Figure([go.Bar(x=compiler, y=y, text=sizes, textposition='auto', marker_color=colors)])
    fig.update_yaxes(title_text="Seconds")
    plot_bgcolor = "#F9F9F9"
    paper_bgcolor = "#F6F8FA"
    fig.update_layout(
            title_text=title,
            plot_bgcolor=plot_bgcolor, paper_bgcolor=paper_bgcolor)
    fig.write_image("%s.png" % filename, width=1000, height=300)

