
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <yaml-cpp/yaml.h>
#include "deck.h"
#include "context.h"
#include "simulator.h"

using namespace std;
using namespace YGO;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        panic("Usage: ./ygo-calc.exe <deck.yml>");
    }
    YAML::Node root = YAML::LoadFile(argv[1]);
    Deck deck(root["deck"]);
    cout << deck;

    Context context;
    for (auto& card : deck.cards())
    {
        context.addCardNameCondition(card);
    }
    YAML::Node classes = root["deck"]["classes"];
    if (classes.IsDefined())
    {
        for (auto it = classes.begin(); it != classes.end(); ++it)
        {
            auto className = it->first.as<t_string>();
            auto condition = it->second.as<t_string>();
            context.addCondition(className, Utils::parse(context, condition));
        }
    }
    context.print(cout);
    cout << endl;

    Simulator simulator(root["simulate"]);
    simulator.run(deck, context);
}
