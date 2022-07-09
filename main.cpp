#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <exception>
#include <unordered_map>
#include <stack>
#include <getopt.h>

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
    // Assign p1 = 0 if '.' not found
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

void GenMake(Tree t, std::unordered_map<std::string, bool> *m, std::stack<std::string> *s, std::ofstream *cf, std::string cmp, std::string cExt, std::string hExt)
{
    // Go from the bottom of the tree up
    for (size_t i = 0; i < t.children.size(); i++)
        GenMake(t.children[i], m, s, cf, cmp, cExt, hExt);

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
    (*cf) << ' ' << fi + ".o";
    f << fi + ".o: " +
             ((t.data[t.data.size() - 1] == 'c') || (t.data[t.data.size() - 1] == 'p')
                  ? fn + cExt + ' '
                  : fn + cExt + ' ' + fn + hExt);
    f << "\n\t" + cmp + " -c " + fn + cExt << '\n';
}

void Help()
{
    std::cout << "Makemaker\n";
    std::cout << "Usage: ";
    std::cout << "makemaker -i <file> -c <compiler> -f <code filetype> -H <header filetype> -o <output name> <compiler arguments>\n";
    std::cout << "Default parameters are:\n-c = g++\n-f = cpp\n-H = h\n-o = output\n";
    std::cout << "Example: makemaker -i main.cpp -c g++ -f cpp -H h -o main\n";
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        Help();
        return 0;
    }
    std::string input = "", compiler = "g++", file = ".cpp", header = ".h", output = "output", arguments = "";
    int opt;
    while ((opt = getopt(argc, argv, "hi:c:f:H:o:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            Help();
            return 0;
            break;
        case 'i':
            input = optarg;
            break;
        case 'c':
            compiler = optarg;
            break;
        case 'f':
            file = "." + std::string(optarg);
            break;
        case 'H':
            header = "." + std::string(optarg);
            break;
        case 'o':
            output = optarg;
            break;
        }
    }
    for (; optind < argc; optind++)
        arguments += argv[optind] + ' ';

    if (input == "")
    {
        Help();
        return 0;
    }
    Tree t = GenTree(input);
    std::unordered_map<std::string, bool> m;
    std::stack<std::string> files;

    // 2 .tmp (s) just in case someone has a file clean.h
    //Setup the clean part, put it in the stack first so that it's placed at the end
    files.push("clean.tmp.tmp");
    std::ofstream cf;
    cf.open("clean.tmp.tmp");
    cf << "clean:\n\trm " << output;

    GenMake(t, &m, &files, &cf, compiler, file, header);
    cf.close();

    std::ofstream f;
    f.open(extractPath(input) + "Makefile");
    if (!f.is_open())
        throw std::runtime_error("Could not create make file");
    f << "output: "; //+ t.data;
    std::vector<std::pair<std::string, bool>> v(m.begin(), m.end());

    for (size_t i = 0; i < m.size(); i++)
        f << ' ' + extractFileName((v[i]).first) + ".o";
    f << "\n\t" + compiler;
    for (size_t i = 0; i < m.size(); i++)
        f << ' ' + extractFileName((v[i]).first) + ".o";
    f << " -o ";
    f << output + arguments + '\n';

    while (!files.empty())
    {
        std::ifstream f1;
        f1.open(files.top());
        std::string l;
        while (getline(f1, l))
            f << l + '\n';
        std::remove(files.top().c_str());
        files.pop();
    }
    return 0;
}