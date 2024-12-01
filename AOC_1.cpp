#include "utils.hpp"
#include <algorithm>
#include <numeric>
#include <unordered_map>

std::pair<int, int> solve_puzzle(std::string filename){
    std::pair<int, int> res;

    std::vector<std::string> lines = read_file_as_lines(filename);
    std::vector<int> list_1;
    std::vector<int> list_2;
    for(auto& n : lines){
        //split each line into vector of ints, stuff them into separate vectors
        std::vector<int> ints_line = split_string_int(n);
        list_1.push_back(ints_line[0]);
        list_2.push_back(ints_line[1]);
    }

    //pt 1: 
    //sort by size
    std::sort(list_1.begin(), list_1.end());
    std::sort(list_2.begin(), list_2.end());
    
    //use a transform to stuff the absolute diff into a third vector
    std::vector<int> vec_res;
    vec_res.resize(list_1.size());
    std::transform(list_1.begin(), list_1.end(), list_2.begin(), vec_res.begin(), 
        [](int a, int b){
            return abs(a - b);
        });
    res.first = std::accumulate(vec_res.begin(), vec_res.end(), 0);

    //pt 2:
    //the logical solution here is to simply fill the map by iterating through each entry of list_2
    std::unordered_map<int, int> occ_map;
    //the funny solution is this:
    int token = *list_2.begin();
    std::vector<int>::iterator first = list_2.begin();
    std::pair<std::vector<int>::iterator,std::vector<int>::iterator> bounds;
    while(true){
        bounds = std::equal_range(first, list_2.end(), token);
        occ_map.insert(std::make_pair(token, static_cast<int>(std::distance(bounds.first, bounds.second))));
        if(bounds.second == list_2.end()--){
            break;
        }
        token = *(bounds.second++);
    }
    //now iterate through list 1 and multiply the values by the entry from list 2:
    res.second = std::accumulate(list_1.begin(), list_1.end(), 0, 
        [&occ_map](int a, int b){
            return (a + b * occ_map[b]);
        });

    return res;
}

int main(){
    std::pair<int, int> res = solve_puzzle("Test_1.txt");
    std::cout << "TEST res pt 1: " << res.first << " pt 2: " << res.second << "\n";
    res = solve_puzzle("Data_1.txt");
    std::cout << "Puzzle res pt 1: " << res.first << " pt 2: " << res.second << "\n";

    return 0;
}