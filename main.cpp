#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <exception>
#include <unordered_map>
#include <stack>

class Tree
{
public:
    std::string data;
    std::vector<Tree> children;
    Tree(std::string d) { data = d; }
};

std::string extractPath(std::string f)
{
    int p;
    if ((p = f.find_last_of('/')) == std::string::npos)
        return "";
    return f.substr(0, p + 1);
}

std::string extractFileName(std::string f)
{
    int p, p1;
    // Assign p = 0 if '/' not found
    (p = f.find_last_of('/')) == std::string::npos ? p = -1 : 0;
    (p1 = f.find_last_of('.')) == std::string::npos ? p1 = 0 : 0;
    return f.substr(p + 1, p1 - p - 1);
}

std::string extractFileNameWithPath(std::string f)
{
    int p1;
    // Assign p = 0 if '/' not found

    (p1 = f.find_last_of('.')) == std::string::npos ? p1 = 0 : 0;
    return f.substr(0, p1);
}
Tree GenTree(std::string start)
{
    Tree tree = Tree(start);
    std::ifstream f;

    f.open(start);
    if (!f.is_open())
        throw std::runtime_error("Could not open " + start);
    std::string path = extractPath(start);
    for (std::string line; std::getline(f, line);)
    {
        if (line[0] != '#')
            continue;
        int p;
        if ((p = line.find('\"')) == std::string::npos)
            continue;
        tree.children.push_back(path + line.substr(p + 1, line.size() - p - 2));
    }
    for (size_t i = 0; i < tree.children.size(); i++)
        tree.children[i] = GenTree(tree.children[i].data);
    return tree;
}

void GenMake(Tree t, std::unordered_map<std::string, bool> *m, std::stack<std::string> *s, std::string cmp, std::string cExt, std::string hExt)
{
    for (size_t i = 0; i < t.children.size(); i++)
        GenMake(t.children[i], m, s, cmp, cExt, hExt);
    // skip node if already created
    if ((*m)[t.data])
        return;
    (*m)[t.data] = 1;

    std::string fi = extractFileName(t.data);
    std::string fn = extractFileNameWithPath(t.data);
    (*s).push(fi + ".tmp");

    std::ofstream f;
    f.open((*s).top());
    if (!f.is_open())
        throw std::runtime_error("Could not create file  for" + t.data);
    f << fi + ".o: " +
             ((t.data[t.data.size() - 1] == 'c') || (t.data[t.data.size() - 1] == 'p')
                  ? fn + cExt + ' '
                  : fn + cExt + ' ' + fn + hExt);
    // for (size_t i = 0; i < t.children.size(); i++)
    //     f <<' ' + extractFileName(t.children[i].data) + ".o";
    f << "\n\t" + cmp + " -c " + fn + cExt;
    // for (size_t i = 0; i < t.children.size(); i++)
    //     f <<' ' + extractFileName(t.children[i].data) + ".o";
    f << '\n';
}

int main(int argc, char **argv)
{    
    if (argc < 5)
    {
        std::cout << "Not enough arguments\n";
        return 1;
    }
    std::string args = "";
    if (argc > 6)
        for (size_t i = 7; i < argc; i++)
            args += argv[i];

    Tree t = GenTree(argv[1]);
    std::unordered_map<std::string, bool> m;
    std::stack<std::string> files;

    std::string cmp = argv[2];


    std::string cpp =  "." + (std::string)argv[3];
    std::string h =  "." + (std::string)argv[4];
    GenMake(t, &m, &files, cmp,cpp,h);

    std::ofstream f;
    f.open(extractPath(argv[1]) + "Makefile");
    if (!f.is_open())
        throw std::runtime_error("Could not create make file");
    f << "output: "; //+ t.data;
    std::vector<std::pair<std::string, bool>> v(m.begin(), m.end());

    for (size_t i = 0; i < m.size(); i++)
        f << ' ' + extractFileName((v[i]).first) + ".o";
    f << "\n\t" + cmp;
    for (size_t i = 0; i < m.size(); i++)
        f << ' ' + extractFileName((v[i]).first) + ".o";
    f << " -o ";
    f << ((argc < 6) ? "output" : argv[5]) + args + '\n';

    while (!files.empty())
    {
        std::ifstream f1;
        f1.open(files.top());
        std::string l;
        while (getline(f1, l))
            f << l +'\n';
        std::remove(files.top().c_str());
        files.pop();
    }

    return 0;
}