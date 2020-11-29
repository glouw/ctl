CFLAGS='-O3 -march=native'

function perf_graph
{
    LOG=$1
    TITLE=$2
    LIST=$3
    OUT=bin
    echo $LOG
    for TEST in ${LIST[*]}
    do
        if [[ $TEST == *.c ]]
        then
            gcc -o $OUT $CFLAGS $TEST -I ctl
        else
            g++ -o $OUT $CFLAGS $TEST
        fi
        ./$OUT >> $LOG
    done
    python3 tests/perf_plot.py $LOG "$TITLE"
    mv $LOG.png images/
    rm $LOG
    rm $OUT
}

function perf_compile_two_bar
{
    KEY='stamp'
    TIMEFORMAT="$KEY %R"
    LOG=$1
    A=$2
    B=$3
    AA=bina
    BB=binb
    echo $LOG
    X=`(time gcc -o $AA $CFLAGS $A -I ctl) 2>&1 | grep $KEY | cut -d ' ' -f 2`
    Y=`(time g++ -o $BB $CFLAGS $B)        2>&1 | grep $KEY | cut -d ' ' -f 2`
    I=`stat --printf="%s" $AA`
    J=`stat --printf="%s" $BB`
    python3 tests/perf_plot_bar.py $LOG $X $Y $I $J $A $B
    mv $LOG.png images/
    rm $AA
    rm $BB
}

perf_graph 'vec.log' "std::vector<int> vs. CTL vec_int ($CFLAGS)" 'tests/perf_vector_push_back.cc tests/perf_vec_push_back.c tests/perf_vector_pop_back.cc tests/perf_vec_pop_back.c tests/perf_vector_sort.cc tests/perf_vec_sort.c'
perf_graph 'lst.log' "std::list<int> vs. CTL lst_int ($CFLAGS)"   'tests/perf_list_push_back.cc tests/perf_lst_push_back.c tests/perf_list_pop_back.cc tests/perf_lst_pop_back.c tests/perf_list_pop_front.cc tests/perf_lst_pop_front.c tests/perf_list_push_front.cc tests/perf_lst_push_front.c tests/perf_list_sort.cc tests/perf_lst_sort.c'
perf_compile_two_bar 'compile.log' 'tests/perf_compile_c99.c' 'tests/perf_compile_cc.cc'
