
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <yaml-cpp/yaml.h>
#include "deck.h"
#include "context.h"
#include "simulator.h"

using namespace std;
namespace fs = std::filesystem;
using namespace YGO;

string interactiveInputFile()
{
    cout << "please input config path (*.yml): " << flush;
    string path;
    cin >> path;
    return path;
}

void checkPath(const string& path)
{
    const fs::path fpath{ path };
    if (!fs::exists(fpath))
    {
        cout << "file doesn't exist: " << fs::absolute(fpath) << endl;
        panic("");
    }
    const fs::file_status status = fs::status(fpath);
    if (status.type() != fs::file_type::regular)
    {
        cout << fpath << " is not a valid file" << endl;
        panic("");
    }
}

int main(int argc, char **argv)
{
    if (argc > 2)
    {
        panic("Usage: ./ygo-calc.exe <deck.yml>");
    }
    string path = argc == 1 ? interactiveInputFile() : argv[1];
    checkPath(path);
    YAML::Node root =  YAML::LoadFile(path);
    Deck deck(root["deck"]);
    cout << deck;

    Context context;
    for (auto& card : deck.cards())
    {
        context.addCardNameCondition(card);
    }
    YAML::Node conditions = root["deck"]["alias"];
    if (conditions.IsDefined())
    {
        for (auto it = conditions.begin(); it != conditions.end(); ++it)
        {
            auto conditionName = it->first.as<t_string>();
            auto condition = it->second.as<t_string>();
            context.addCondition(conditionName, Utils::parse(context, condition));
        }
    }
    //context.print(cout);
    //cout << endl;

    Simulator simulator(root["simulate"]);
    simulator.run(deck, context);

    system("pause");
    return 0;
}
