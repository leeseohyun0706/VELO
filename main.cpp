

#include "../VELO/includes/HashMap.hpp"
#include "../VELO/includes/Tree.hpp"
#include "../VELO/includes/Iterator.hpp"
#include "includes/Array.hpp"
#include "includes/Function.hpp"
#include "includes/Tuple.hpp"
#include <cstdlib>
#include <ctime>
#include <string>
#include <unordered_map>
using namespace VELO;

void rstp(int l1, int l2, int l3) {
    printf("%d\n", l1+l2+l3);
}

int main() {
    Function<void(int,int,int)> fc(rstp);
    static constexpr int N = 200000;

    for(int i = 0; i < 100; i++) {
        fc(i, i, i);
    }
    
    clock_t c1 = clock();
    {
    VELO::HashMap<std::string, int, Hash<std::string>, Basic_Template_Allocator<__hash_map_node<std::string, int>>> cmap(N);
        
    for(int i = 0; i < N; i++) {
        cmap.insert("Fuck"+std::to_string(i), 2676);
        cmap.insert("느금"+std::to_string(i), 6466);
        cmap.insert("Fu3ck"+std::to_string(i), 2676);
        cmap.insert("느2금"+std::to_string(i), 6466);
        cmap.insert("Fuc0k"+std::to_string(i), 2676);
        cmap.insert("느9금"+std::to_string(i), 6466);
        cmap.insert("Fu3ck"+std::to_string(i), 2676);
        cmap.insert("느금111"+std::to_string(i), 6466);

        cmap.erase("Fu3ck"+std::to_string(i));
        cmap.erase("느9금"+std::to_string(i));
    }
    }
    clock_t c2 = clock();

    clock_t l1 = clock();
{
    std::unordered_map<std::string, int, Hash<std::string>> cmap;
    cmap.reserve(N);
    for(int i = 0; i < N; i++) {
        cmap.insert({"Fuck"+std::to_string(i), 2676});
        cmap.insert({"느금"+std::to_string(i), 6466});
        cmap.insert({"Fu3ck"+std::to_string(i), 2676});
        cmap.insert({"느2금"+std::to_string(i), 6466});
        cmap.insert({"Fuc0k"+std::to_string(i), 2676});
        cmap.insert({"느9금"+std::to_string(i), 6466});
        cmap.insert({"Fu3ck"+std::to_string(i), 2676});
        cmap.insert({"느금111"+std::to_string(i), 6466});

        cmap.erase("Fu3ck"+std::to_string(i));
        cmap.erase("느9금"+std::to_string(i));
    }
}
    clock_t l2 = clock();

    printf("C : %lu\nL : %lu\n", c2-c1, l2-l1);

    Tuple<int, std::string, char, float> fsc(34, "fdss", 'c', 32.f);

    printf("%s\n", get<1>(fsc).c_str());    
    // srand(time(0));
    // RBSet<size_t> rset;
    // for(int i = 0; i < 100; ++i) {
    //     rset.insert(rand()%10000);
    // }

    // VELO::for_each(rset.begin(), rset.end(), [](size_t ls) {
    //     printf("%lu\n", ls);
    // });
    // for(auto& r : rset) printf("%lu\n", r);
    // __rb_tree<std::string> sset;
    // sset.insert("니애미");
    // sset.insert("니애비");
    // sset.insert("니할매");
    // sset.insert("니할배");

    // for(auto& i : sset) {
    //     printf("%s\n", i.c_str());
    // }
    
    // sset.erase("니애미");

    // for(auto& i : sset) {
    //     printf("%s\n", i.c_str());
    // }

    // __rb_tree<int> fvec;
    // fvec.insert(362);
    // fvec.insert(3692);
    // fvec.insert(3362);
    // fvec.insert(36872);

    // for(auto& i : fvec) {
    //     printf("%d\n", i);
    // }
}
