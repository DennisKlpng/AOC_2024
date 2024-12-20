import sys


if __name__ == '__main__':
    with open(sys.argv[1], "r") as f:
        input_list = f.readline().split(",")
        fishies = [input_list.count(str(i)) for i in range(9)]

        for day in range(256):
            fishies_to_multiply = fishies[0]
            for i in range(1, 9):
                fishies[i-1] = fishies[i]
            fishies[6] += fishies_to_multiply
            fishies[8] = fishies_to_multiply

        print(" Fish count: " + str(sum(fishies)))
