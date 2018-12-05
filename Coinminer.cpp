#include <iostream>
#include <string>
#include <ctime>
#include <random>
#include "sha1/sha1.cpp"
#include <numeric>
#include <algorithm>

using namespace std;

string gen_nonce()
{
    string str;
    unsigned char a;
    for (a=32; a<128; a++){
        str+= (char)a;
    }

    random_device rd;
    mt19937 mt_rand(rd());
    shuffle(str.begin(), str.end(), mt_rand);
    return str.substr(0, 32);
}

class Token {
public:
    string m_nonce;
    string m_triOwn;
    string m_proto;
    string m_reserved;
    time_t m_time;

    virtual void regen(){};


};


class genToken : public Token {
public:
    string token;

    genToken(string triOwn){
        this->m_nonce = gen_nonce();
        this->m_triOwn =  triOwn;
        this->m_proto = "CC1.0";
        this->m_reserved = "0f0f0f";
        this->m_time = time(NULL);
        this->token = m_nonce + "-" + m_triOwn + "-" + m_proto + "-" + to_string(m_time) + "-" + m_reserved;
    }

    void regen(){
        this->token = gen_nonce() + "-" + m_triOwn + "-" + m_proto + "-" + to_string(this->m_time = time(NULL)) + "-" + m_reserved;
    }
};

class Benchmark{
public:
    SHA1 shaone;
    clock_t start;
    double duration;
    time_t end;
    double moyenne;
    vector<double> moy;
    int nbToken;

    void benchmark(genToken* piece){
        start= clock();
        end = time(nullptr)+ 60;
        nbToken =0;

        cout <<"Starting benchmark ... "<<endl;

        while(nbToken<8 && time(nullptr) <= end)
        {

            // Hash SHA1
            shaone.update(piece->token);
            string hash = shaone.final();

            if(hash[0] == 'c' && hash[1] == 'c' && hash[2] == 'c' && hash[3] == 'c' && hash[4] != 'c'){
                duration = (clock() - start)/(double) CLOCKS_PER_SEC;
                moy.push_back(duration);
                start = clock();
                nbToken++;

            }
            piece->regen();
        }
        this->moyenne = accumulate( moy.begin(), moy.end(), 0.0)/moy.size();
        showBenchmark(moyenne);
    }


    template <typename T> void showBenchmark(T val){
        cout << "\n*** Mining projections ***"<<endl;
        cout << "6c (subcoin) " << val*256 << " s	(" << val*256 /3600<<" h)"<<endl;
        cout << "7c (coin) " << val*4096 <<" s	("<< val*4096 /3600<<" h)"<<endl;
        cout << "8c (hexcoin) " << val*65536  <<" s	("<<  val*65536 /3600<<" h)"<<endl;
        cout << "9c (arkenstone) " << val*1048576 <<" s	(" << val*1048576 /3600<<" h)"<<endl;
    }

    inline double sumTime(int a, int b){
        if(a>0 && b>0){
            return (a+b);
        }else if(a>0 && b==0){
            return a;
        }else if(a==0 && b>0){
            return b;
        }else {
            return -1;
        }
    };

    void generTokens(genToken* piece, int number){

        while (true){
            shaone.update(piece->token);
            string hash = shaone.final();

            int i=0;
            bool exist = true;

            while (i != number){
                if(hash[i]=='c'){
                    i++;
                }else{
                    exist = false;
                    break;
                }
            }

            if(exist){
                cout << hash << endl;
                //cout << piece->token<< endl;
            }

            piece->regen();

        }
    }
};




int main(int argc, char *argv[]){

    Benchmark b;
    try {
        if (argc == 1) {
            genToken* piece = new genToken("GOU");
            b.generTokens(piece, 1);
        } else if (argc > 1) {
            if (string(argv[1]) == "-z") {
                genToken* piece = new genToken("GOU");
                b.benchmark(piece);
            }

            else if (string(argv[1]) == "-t" && string(argv[3]) == "-m" && atoi(argv[4])>1){

                genToken* piece = new genToken(string(argv[2]));
                b.generTokens(piece, atoi(argv[4]));
            }

            else {
                throw std::invalid_argument("Invalid argument !");
            }
        }
    }
    catch (exception &err)
    {
        cout << "Exception: Invalid argument !" <<endl;
    }

    return 0;
}
