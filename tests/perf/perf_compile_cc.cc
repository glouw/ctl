#include <deque>
#include <list>
#include <queue>
#include <set>
#include <stack>
#include <vector>
#include <string>

int main()
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
