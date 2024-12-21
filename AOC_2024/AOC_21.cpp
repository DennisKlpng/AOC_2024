#include "utils.hpp"
#include <deque>

typedef std::pair<int16_t, int16_t> pt;
constexpr pt operator+(const pt& pt1, const pt& pt2){
    return pt(pt1.first + pt2.first, pt1.second + pt2.second);
}

struct visited_pt{
    pt pos;
    std::string instructions;
};

typedef std::pair<std::string, int64_t> inst_lvl;

static const std::map<int16_t, pt> keypad({
    { 7, pt(0,3)}, {8, pt(1,3)}, { 9, pt(2,3)},
    { 4, pt(0,2)}, {5, pt(1,2)}, { 6, pt(2,2)},
    { 1, pt(0,1)}, {2, pt(1,1)}, { 3, pt(2,1)},
    {-1, pt(0,0)}, {0, pt(1,0)}, {10, pt(2,0)}});

static const std::map<std::string, pt> dirpad({
    {" ", pt(0,1)}, {"^", pt(1,1)}, {"A", pt(2,1)},
    {"<", pt(0,0)}, {"v", pt(1,0)}, {">", pt(2,0)}});

static std::map<inst_lvl, int64_t> map_hash;

//robot always starts at A and ends at A, since every instruction ends with A
//this means when the outermost robot does some kind of move, this move always translates to the same dirpad-moves below
//=> we can just hash that move-pattern and get results faster
int64_t solve_robot(const std::string& instr, const int64_t rec_depth){
    inst_lvl hash_level(instr, rec_depth);

    if(map_hash.contains(hash_level)) return map_hash.at(hash_level);
    if(rec_depth == 1) return instr.size();

    int64_t res = 0;
    pt curr_pos(2,1);
    for(uint64_t i = 0; i < instr.size(); i++){
        int64_t res_local = std::numeric_limits<int64_t>::max();
        pt dst_pos(dirpad.at(std::string(&instr[i], 1)));
        std::deque<visited_pt> vis_queue{{curr_pos, ""}};
        while(!vis_queue.empty()){
            //bfs to get shortest path on dirpad
            auto curr = vis_queue.front();
            vis_queue.pop_front();

            if(curr.pos == dst_pos){
                //search for shortest robot-instructions to get that path
                // auto res_tmp = solve_robot(curr.instructions + "A", rec_depth - 1);
                res_local = std::min(res_local, solve_robot(curr.instructions + "A", rec_depth - 1));
                continue;
            }
            if(curr.pos == pt(0, 1)){
                continue; //never visit that spot
            }

            //if current x-pos != dst x-pos: step in x-direction
            if(curr.pos.first < dst_pos.first){
                vis_queue.push_back({curr.pos + pt(1, 0), curr.instructions + ">"});
            }
            else if(curr.pos.first > dst_pos.first){
                vis_queue.push_back({curr.pos + pt(-1, 0), curr.instructions + "<"});
            }
            //if current y-pos != dst y-pos: step in y-direction
            if(curr.pos.second < dst_pos.second){
                vis_queue.push_back({curr.pos + pt(0, 1), curr.instructions + "^"});
            }
            else if(curr.pos.second > dst_pos.second){
                vis_queue.push_back({curr.pos + pt(0, -1), curr.instructions + "v"});
            }
        }
        curr_pos = dst_pos;
        res += res_local;
    }

    map_hash.emplace(hash_level, res);
    return res;
}

int64_t solve_numpad_press(const pt& curr_pos, const pt& dst_pos){
    int64_t res = std::numeric_limits<int64_t>::max();
    std::deque<visited_pt> vis_queue{{curr_pos, ""}};
    while(!vis_queue.empty()){
        //bfs shortest path
        auto curr = vis_queue.front();
        vis_queue.pop_front();

        if(curr.pos == dst_pos){
            //search for shortest robot-instructions to get that path
            int64_t rec_depth = 3; //num dirpads to operate
            auto res_tmp = solve_robot(curr.instructions + "A", rec_depth);
            res = std::min(res, res_tmp);
            continue;
        }
        if(curr.pos == pt(0, 0)){
            continue; //never visit that spot
        }

        //if current x-pos != dst x-pos: step in x-direction
        if(curr.pos.first < dst_pos.first){
            vis_queue.push_back({curr.pos + pt(1, 0), curr.instructions + ">"});
        }
        else if(curr.pos.first > dst_pos.first){
            vis_queue.push_back({curr.pos + pt(-1, 0), curr.instructions + "<"});
        }
        //if current y-pos != dst y-pos: step in y-direction
        if(curr.pos.second < dst_pos.second){
            vis_queue.push_back({curr.pos + pt(0, 1), curr.instructions + "^"});
        }
        else if(curr.pos.second > dst_pos.second){
            vis_queue.push_back({curr.pos + pt(0, -1), curr.instructions + "v"});
        }
    }
    return res;
}

int64_t solve_intcode(const std::string code){
    int64_t res = 0;
    
    pt start(2,0);
    //get shortest path to solve this digit code => get shortest robot path for each move between digits
    for(int i : {0,1,2,3}){
        int16_t tgt = 0;
        if(code[i]=='A') tgt = 10;
        else tgt = std::stoi(std::string(&code[i], 1));
        pt dest = keypad.at(tgt);
        res += solve_numpad_press(start, dest);
        start = dest;
    }

    return res;
}

std::pair<uint64_t, uint64_t> solve_puzzle(std::string filename){
    std::pair<uint64_t, uint64_t> res{0, 0};

    auto data = read_file_as_lines(filename);
    for (auto &line : data){
        res.first += get_ints_from_string(line)[0] * solve_intcode(line);
    }

    return res;
}

int main(){
    std::pair<uint64_t, uint64_t> res = solve_puzzle("inputs/Test_21.txt");
    print("Test res pt 1:", res.first, "pt 2:", res.second);
    double time_spent;
    res = profile_function(solve_puzzle, time_spent, "inputs/Data_21.txt");
    print("Puzzle res pt 1:", res.first, "pt 2:", res.second, "puzzle calculation took:", time_spent, "ms");

    return 0;
}