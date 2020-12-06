#include <deque>
#include <list>
#include <queue>
#include <set>
#include <stack>
#include <vector>
#include <string>

void
A()
{
    std::deque<int> a;
    std::vector<int>b;
    std::list<int> c;
    std::queue<int> d;
    std::set<int> e;
    std::stack<int> f;
    std::string g;
    std::priority_queue<int> i;

    a.push_back(1);
    a.push_front(1);
    b.push_back(1);
    c.push_back(1);
    c.push_front(1);
    d.push(1);
    e.insert(1);
    f.push(1);
    i.push(1);

    a.pop_back();
    a.pop_front();
    b.pop_back();
    c.pop_back();
    c.pop_front();
    d.pop();
    e.erase(1);
    f.pop();
    i.pop();
}

void
B()
{
    std::deque<short> a;
    std::vector<short>b;
    std::list<short> c;
    std::queue<short> d;
    std::set<short> e;
    std::stack<short> f;
    std::string g;
    std::priority_queue<short> i;

    a.push_back(1);
    a.push_front(1);
    b.push_back(1);
    c.push_back(1);
    c.push_front(1);
    d.push(1);
    e.insert(1);
    f.push(1);
    i.push(1);

    a.pop_back();
    a.pop_front();
    b.pop_back();
    c.pop_back();
    c.pop_front();
    d.pop();
    e.erase(1);
    f.pop();
    i.pop();
}

void
C()
{
    std::deque<float> a;
    std::vector<float>b;
    std::list<float> c;
    std::queue<float> d;
    std::set<float> e;
    std::stack<float> f;
    std::string g;
    std::priority_queue<float> i;

    a.push_back(1.0);
    a.push_front(1.0);
    b.push_back(1.0);
    c.push_back(1.0);
    c.push_front(1.0);
    d.push(1.0);
    e.insert(1.0);
    f.push(1.0);
    i.push(1.0);

    a.pop_back();
    a.pop_front();
    b.pop_back();
    c.pop_back();
    c.pop_front();
    d.pop();
    e.erase(1.0);
    f.pop();
    i.pop();
}

void
D()
{
    std::deque<double> a;
    std::vector<double>b;
    std::list<double> c;
    std::queue<double> d;
    std::set<double> e;
    std::stack<double> f;
    std::string g;
    std::priority_queue<double> i;

    a.push_back(1.0);
    a.push_front(1.0);
    b.push_back(1.0);
    c.push_back(1.0);
    c.push_front(1.0);
    d.push(1.0);
    e.insert(1.0);
    f.push(1.0);
    i.push(1.0);

    a.pop_back();
    a.pop_front();
    b.pop_back();
    c.pop_back();
    c.pop_front();
    d.pop();
    e.erase(1.0);
    f.pop();
    i.pop();
}

int
main()
{
    A();
    B();
    C();
    D();
}
