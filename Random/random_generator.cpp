#include <iostream>
#include <random>
#include <chrono>
#include <vector>
#include <fstream>
using namespace std;

mt19937 mt(chrono::high_resolution_clock::now().time_since_epoch().count());

vector <char> characters;

void load_symbols()
{
    characters.clear();
    for(int i=0;i<26;i++)
    {
        characters.emplace_back('A'+i);
    }

    for(int i=0;i<26;i++)
    {
        characters.emplace_back('a'+i);
    }

    characters.emplace_back('!');
    characters.emplace_back('@');
    characters.emplace_back('#');
    characters.emplace_back('$');
    characters.emplace_back('%');
    characters.emplace_back('*');
    characters.emplace_back('-');
    characters.emplace_back('=');
    characters.emplace_back('(');
    characters.emplace_back(')');
    
}

class Generator
{
    private:
        vector <string> randomStrs;
    public:
        Generator(int size,int lb,int ub)
        {
            load_symbols();
            uniform_int_distribution <int> range(lb,ub);
            uniform_int_distribution <int> rchars(0,characters.size()-1);
            string word;
            for(int i=0;i<size;i++)
            {
                word="";
                for(int i=0,t=range(mt);i<t;i++)
                {
                    word+=characters[rchars(mt)];
                }
                this->randomStrs.emplace_back(word);
            }
        }

        void to_txt()
        {
            fstream fp;
            fp.open("Randomizer5.txt",ios::out);
            for(auto &x:this->randomStrs)
            {
                fp<<x<<endl;
            }
            fp.close();
        }
};


int main()
{
    Generator gen(50,10,30);
    gen.to_txt();
    return EXIT_SUCCESS;
}