# poke_data
Data from poke api but in another format for [PokeGuesser](https://pokeguesser.baduit.eu/) and Pokedle (not created yet)

## Designs choices

### Why C++
I wanted to try a very little bit std::expected and generator in C++ and see how it could interact with ranges.

### Error management
I used std::expected for recoverable errors (only once in the code I think), otherwise, if I can't recover from it, I use exception. (catched in main to be sure that the error message is displayed even on windows)

### Strong typing
I used strong typed when I though it made sense and added value, maybe some other fields could benefit from it.

### I didn't really care about performance
I just wanted to try out some stuff and then to be able to use it on a pet project

### Why not just call pokeapi
* Calling another API can add some overhead when loading a pokemon if we wan't something else than english informations so I needed another solution that just using the rest API, there is probably other solution, and they are probably smarter but that's not the point.
* This project is an excuse to try some stuff in C++

## How to build
Use cmake + vcpkg and a recent compiler with some feature from C++23 (I think there is only std::expected, everything else is C++20 at most)

## How to run
* The first argument is the path of the data directory. (for example C:/Users/MyUserName/Documents/GitHub/api-data/data)    
* The second argument is the path of the output data directory will be written (it must exists and be empty or at least without any already generated files)

Todo : add an exemple
