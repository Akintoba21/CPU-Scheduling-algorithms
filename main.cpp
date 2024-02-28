#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <string>
#include <stdio.h>
#include <math.h>
#include <map>
#include <algorithm>
#include <tuple>
#include <deque>

double next_exp(double l){
    double n = drand48();
    return (0 - (log(n))) / l;
}

std::map<std::string, std::deque<double>> generateBursts(bool io, double l, int bound){
    std::map<std::string, std::deque<double>> data;
    double itime = floor(next_exp(l));
    while(itime > bound){
        itime = floor(next_exp(l));
    };
    double nbursts = ceil(drand48() * 100);
    std::cout << " arrival time " << itime << "ms; " << nbursts << " CPU bursts\n";
    data["itime"].push_back(itime);
    for(int i = 0;i<nbursts;i++){
        double cputime = ceil(next_exp(l));
        while(cputime >= bound){
        cputime = ceil(next_exp(l));
        }
        double iotime;
        if(i != nbursts-1){
            iotime = ceil(next_exp(l));
            while(iotime >= bound){
                iotime = ceil(next_exp(l));
            }
            iotime = iotime * 10;
        }
        if(!io){
            cputime *= 4;
            if(i != nbursts-1){iotime = floor(iotime/4);}
        }
        //std::cout << "--> CPU burst " << cputime << "ms";
        if(i != nbursts-1){
            //std::cout << " --> I/O burst " << iotime << "ms\n";
        }
        else{/*std::cout << "\n";*/}
        data["cbursts"].push_back(cputime);
        data["ibursts"].push_back(iotime);
    }
    return data;
}

int fcfs(std::map<char, std::map<std::string, std::deque<double>>> &bursts,std::map<std::string,double> &data,int Bcount, int Pcount, int tcs, int cpuCount, int cpubcount, int iobcount, double btime, double cbtime, double ibtime){
    std::map<char, std::map<std::string, std::deque<double>>> tempbursts(bursts);
    int time = 0;
    int switcht = tcs/2;
    std::cout << "time " << time << "ms: Simulator started for FCFS [Q <empty>]\n";
    std::deque<char> queue;
    std::vector<char> arrived;
    std::map<char,double> io;
    std::tuple<char,double> cpu;
    std::map<char,std::tuple<double,double>> waitTimes;
    int switches = 0;
    int cpuswitches = 0;
    int ioswitches = 0;
    cpu = std::make_tuple(NULL, 0);
    while(tempbursts.size() > 0){
        //if(time > 100000){break;}
        bool increment = true; //only want to increment time manually if nothing interesting has happened
        //std::cout << time << "\n";
        if(arrived.size() < (long unsigned int)Pcount){//check if any new processes have arrived
            for(auto & it : tempbursts){
                double temp = it.second["itime"].front();
                //std::cout << temp << "\n";
                if(temp == time && std::find(arrived.begin(),arrived.end(),it.first) == arrived.end()){
                    increment = false;
                    queue.push_back(it.first);
                    waitTimes[it.first] = std::make_tuple(0,time);
                    arrived.push_back(it.first);
                    if(time < 10000){std::cout << "time " << time << "ms: Process " << it.first << " arrived; added to ready queue [Q";
                    for(long unsigned int i = 0;i < queue.size();i++){
                        std::cout << " " << queue[i];
                    }
                    std::cout << "]\n";
                    }
                }
            }
        }
        //if cpu is empty, add process from queue
        if(!std::get<0>(cpu)){
            if(queue.size() != 0){
                increment = false;
                char pname = queue.front();
                waitTimes[pname] = std::make_tuple(std::get<0>(waitTimes[pname]) + (time-std::get<1>(waitTimes[pname])), 0);
                queue.pop_front();
                double ptime = tempbursts[pname]["cbursts"].front();
                tempbursts[pname]["cbursts"].pop_front();
                time += switcht;
                for(auto it : io){
                            if(it.second <= time){
                                increment = false;
                                if(tempbursts[it.first]["cbursts"].size() != 0){
                                    queue.push_back(it.first);
                                    waitTimes[it.first] = std::make_tuple(std::get<0>(waitTimes[it.first]), it.second);
                                    if(time < 10000){std::cout << "time " << it.second << "ms: Process " << it.first << " completed I/O; added to ready queue [Q";
                                    if(queue.size() == 0){
                                        std::cout << " <empty>]\n";
                                    }
                                    else{
                                        for(long unsigned int i = 0;i < queue.size();i++){
                                            std::cout << " " << queue[i];
                                        }
                                        std::cout << "]\n";
                                    }
                                    }
                                }
                                else{
                                    tempbursts.erase(it.first);
                                }
                                io.erase(it.first);
                                break;
                            }
                        }
                switches++;
                if(pname - 'A' < Pcount - cpuCount){ioswitches++;}
                else{cpuswitches++;}
                if(time < 10000){std::cout << "time " << time << "ms: Process " << pname << " started using the CPU for " << ptime << "ms burst [Q";
                if(queue.size() == 0){
                    std::cout << " <empty>]\n";
                }
                else{
                    for(long unsigned int i = 0;i < queue.size();i++){
                        std::cout << " " << queue[i];
                    }
                    std::cout << "]\n";
                }
                }
                cpu = std::make_tuple(pname, ptime + time);
                if(tempbursts[std::get<0>(cpu)]["cbursts"].size() == 0){
                    std::cout << "time " << time + ptime << "ms: Process " << std::get<0>(cpu) << " terminated [Q";
                    if(queue.size() == 0){
                        std::cout << " <empty>]\n";
                    }
                    else{
                        for(long unsigned int i = 0;i < queue.size();i++){
                            std::cout << " " << queue[i];
                        }
                        std::cout << "]\n";
                    }
                    tempbursts.erase(std::get<0>(cpu));
                    if(tempbursts.size() == 0){
                        time += ptime + switcht;
                        for(auto it : io){
                            if(it.second <= time){
                                increment = false;
                                if(tempbursts[it.first]["cbursts"].size() != 0){
                                    queue.push_back(it.first);
                                    waitTimes[it.first] = std::make_tuple(std::get<0>(waitTimes[it.first]), it.second);
                                    if(time < 10000){std::cout << "time " << it.second << "ms: Process " << it.first << " completed I/O; added to ready queue [Q";
                                    if(queue.size() == 0){
                                        std::cout << " <empty>]\n";
                                    }
                                    else{
                                        for(long unsigned int i = 0;i < queue.size();i++){
                                            std::cout << " " << queue[i];
                                        }
                                        std::cout << "]\n";
                                    }
                                    }
                                }
                                else{
                                    tempbursts.erase(it.first);
                                }
                                io.erase(it.first);
                                break;
                            }
                        }
                        break;
                    } 
                }
            }
        }
        else{
            if(time == std::get<1>(cpu)){
                increment = false;
                if(time < 10000){std::cout << "time " << time << "ms: Process " << std::get<0>(cpu) << " completed a CPU burst; " << tempbursts[std::get<0>(cpu)]["cbursts"].size() << " bursts to go [Q";
                if(queue.size() == 0){
                    std::cout << " <empty>]\n";
                }
                else{
                    for(long unsigned int i = 0;i < queue.size();i++){
                        std::cout << " " << queue[i];
                    }
                    std::cout << "]\n";
                }
                }
                double itime = tempbursts[std::get<0>(cpu)]["ibursts"].front();
                tempbursts[std::get<0>(cpu)]["ibursts"].pop_front();
                itime += time;
                if(time < 10000){std::cout << "time " << time << "ms: Process " << std::get<0>(cpu) << " switching out of CPU; blocking on I/O until time " << itime + switcht << "ms [Q";
                if(queue.size() == 0){
                    std::cout << " <empty>]\n";
                }
                else{
                    for(long unsigned int i = 0;i < queue.size();i++){
                        std::cout << " " << queue[i];
                    }
                    std::cout << "]\n";
                }
                }
                io[std::get<0>(cpu)] = itime + switcht;
                time += switcht;
                for(auto it : io){
                            if(it.second <= time){
                                increment = false;
                                if(tempbursts[it.first]["cbursts"].size() != 0){
                                    queue.push_back(it.first);
                                    waitTimes[it.first] = std::make_tuple(std::get<0>(waitTimes[it.first]), it.second);
                                    if(time < 10000){std::cout << "time " << it.second << "ms: Process " << it.first << " completed I/O; added to ready queue [Q";
                                    if(queue.size() == 0){
                                        std::cout << " <empty>]\n";
                                    }
                                    else{
                                        for(long unsigned int i = 0;i < queue.size();i++){
                                            std::cout << " " << queue[i];
                                        }
                                        std::cout << "]\n";
                                    }
                                    }
                                }
                                else{
                                    tempbursts.erase(it.first);
                                }
                                io.erase(it.first);
                                break;
                            }
                        }
                cpu = std::make_tuple(NULL, 0);
            }
        }
        for(auto it : io){
            if(it.second == time){
                increment = false;
                if(tempbursts[it.first]["cbursts"].size() != 0){
                    queue.push_back(it.first);
                    waitTimes[it.first] = std::make_tuple(std::get<0>(waitTimes[it.first]), time);
                    if(time < 10000){std::cout << "time " << time << "ms: Process " << it.first << " completed I/O; added to ready queue [Q";
                    if(queue.size() == 0){
                        std::cout << " <empty>]\n";
                    }
                    else{
                        for(long unsigned int i = 0;i < queue.size();i++){
                            std::cout << " " << queue[i];
                        }
                        std::cout << "]\n";
                    }
                    }
                }
                else{
                    tempbursts.erase(it.first);
                }
                io.erase(it.first);
                break;
            }
        }
        if(increment){time++;} //if nothing interesting has happened, increment time
    }
    std::cout << "time " << time << "ms: Simulator ended for FCFS [Q <empty>]\n";
    double totalwtime = 0;
    double cpuwtime = 0;
    double iowtime = 0;
    int i = 0;
    for(auto it : waitTimes){
        totalwtime += std::get<0>(it.second);
        if(i < Pcount - cpuCount){
            iowtime += std::get<0>(it.second);
        }
        else{
            cpuwtime += std::get<0>(it.second);
            }
        i++;
    }
    data["avgWtime"] = totalwtime / Bcount; 
    data["cpuavgWtime"] = cpuwtime / cpubcount;
    data["ioavgWtime"] = iowtime / iobcount;
    data["avgTtime"] = (btime + (switches * (switcht*2))+totalwtime) / Bcount;
    data["cpuavgTtime"] = (cbtime + (cpuswitches * (switcht*2))+cpuwtime) / cpubcount;
    data["ioavgTtime"] = (ibtime + (ioswitches * (switcht*2))+iowtime) / iobcount;
    data["switches"] = switches;
    data["cpuswitches"] = cpuswitches;
    data["ioswitches"] = ioswitches;
    return time;
}

int sjf(std::map<char, std::map<std::string, std::deque<double>>> &bursts,std::map<std::string,double> &data,int Bcount, int Pcount, int tcs, double alpha, double lambda, int cpuCount, int cpubcount, int iobcount, double btime, double cbtime, double ibtime){
    std::map<char, std::map<std::string, std::deque<double>>> tempbursts(bursts);
    int time = 0;
    int switcht = tcs/2;
    std::cout << "time " << time << "ms: Simulator started for SJF [Q <empty>]\n";
    std::deque<std::tuple<char, double>> queue;
    std::vector<char> arrived;
    std::map<char,double> io;
    std::tuple<char,double> cpu;
    int switches = 0;
    int cpuswitches = 0;
    int ioswitches = 0;
    std::map<char,double> tau;
    std::map<char,double> newtau;
    std::map<char,std::tuple<double,double>> waitTimes;
    cpu = std::make_tuple(NULL, 0);
    while(tempbursts.size() > 0){
        //if(time > 100000){break;}
        bool increment = true; //only want to increment time manually if nothing interesting has happened
        //std::cout << time << "\n";
        if(arrived.size() < (long unsigned int)Pcount){//check if any new processes have arrived
            for(auto & it : tempbursts){
                double temp = it.second["itime"].front();
                //std::cout << temp << "\n";
                if(temp == time && std::find(arrived.begin(),arrived.end(),it.first) == arrived.end()){
                    increment = false;
                    std::tuple<char,double> ptup = std::make_tuple(it.first, 1/lambda);
                    if(queue.size() > 0){
                        for(std::deque<std::tuple<char,double>>::iterator itr = queue.begin();itr != queue.end();itr++){
                            if(std::get<1>(*itr) > 1/lambda || queue.size() == 0 || ((std::get<1>(*itr) == 1/lambda) && (it.first - 'A' < std::get<0>(*itr) - 'A'))){
                                queue.insert(itr, ptup);
                                break;
                            }
                        }
                    }
                    if(std::find(queue.begin(),queue.end(),ptup) == queue.end()){queue.push_back(ptup);}
                    waitTimes[it.first] = std::make_tuple(0,time);
                    //queue.push_back(ptup);
                    arrived.push_back(it.first);
                    tau[it.first] = 1/lambda;
                    if(time < 10000){std::cout << "time " << time << "ms: Process " << it.first << " (tau " << tau[it.first] << "ms) arrived; added to ready queue [Q";
                    for(long unsigned int i = 0;i < queue.size();i++){
                        std::cout << " " << std::get<0>(queue[i]);
                    }
                    std::cout << "]\n";
                }
                }
            }
        }
        //if cpu is empty, add process from queue
        if(!std::get<0>(cpu)){
            if(queue.size() != 0){
                increment = false;
                char pname = std::get<0>(queue.front());
                queue.pop_front();
                waitTimes[pname] = std::make_tuple(std::get<0>(waitTimes[pname]) + (time - std::get<1>(waitTimes[pname])), 0);
                double ptime = tempbursts[pname]["cbursts"].front();
                tempbursts[pname]["cbursts"].pop_front();
                time += switcht;
                for(auto it : io){
                            if(it.second <= time){
                                increment = false;
                                if(tempbursts[it.first]["cbursts"].size() != 0){
                                    if(queue.size() > 0){
                                        for(std::deque<std::tuple<char,double>>::iterator itr = queue.begin();itr != queue.end();itr++){
                                            if(std::get<1>(*itr) > tau[it.first] || queue.size() == 0 || ((std::get<1>(*itr) == tau[it.first]) && (it.first - 'A' < std::get<0>(*itr) - 'A'))){
                                                queue.insert(itr, std::make_tuple(it.first, tau[it.first]));
                                                break;
                                            }
                                        }
                                    }
                                    if(std::find(queue.begin(),queue.end(),std::make_tuple(it.first, tau[it.first])) == queue.end()){queue.push_back(std::make_tuple(it.first, tau[it.first]));}
                                    waitTimes[it.first] = std::make_tuple(std::get<0>(waitTimes[it.first]), it.second);
                                    if(time < 10000){std::cout << "time " << it.second << "ms: Process " << it.first << " completed I/O; added to ready queue [Q";
                                    if(queue.size() == 0){
                                        std::cout << " <empty>]\n";
                                    }
                                    else{
                                        for(long unsigned int i = 0;i < queue.size();i++){
                                            std::cout << " " << std::get<0>(queue[i]);
                                        }
                                        std::cout << "]\n";
                                    }
                                    }
                                }
                                else{
                                    tempbursts.erase(it.first);
                                }
                                io.erase(it.first);
                                break;
                            }
                        }
                switches++;
                if(pname - 'A' < Pcount - cpuCount){ioswitches++;}
                else{cpuswitches++;}
                double newTau = ceil((alpha * ptime) + ((1-alpha) * tau[pname]));
                if(time < 10000){std::cout << "time " << time << "ms: Process " << pname << " (tau " << tau[pname] << "ms) started using the CPU for " << ptime << "ms burst [Q";
                if(queue.size() == 0){
                    std::cout << " <empty>]\n";
                }
                else{
                    for(long unsigned int i = 0;i < queue.size();i++){
                        std::cout << " " << std::get<0>(queue[i]);
                    }
                    std::cout << "]\n";
                }
                }
                if(newTau != tau[pname]){
                    newtau[pname] = newTau;
                }
                cpu = std::make_tuple(pname, ptime + time);
                if(tempbursts[std::get<0>(cpu)]["cbursts"].size() == 0){
                    std::cout << "time " << time + ptime << "ms: Process " << std::get<0>(cpu) << " terminated [Q";
                    if(queue.size() == 0){
                        std::cout << " <empty>]\n";
                    }
                    else{
                        for(long unsigned int i = 0;i < queue.size();i++){
                            std::cout << " " << std::get<0>(queue[i]);
                        }
                        std::cout << "]\n";
                    }
                    tempbursts.erase(std::get<0>(cpu));
                    if(tempbursts.size() == 0){
                        time += ptime + switcht;
                        for(auto it : io){
                            if(it.second <= time){
                                increment = false;
                                if(tempbursts[it.first]["cbursts"].size() != 0){
                                    if(queue.size() > 0){
                                        for(std::deque<std::tuple<char,double>>::iterator itr = queue.begin();itr != queue.end();itr++){
                                            if(std::get<1>(*itr) > tau[it.first] || queue.size() == 0 || ((std::get<1>(*itr) == tau[it.first]) && (it.first - 'A' < std::get<0>(*itr) - 'A'))){
                                                queue.insert(itr, std::make_tuple(it.first, tau[it.first]));
                                                break;
                                            }
                                        }
                                    }
                                    if(std::find(queue.begin(),queue.end(),std::make_tuple(it.first, tau[it.first])) == queue.end()){queue.push_back(std::make_tuple(it.first, tau[it.first]));}
                                    waitTimes[it.first] = std::make_tuple(std::get<0>(waitTimes[it.first]), it.second);
                                    if(time < 10000){std::cout << "time " << it.second << "ms: Process " << it.first << " completed I/O; added to ready queue [Q";
                                    if(queue.size() == 0){
                                        std::cout << " <empty>]\n";
                                    }
                                    else{
                                        for(long unsigned int i = 0;i < queue.size();i++){
                                            std::cout << " " << std::get<0>(queue[i]);
                                        }
                                        std::cout << "]\n";
                                    }
                                    }
                                }
                                else{
                                    tempbursts.erase(it.first);
                                }
                                io.erase(it.first);
                                break;
                            }
                        }
                        break;
                    } 
                }
            }
        }
        else{
            if(time == std::get<1>(cpu)){
                increment = false;
                if(time < 10000){std::cout << "time " << time << "ms: Process " << std::get<0>(cpu) << " (tau " << tau[std::get<0>(cpu)] << "ms) completed a CPU burst; " << tempbursts[std::get<0>(cpu)]["cbursts"].size() << " bursts to go [Q";
                if(queue.size() == 0){
                    std::cout << " <empty>]\n";
                }
                else{
                    for(long unsigned int i = 0;i < queue.size();i++){
                        std::cout << " " << std::get<0>(queue[i]);
                    }
                    std::cout << "]\n";
                }
                }
                double itime = tempbursts[std::get<0>(cpu)]["ibursts"].front();
                tempbursts[std::get<0>(cpu)]["ibursts"].pop_front();
                itime += time;
                if(newtau[std::get<0>(cpu)] != tau[std::get<0>(cpu)]){
                    if(time < 10000){std::cout << "time " << time << "ms: Recalculating tau for process " << std::get<0>(cpu) << ": old tau " << tau[std::get<0>(cpu)] << "ms ==> new tau " << newtau[std::get<0>(cpu)] << "ms [Q";
                    if(queue.size() == 0){
                    std::cout << " <empty>]\n";
                    }
                    else{
                        for(long unsigned int i = 0;i < queue.size();i++){
                            std::cout << " " << std::get<0>(queue[i]);
                        }
                        std::cout << "]\n";
                    }
                    }
                    tau[std::get<0>(cpu)] = newtau[std::get<0>(cpu)];
                }
                if(time < 10000){std::cout << "time " << time << "ms: Process " << std::get<0>(cpu) << " switching out of CPU; blocking on I/O until time " << itime + switcht << "ms [Q";
                if(queue.size() == 0){
                    std::cout << " <empty>]\n";
                }
                else{
                    for(long unsigned int i = 0;i < queue.size();i++){
                        std::cout << " " << std::get<0>(queue[i]);
                    }
                    std::cout << "]\n";
                }
                }
                io[std::get<0>(cpu)] = itime + switcht;
                time += switcht;
                for(auto it : io){
                            if(it.second <= time){
                                increment = false;
                                if(tempbursts[it.first]["cbursts"].size() != 0){
                                    if(queue.size() > 0){
                                        for(std::deque<std::tuple<char,double>>::iterator itr = queue.begin();itr != queue.end();itr++){
                                            if(std::get<1>(*itr) > tau[it.first] || queue.size() == 0 || ((std::get<1>(*itr) == tau[it.first]) && (it.first - 'A' < std::get<0>(*itr) - 'A'))){
                                                queue.insert(itr, std::make_tuple(it.first, tau[it.first]));
                                                break;
                                            }
                                        }
                                    }
                                    if(std::find(queue.begin(),queue.end(),std::make_tuple(it.first, tau[it.first])) == queue.end()){queue.push_back(std::make_tuple(it.first, tau[it.first]));}
                                    waitTimes[it.first] = std::make_tuple(std::get<0>(waitTimes[it.first]), it.second);
                                    if(time < 10000){std::cout << "time " << it.second << "ms: Process " << it.first << " completed I/O; added to ready queue [Q";
                                    if(queue.size() == 0){
                                        std::cout << " <empty>]\n";
                                    }
                                    else{
                                        for(long unsigned int i = 0;i < queue.size();i++){
                                            std::cout << " " << std::get<0>(queue[i]);
                                        }
                                        std::cout << "]\n";
                                    }
                                    }
                                }
                                else{
                                    tempbursts.erase(it.first);
                                }
                                io.erase(it.first);
                                break;
                            }
                        }
                cpu = std::make_tuple(NULL, 0);
            }
        }
        for(auto it : io){
            if(it.second == time){
                increment = false;
                if(tempbursts[it.first]["cbursts"].size() != 0){
                    std::tuple<char,double> ptup = std::make_tuple(it.first, tau[it.first]);
                    if(queue.size() > 0){
                        for(std::deque<std::tuple<char,double>>::iterator itr = queue.begin();itr != queue.end();itr++){
                            if(std::get<1>(*itr) > tau[it.first] || (std::get<1>(*itr) == tau[it.first] && (std::get<0>(*itr) - 'A') < (it.first - 'A'))){
                                queue.insert(itr, ptup);
                                break;
                            }
                        }
                    }
                    if(std::find(queue.begin(),queue.end(),ptup) == queue.end()){queue.push_back(ptup);}
                    waitTimes[it.first] = std::make_tuple(std::get<0>(waitTimes[it.first]), time);
                    if(time < 10000){std::cout << "time " << time << "ms: Process " << it.first << " (tau " << tau[it.first] << "ms) completed I/O; added to ready queue [Q";
                    if(queue.size() == 0){
                        std::cout << " <empty>]\n";
                    }
                    else{
                        for(long unsigned int i = 0;i < queue.size();i++){
                            std::cout << " " << std::get<0>(queue[i]);
                        }
                        std::cout << "]\n";
                    }
                }
                }
                else{
                    if(queue.size() == 0){
                        //std::cout << " <empty>]\n";
                    }
                    else{
                        for(long unsigned int i = 0;i < queue.size();i++){
                            //std::cout << " " << std::get<0>(queue[i]);
                        }
                        //std::cout << "]\n";
                    }
                    tempbursts.erase(it.first);
                }
                io.erase(it.first);
                break;
            }
        }
        if(increment){time++;} //if nothing interesting has happened, increment time
    }
    std::cout << "time " << time << "ms: Simulator ended for SJF [Q <empty>]\n";
    double totalwtime = 0;
    double cpuwtime = 0;
    double iowtime = 0;
    int i = 0;
    for(auto it : waitTimes){
        totalwtime += std::get<0>(it.second);
        if(i < Pcount - cpuCount){
            iowtime += std::get<0>(it.second);
        }
        else{
            cpuwtime += std::get<0>(it.second);
            }
        i++;
    }
    data["avgWtime"] = totalwtime / Bcount; 
    data["cpuavgWtime"] = cpuwtime / cpubcount;
    data["ioavgWtime"] = iowtime / iobcount;
    data["avgTtime"] = (btime + (switches * (switcht*2))+totalwtime) / Bcount;
    data["cpuavgTtime"] = (cbtime + (cpuswitches * (switcht*2))+cpuwtime) / cpubcount;
    data["ioavgTtime"] = (ibtime + (ioswitches * (switcht*2))+iowtime) / iobcount;
    data["switches"] = switches;
    data["cpuswitches"] = cpuswitches;
    data["ioswitches"] = ioswitches;
    return time;
}

int srt(std::map<char, std::map<std::string, std::deque<double>>> &bursts,std::map<std::string,double> &data,int Bcount, int Pcount, int tcs, double alpha, double lambda, int cpuCount, int cpubcount, int iobcount, double btime, double cbtime, double ibtime){
    std::map<char, std::map<std::string, std::deque<double>>> tempbursts(bursts);
    int time = 0;
    int switcht = tcs/2;
    std::cout << "time " << time << "ms: Simulator started for SRT [Q <empty>]\n";
    std::deque<std::tuple<char, double>> queue;
    std::deque<char> charqueue;
    std::vector<char> arrived;
    std::map<char,double> io;
    std::tuple<char,double,double> cpu;
    int switches = 0;
    int cpuswitches = 0;
    int ioswitches = 0;
    int p = 0;
    int cp = 0;
    int ip = 0;
    std::map<char,double> tau;
    std::map<char,double> newtau;
    std::map<char,double> taustart;
    std::map<char,std::tuple<double,double>> interrupted;
    std::map<char,double> unfinished;
    std::map<char,std::tuple<double,double>> waitTimes;
    //make interrupted a tuple to allow the original burst time to be stored
    //similarly, extend cpu tuple to allow original burst time to be stored so it can be added to resumed burst output
    cpu = std::make_tuple(NULL, 0, 0);
    bool failure = false;
    while(tempbursts.size() > 0){
        if(time > 2000000){
            //failure = true;
            //break;
        }
        bool increment = true; //only want to increment time manually if nothing interesting has happened
        //std::cout << time << "\n";
        if(arrived.size() < (long unsigned int)Pcount){//check if any new processes have arrived
            for(auto & it : tempbursts){
                double temp = it.second["itime"].front();
                //std::cout << temp << "\n";
                if(temp == time && std::find(arrived.begin(),arrived.end(),it.first) == arrived.end()){
                    increment = false;
                    std::tuple<char,double> ptup = std::make_tuple(it.first, 1/lambda);
                    if(std::find(queue.begin(),queue.end(),ptup) == queue.end()){queue.push_back(ptup);}
                    if(queue.size() > 1){std::sort(queue.begin(), queue.end());}
                    charqueue.push_back(it.first);
                    waitTimes[it.first] = std::make_tuple(0,time);
                    arrived.push_back(it.first);
                    tau[it.first] = 1/lambda;
                    if(std::get<0>(cpu) && 1/lambda < taustart[std::get<0>(cpu)] - time){
                        //preempt
                        p++;
                        if(std::get<0>(cpu) - 'A' < Pcount - cpuCount){ip++;}
                        else{cp++;}
                        if(time < 10000){std::cout << "time " << time << "ms: Process " << it.first << " (tau " << tau[it.first] << "ms) arrived; preempting " << std::get<0>(cpu) << " [Q";
                        if(unfinished.find(std::get<0>(cpu)) == unfinished.end()){
                            interrupted[std::get<0>(cpu)] = std::make_tuple(std::get<1>(cpu) - time, std::get<2>(cpu));
                            unfinished[std::get<0>(cpu)] = std::get<2>(cpu);
                        }
                        else{interrupted[std::get<0>(cpu)] = std::make_tuple(std::get<1>(cpu) - time, unfinished[std::get<0>(cpu)]);}
                        if(queue.size() == 0){
                        std::cout << " <empty>]\n";
                        }
                        else{
                            for(long unsigned int i = 0;i < queue.size();i++){
                                std::cout << " " << std::get<0>(queue[i]);
                            }
                        std::cout << "]\n";
                        }
                        }
                        std::tuple<char,double> prem = std::make_tuple(std::get<0>(cpu), taustart[std::get<0>(cpu)] - time);
                        //change tau to time - taustart and figure out how to store taustart
                        if(std::find(queue.begin(),queue.end(),prem) == queue.end()){queue.push_back(prem);}
                        if(queue.size() > 1){std::sort(queue.begin(),queue.end());}
                        charqueue.push_back(std::get<0>(prem));
                        waitTimes[std::get<0>(cpu)] = std::make_tuple(0,time);
                        char premname = std::get<0>(queue.front());
                        queue.pop_front();
                        for(auto j = charqueue.begin(); j != charqueue.end();j++){
                            if(*j == premname){
                                charqueue.erase(j);
                                break;
                            }
                        }
                        waitTimes[premname] = std::make_tuple(std::get<0>(waitTimes[premname]) + (time - std::get<1>(waitTimes[premname])), 0);
                        double premtime = tempbursts[premname]["cbursts"].front();
                        tempbursts[premname]["cbursts"].pop_front();
                        time += switcht * 2;
                        for(auto it : io){
                            if(it.second <= time){
                                increment = false;
                                if(tempbursts[it.first]["cbursts"].size() != 0){
                                    if(std::find(queue.begin(),queue.end(),ptup) == queue.end()){queue.push_back(ptup);}
                                    if(queue.size() > 1){std::sort(queue.begin(),queue.end());}
                                    charqueue.push_back(std::get<0>(ptup));
                                    waitTimes[it.first] = std::make_tuple(std::get<0>(waitTimes[it.first]), it.second);
                                    if(time > 10000){std::cout << "time " << it.second << "ms: Process " << it.first << " completed I/O; added to ready queue [Q";
                                    if(queue.size() == 0){
                                        std::cout << " <empty>]\n";
                                    }
                                    else{
                                        for(long unsigned int i = 0;i < queue.size();i++){
                                            std::cout << " " << std::get<0>(queue[i]);
                                        }
                                        std::cout << "]\n";
                                    }
                                    }
                                }
                                else{
                                    tempbursts.erase(it.first);
                                }
                                io.erase(it.first);
                                break;
                            }
                        }
                        std::cout << "time " << time << "ms: Process " << premname << " (tau " << tau[premname] << "ms) started using the CPU for " << premtime << "ms burst [Q";
                        if(queue.size() == 0){
                                std::cout << " <empty>]\n";
                            }
                            else{
                                for(long unsigned int i = 0;i < queue.size();i++){
                                    std::cout << " " << std::get<0>(queue[i]);
                                }
                                std::cout << "]\n";
                            }
                        
                        switches++;
                        if(premname - 'A' < Pcount - cpuCount){ioswitches++;}
                        else{cpuswitches++;}
                        double newTau = ceil((alpha * premtime) + ((1-alpha) * tau[premname]));
                        if(newTau != tau[premname]){
                            newtau[premname] = newTau;
                        }
                        cpu = std::make_tuple(premname, premtime + time, premtime);
                        taustart[premname] = time + tau[premname];
                        if(tempbursts[std::get<0>(cpu)]["cbursts"].size() == 0 && premtime <= std::get<0>(interrupted[premname])){
                            std::cout << "time " << time + premtime << "ms: Process " << std::get<0>(cpu) << " terminated [Q";
                            if(queue.size() == 0){
                                std::cout << " <empty>]\n";
                            }
                            else{
                                for(long unsigned int i = 0;i < queue.size();i++){
                                    std::cout << " " << std::get<0>(queue[i]);
                                }
                                std::cout << "]\n";
                            }
                            tempbursts.erase(std::get<0>(cpu));
                            if(tempbursts.size() == 0){
                                time += premtime + switcht;
                                for(auto it : io){
                            if(it.second <= time){
                                increment = false;
                                if(tempbursts[it.first]["cbursts"].size() != 0){
                    if(std::find(queue.begin(),queue.end(),ptup) == queue.end()){queue.push_back(ptup);}
                    if(queue.size() > 1){std::sort(queue.begin(),queue.end());}
                    charqueue.push_back(std::get<0>(ptup));
                                    waitTimes[it.first] = std::make_tuple(std::get<0>(waitTimes[it.first]), it.second);
                                    if(time < 10000){std::cout << "time " << it.second << "ms: Process " << it.first << " completed I/O; added to ready queue [Q";
                                    if(queue.size() == 0){
                                        std::cout << " <empty>]\n";
                                    }
                                    else{
                                        for(long unsigned int i = 0;i < queue.size();i++){
                                            std::cout << " " << std::get<0>(queue[i]);
                                        }
                                        std::cout << "]\n";
                                    }
                                    }
                                }
                                else{
                                    tempbursts.erase(it.first);
                                }
                                io.erase(it.first);
                                break;
                            }
                        }
                                break;
                            } 
                        }
                    }
                    else{
                        if(time < 10000){std::cout << "time " << time << "ms: Process " << it.first << " (tau " << tau[it.first] << "ms) arrived; added to ready queue [Q";
                        if(queue.size() == 0){
                        std::cout << " <empty>]\n";
                        }
                        else{
                            for(long unsigned int i = 0;i < queue.size();i++){
                                std::cout << " " << std::get<0>(queue[i]);
                            }
                        std::cout << "]\n";
                        }
                        }
                    }
                }
            }
        }
        //if cpu is empty, add process from queue
        if(!std::get<0>(cpu)){
            bool inter = false; //if resuming an interupted process, the print must be different
            if(queue.size() != 0){
                increment = false;
                char pname = std::get<0>(queue.front());
                waitTimes[pname] = std::make_tuple(std::get<0>(waitTimes[pname]) + (time - std::get<1>(waitTimes[pname])), 0);
                queue.pop_front();
                for(auto j = charqueue.begin(); j != charqueue.end();j++){
                            if(*j == pname){
                                charqueue.erase(j);
                                break;
                            }
                        }
                double ptime;
                if(interrupted.find(pname) == interrupted.end()){
                    ptime = tempbursts[pname]["cbursts"].front();
                    tempbursts[pname]["cbursts"].pop_front();
                }
                else{
                    ptime = std::get<0>(interrupted[pname]);
                    inter = true;
                }
                time += switcht;
                for(auto it : io){
                            if(it.second <= time){
                                increment = false;
                                if(tempbursts[it.first]["cbursts"].size() != 0){
                    if(std::find(queue.begin(),queue.end(),std::make_tuple(it.first, tau[it.first])) == queue.end()){queue.push_back(std::make_tuple(it.first, tau[it.first]));}
                    if(queue.size() > 1){std::sort(queue.begin(),queue.end());}
                    charqueue.push_back(it.first);
                                    waitTimes[it.first] = std::make_tuple(std::get<0>(waitTimes[it.first]), it.second);
                                    if(time < 10000){std::cout << "time " << it.second << "ms: Process " << it.first << " completed I/O; added to ready queue [Q";
                                    if(queue.size() == 0){
                                        std::cout << " <empty>]\n";
                                    }
                                    else{
                                        for(long unsigned int i = 0;i < queue.size();i++){
                                            std::cout << " " << std::get<0>(queue[i]);
                                        }
                                        std::cout << "]\n";
                                    }
                                    }
                                }
                                else{
                                    tempbursts.erase(it.first);
                                }
                                io.erase(it.first);
                                break;
                            }
                        }
                switches++;
                if(pname - 'A' < Pcount - cpuCount){ioswitches++;}
                else{cpuswitches++;}
                if(!inter){
                    if(time < 10000){std::cout << "time " << time << "ms: Process " << pname << " (tau " << tau[pname] << "ms) started using the CPU for " << ptime << "ms burst [Q";}
                    
                    }
                else{
                    if(time < 10000){std::cout << "time " << time << "ms: Process " << pname << "(tau " << tau[pname] << "ms) started using the CPU for remaining " << ptime << "ms of " << std::get<1>(interrupted[pname]) << "ms burst [Q";}}
                    
                double newTau = 0;
                if(!inter){newTau = ceil((alpha * ptime) + ((1-alpha) * tau[pname]));}
                else{
                    newTau = ceil((alpha * std::get<1>(interrupted[pname])) + ((1-alpha) * tau[pname]));
                    interrupted.erase(pname);
                }
                if(time < 10000){if(queue.size() == 0){
                    std::cout << " <empty>]\n";
                }
                else{
                    for(long unsigned int i = 0;i < queue.size();i++){
                        std::cout << " " << std::get<0>(queue[i]);
                    }
                    std::cout << "]\n";
                }
            }
                    if(newTau != tau[pname]){
                        newtau[pname] = newTau;
                    }
                
                cpu = std::make_tuple(pname, ptime + time, ptime);
                taustart[pname] = time + tau[pname];
                if(tempbursts[std::get<0>(cpu)]["cbursts"].size() == 0 && ptime <= std::get<0>(interrupted[pname])){
                    std::cout << "time " << time + ptime << "ms: Process " << std::get<0>(cpu) << " terminated [Q";
                    if(queue.size() == 0){
                        std::cout << " <empty>]\n";
                    }
                    else{
                        for(long unsigned int i = 0;i < queue.size();i++){
                            std::cout << " " << std::get<0>(queue[i]);
                        }
                        std::cout << "]\n";
                    }
                    tempbursts.erase(std::get<0>(cpu));
                    if(tempbursts.size() == 0){
                        time += ptime + switcht;
                        for(auto it : io){
                            if(it.second <= time){
                                increment = false;
                                if(tempbursts[it.first]["cbursts"].size() != 0){
                    if(std::find(queue.begin(),queue.end(),std::make_tuple(it.first, tau[it.first])) == queue.end()){queue.push_back(std::make_tuple(it.first, tau[it.first]));}
                    if(queue.size() > 1){std::sort(queue.begin(),queue.end());}
                    charqueue.push_back(it.first);
                                    waitTimes[it.first] = std::make_tuple(std::get<0>(waitTimes[it.first]), it.second);
                                    if(time < 10000){std::cout << "time " << it.second << "ms: Process " << it.first << " completed I/O; added to ready queue [Q";
                                    if(queue.size() == 0){
                                        std::cout << " <empty>]\n";
                                    }
                                    else{
                                        for(long unsigned int i = 0;i < queue.size();i++){
                                            std::cout << " " << std::get<0>(queue[i]);
                                        }
                                        std::cout << "]\n";
                                    }
                                    }
                                }
                                else{
                                    tempbursts.erase(it.first);
                                }
                                io.erase(it.first);
                                break;
                            }
                        }
                        break;
                    } 
                }
            }
        }
        else{
            if(time == std::get<1>(cpu)){
                increment = false;
                if(time < 10000){std::cout << "time " << time << "ms: Process " << std::get<0>(cpu) << " (tau " << tau[std::get<0>(cpu)] << "ms) completed a CPU burst; " << tempbursts[std::get<0>(cpu)]["cbursts"].size() << " bursts to go [Q";
                if(queue.size() == 0){
                    std::cout << " <empty>]\n";
                }
                else{
                    for(long unsigned int i = 0;i < queue.size();i++){
                        std::cout << " " << std::get<0>(queue[i]);
                    }
                    std::cout << "]\n";
                }
                }
                double itime = tempbursts[std::get<0>(cpu)]["ibursts"].front();
                tempbursts[std::get<0>(cpu)]["ibursts"].pop_front();
                itime += time;
                unfinished.erase(std::get<0>(cpu));
                if(newtau[std::get<0>(cpu)] != tau[std::get<0>(cpu)]){
                    if(time < 10000){std::cout << "time " << time << "ms: Recalculating tau for process " << std::get<0>(cpu) << ": old tau " << tau[std::get<0>(cpu)] << "ms ==> new tau " << newtau[std::get<0>(cpu)] << "ms [Q";
                    if(queue.size() == 0){
                    std::cout << " <empty>]\n";
                    }
                    else{
                        for(long unsigned int i = 0;i < queue.size();i++){
                            std::cout << " " << std::get<0>(queue[i]);
                        }
                        std::cout << "]\n";
                    }
                    }
                    tau[std::get<0>(cpu)] = newtau[std::get<0>(cpu)];
                }
                if(time < 10000){std::cout << "time " << time << "ms: Process " << std::get<0>(cpu) << " switching out of CPU; blocking on I/O until time " << itime + switcht << "ms [Q";
                if(queue.size() == 0){
                    std::cout << " <empty>]\n";
                }
                else{
                    for(long unsigned int i = 0;i < queue.size();i++){
                        std::cout << " " << std::get<0>(queue[i]);
                    }
                    std::cout << "]\n";
                }
                }
                io[std::get<0>(cpu)] = itime + switcht;
                time += switcht;
                for(auto it : io){
                            if(it.second <= time){
                                increment = false;
                                if(tempbursts[it.first]["cbursts"].size() != 0){
                    if(std::find(queue.begin(),queue.end(), std::make_tuple(it.first, tau[it.first])) == queue.end()){queue.push_back(std::make_tuple(it.first, tau[it.first]));}
                    if(queue.size() > 1){std::sort(queue.begin(),queue.end());}
                    charqueue.push_back(it.first);
                                    waitTimes[it.first] = std::make_tuple(std::get<0>(waitTimes[it.first]), it.second);
                                    if(time < 10000){std::cout << "time " << it.second << "ms: Process " << it.first << " completed I/O; added to ready queue [Q";
                                    if(queue.size() == 0){
                                        std::cout << " <empty>]\n";
                                    }
                                    else{
                                        for(long unsigned int i = 0;i < queue.size();i++){
                                            std::cout << " " << std::get<0>(queue[i]);
                                        }
                                        std::cout << "]\n";
                                    }
                                    }
                                }
                                else{
                                    tempbursts.erase(it.first);
                                }
                                io.erase(it.first);
                                break;
                            }
                        }
                cpu = std::make_tuple(NULL, 0, 0);
            }
        }
        for(auto it : io){
            if(it.second == time){
                increment = false;
                if(tempbursts[it.first]["cbursts"].size() != 0){
                    std::tuple<char,double> ptup = std::make_tuple(it.first, tau[it.first]);
                    if(std::find(queue.begin(),queue.end(),ptup) == queue.end()){queue.push_back(ptup);}
                    if(queue.size() > 1){std::sort(queue.begin(),queue.end());}
                    charqueue.push_back(std::get<0>(ptup));
                    waitTimes[it.first] = std::make_tuple(std::get<0>(waitTimes[it.first]), time);
                    if(std::get<0>(cpu) && tau[std::get<0>(ptup)] < taustart[std::get<0>(cpu)] - time){
                        //preempt
                        p++;
                        if(std::get<0>(cpu) - 'A' < Pcount - cpuCount){ip++;}
                        else{cp++;}
                        if(unfinished.find(std::get<0>(cpu)) == unfinished.end()){
                            interrupted[std::get<0>(cpu)] = std::make_tuple(std::get<1>(cpu) - time, std::get<2>(cpu));
                            unfinished[std::get<0>(cpu)] = std::get<2>(cpu);
                        }
                        else{interrupted[std::get<0>(cpu)] = std::make_tuple(std::get<1>(cpu) - time, unfinished[std::get<0>(cpu)]);}
                        if(time < 10000){std::cout << "time " << time << "ms: Process " << std::get<0>(ptup) << " (tau " << tau[std::get<0>(ptup)] << "ms) completed I/O; preempting " << std::get<0>(cpu) << " [Q";
                        if(queue.size() == 0){
                        std::cout << " <empty>]\n";
                        }
                        else{
                            for(long unsigned int i = 0;i < queue.size();i++){
                                std::cout << " " << std::get<0>(queue[i]);
                            }
                        std::cout << "]\n";
                        }
                        }
                        std::tuple<char,double> prem = std::make_tuple(std::get<0>(cpu), taustart[std::get<0>(cpu)] - time);
                        newtau[std::get<0>(cpu)] = taustart[std::get<0>(cpu)]-time;
                        char premname = std::get<0>(queue.front());
                        queue.pop_front();
                        for(auto j = charqueue.begin(); j != charqueue.end();j++){
                            if(*j == premname){
                                charqueue.erase(j);
                                break;
                            }
                        }
                        waitTimes[premname] = std::make_tuple(std::get<0>(waitTimes[premname]) + (time - std::get<1>(waitTimes[premname])), 0);
                        if(std::find(queue.begin(),queue.end(),prem) == queue.end()){queue.push_back(prem);}
                        if(queue.size() > 1){std::sort(queue.begin(),queue.end());}
                        charqueue.push_back(std::get<0>(prem));
                        waitTimes[std::get<0>(cpu)] = std::make_tuple(std::get<0>(waitTimes[std::get<0>(cpu)]), time);
                        double premtime = tempbursts[premname]["cbursts"].front();
                        tempbursts[premname]["cbursts"].pop_front();
                        time += switcht * 2 ;
                        for(auto it : io){
                            if(it.second <= time){
                                increment = false;
                                if(tempbursts[it.first]["cbursts"].size() != 0){
                    if(std::find(queue.begin(),queue.end(),ptup) == queue.end()){queue.push_back(ptup);}
                    if(queue.size() > 1){std::sort(queue.begin(),queue.end());}
                    charqueue.push_back(std::get<0>(ptup));
                                    waitTimes[it.first] = std::make_tuple(std::get<0>(waitTimes[it.first]), it.second);
                                    if(time < 10000){std::cout << "time " << it.second << "ms: Process " << it.first << " completed I/O; added to ready queue [Q";
                                    if(queue.size() == 0){
                                        std::cout << " <empty>]\n";
                                    }
                                    else{
                                        for(long unsigned int i = 0;i < queue.size();i++){
                                            std::cout << " " << std::get<0>(queue[i]);
                                        }
                                        std::cout << "]\n";
                                    }
                                    }
                                }
                                else{
                                    tempbursts.erase(it.first);
                                }
                                io.erase(it.first);
                                break;
                            }
                        }
                        switches++;
                        if(premname - 'A' < Pcount - cpuCount){ioswitches++;}
                        else{cpuswitches++;}
                        double newTau = ceil((alpha * premtime) + ((1-alpha) * tau[premname]));
                        if(newTau != tau[premname]){
                            newtau[premname] = newTau;
                        }
                        cpu = std::make_tuple(premname, premtime + time, premtime);
                        taustart[premname] = time + tau[premname];
                        if(time < 10000){std::cout << "time " << time << "ms: Process " << premname << " (tau " << tau[premname] << "ms) started using the CPU for " << premtime << "ms burst [Q";
                        if(queue.size() == 0){
                                std::cout << " <empty>]\n";
                            }
                            else{
                                for(long unsigned int i = 0;i < queue.size();i++){
                                    std::cout << " " << std::get<0>(queue[i]);
                                }
                                std::cout << "]\n";
                            }
                        }
                        if(tempbursts[std::get<0>(cpu)]["cbursts"].size() == 0 && premtime <= std::get<0>(interrupted[premname])){
                            std::cout << "time " << time + premtime << "ms: Process " << std::get<0>(cpu) << " terminated [Q";
                            if(queue.size() == 0){
                                std::cout << " <empty>]\n";
                            }
                            else{
                                for(long unsigned int i = 0;i < queue.size();i++){
                                    std::cout << " " << std::get<0>(queue[i]);
                                }
                                std::cout << "]\n";
                            }
                            tempbursts.erase(std::get<0>(cpu));
                            if(tempbursts.size() == 0){
                                time += premtime + switcht;
                                for(auto it : io){
                            if(it.second <= time){
                                increment = false;
                                if(tempbursts[it.first]["cbursts"].size() != 0){
                    if(std::find(queue.begin(),queue.end(),ptup) == queue.end()){queue.push_back(ptup);}
                    if(queue.size() > 1){std::sort(queue.begin(),queue.end());}
                    charqueue.push_back(std::get<0>(ptup));
                                    waitTimes[it.first] = std::make_tuple(std::get<0>(waitTimes[it.first]), it.second);
                                    if(time < 10000){std::cout << "time " << it.second << "ms: Process " << it.first << " completed I/O; added to ready queue [Q";
                                    if(queue.size() == 0){
                                        std::cout << " <empty>]\n";
                                    }
                                    else{
                                        for(long unsigned int i = 0;i < queue.size();i++){
                                            std::cout << " " << std::get<0>(queue[i]);
                                        }
                                        std::cout << "]\n";
                                    }
                                    }
                                }
                                else{
                                    tempbursts.erase(it.first);
                                }
                                io.erase(it.first);
                                break;
                            }
                        }
                                break;
                            } 
                        }
                    }
                    else{
                        if(time < 10000){std::cout << "time " << time << "ms: Process " << it.first << " (tau " << tau[it.first] << "ms) completed I/O; added to ready queue [Q";
                        if(queue.size() == 0){
                            std::cout << " <empty>]\n";
                        }
                        else{
                            for(long unsigned int i = 0;i < queue.size();i++){
                                std::cout << " " << std::get<0>(queue[i]);
                            }
                            std::cout << "]\n";
                        }
                        
                    }
                }
                }
                else{
                    if(queue.size() == 0){
                        //std::cout << " <empty>]\n";
                    }
                    else{
                        for(long unsigned int i = 0;i < queue.size();i++){
                            //std::cout << " " << std::get<0>(queue[i]);
                        }
                        //std::cout << "]\n";
                    }
                    tempbursts.erase(it.first);
                }
                io.erase(it.first);
                break;
            }
        }
        if(increment){time++;} //if nothing interesting has happened, increment time
    }
    if(failure){
        for(auto it : tempbursts){
            std::cout << "time " << time << "ms: Process " << it.first << " terminated [Q <empty>]\n";
        }
    }
    std::cout << "time " << time << "ms: Simulator ended for SRT [Q <empty>]\n";
    double totalwtime = 0;
    double cpuwtime = 0;
    double iowtime = 0;
    int i = 0;
    for(auto it : waitTimes){
        totalwtime += std::get<0>(it.second);
        if(i < Pcount - cpuCount){
            iowtime += std::get<0>(it.second);
        }
        else{
            cpuwtime += std::get<0>(it.second);
            }
        i++;
    }
    data["avgWtime"] = totalwtime / Bcount; 
    data["cpuavgWtime"] = cpuwtime / cpubcount;
    data["ioavgWtime"] = iowtime / iobcount;
    data["avgTtime"] = (btime + (switches * (switcht*2))+totalwtime) / Bcount;
    data["cpuavgTtime"] = (cbtime + (cpuswitches * (switcht*2))+cpuwtime) / cpubcount;
    data["ioavgTtime"] = (ibtime + (ioswitches * (switcht*2))+iowtime) / iobcount;
    data["switches"] = switches;
    data["preempt"] = p;
    data["cpupreempt"] = cp;
    data["iopreempt"] = ip;
    data["cpuswitches"] = cpuswitches;
    data["ioswitches"] = ioswitches;
    return time;
}

int rr(std::map<char, std::map<std::string, std::deque<double>>> &bursts,std::map<std::string,double> &data,int Bcount, int Pcount, int tcs, int tslice, double lambda, int cpuCount, int cpubcount, int iobcount, double btime, double cbtime, double ibtime){
    std::map<char, std::map<std::string, std::deque<double>>> tempbursts(bursts);
    int time = 0;
    int switcht = tcs/2;
    std::cout << "time " << time << "ms: Simulator started for RR [Q <empty>]\n";
    std::deque<char> queue;
    std::vector<char> arrived;
    std::map<char,double> io;
    std::tuple<char,double,double> cpu;
    std::map<char,std::tuple<double,double>> interrupted;
    std::map<char,double> unfinished;
    std::map<char,std::tuple<double,double>> waitTimes;
    int switches = 0;
    int cpuswitches = 0;
    int ioswitches = 0;
    int slicetimer = 0;
    int p = 0;
    int cp = 0;
    int ip = 0;
    cpu = std::make_tuple(NULL, 0, 0);
    bool failure = false;
    while(tempbursts.size() > 0){
        if(time > 2500000){
            //failure = true;
            //break;
            }
        bool increment = true; //only want to increment time manually if nothing interesting has happened
        //std::cout << time << "\n";
        if(arrived.size() < (long unsigned int)Pcount){//check if any new processes have arrived
            for(auto & it : tempbursts){
                double temp = it.second["itime"].front();
                //std::cout << temp << "\n";
                if(temp == time && std::find(arrived.begin(),arrived.end(),it.first) == arrived.end()){
                    increment = false;
                    queue.push_back(it.first);
                    waitTimes[it.first] = std::make_tuple(0,time);
                    arrived.push_back(it.first);
                    if(time < 10000){std::cout << "time " << time << "ms: Process " << it.first << " arrived; added to ready queue [Q";
                    for(long unsigned int i = 0;i < queue.size();i++){
                        std::cout << " " << queue[i];
                    }
                    std::cout << "]\n";
                }
                }
            }
        }
        //if cpu is empty, add process from queue
        if(!std::get<0>(cpu)){
            if(queue.size() != 0){
                increment = false;
                bool inter = false;
                char pname = queue.front();
                queue.pop_front();
                waitTimes[pname] = std::make_tuple(std::get<0>(waitTimes[pname]) + (time - std::get<1>(waitTimes[pname])), 0);
                double ptime;
                if(interrupted.find(pname) == interrupted.end()){
                    ptime = tempbursts[pname]["cbursts"].front();
                    tempbursts[pname]["cbursts"].pop_front();
                }
                else{
                    ptime = std::get<0>(interrupted[pname]);
                    inter = true;
                }
                time += switcht;
                for(auto it : io){
                            if(it.second <= time){
                                increment = false;
                                if(tempbursts[it.first]["cbursts"].size() != 0){
                                    queue.push_back(it.first);
                                    waitTimes[it.first] = std::make_tuple(std::get<0>(waitTimes[it.first]), it.second);
                                    if(time < 10000){std::cout << "time " << it.second << "ms: Process " << it.first << " completed I/O; added to ready queue [Q";
                                    if(queue.size() == 0){
                                        std::cout << " <empty>]\n";
                                    }
                                    else{
                                        for(long unsigned int i = 0;i < queue.size();i++){
                                            std::cout << " " << queue[i];
                                        }
                                        std::cout << "]\n";
                                    }
                                    }
                                }
                                else{
                                    tempbursts.erase(it.first);
                                }
                                io.erase(it.first);
                                break;
                            }
                        }
                switches++;
                if(pname - 'A' < Pcount - cpuCount){ioswitches++;}
                else{cpuswitches++;}
                if(!inter){if(time < 10000){std::cout << "time " << time << "ms: Process " << pname << " started using the CPU for " << ptime << "ms burst [Q";}}
                else{
                    if(time < 10000){std::cout << "time " << time << "ms: Process " << pname << " started using the CPU for remaining " << std::get<0>(interrupted[pname]) << "ms of " << std::get<1>(interrupted[pname]) << "ms burst [Q";}
                    interrupted.erase(pname);
                    }
                if(time < 10000){if(queue.size() == 0){
                    std::cout << " <empty>]\n";
                }
                else{
                    for(long unsigned int i = 0;i < queue.size();i++){
                        std::cout << " " << queue[i];
                    }
                    std::cout << "]\n";
                }
                }
                cpu = std::make_tuple(pname, ptime + time, ptime);
                slicetimer = time + tslice;
                if(tempbursts[std::get<0>(cpu)]["cbursts"].size() == 0 && ptime <= tslice){
                    std::cout << "time " << time + ptime << "ms: Process " << std::get<0>(cpu) << " terminated [Q";
                    if(queue.size() == 0){
                        std::cout << " <empty>]\n";
                    }
                    else{
                        for(long unsigned int i = 0;i < queue.size();i++){
                            std::cout << " " << queue[i];
                        }
                        std::cout << "]\n";
                    }
                    tempbursts.erase(std::get<0>(cpu));
                    if(tempbursts.size() == 0){
                        time += ptime + switcht;
                        for(auto it : io){
                            if(it.second <= time){
                                increment = false;
                                if(tempbursts[it.first]["cbursts"].size() != 0){
                                    queue.push_back(it.first);
                                    waitTimes[it.first] = std::make_tuple(std::get<0>(waitTimes[it.first]), it.second);
                                    if(time < 10000){std::cout << "time " << it.second << "ms: Process " << it.first << " completed I/O; added to ready queue [Q";
                                    if(queue.size() == 0){
                                        std::cout << " <empty>]\n";
                                    }
                                    else{
                                        for(long unsigned int i = 0;i < queue.size();i++){
                                            std::cout << " " << queue[i];
                                        }
                                        std::cout << "]\n";
                                    }
                                    }
                                }
                                else{
                                    tempbursts.erase(it.first);
                                }
                                io.erase(it.first);
                                break;
                            }
                        }
                        break;
                    } 
                }
            }
        }
        else{
            if(time == std::get<1>(cpu) || time == slicetimer){
                increment = false;
                bool slice = false;
                if(time == slicetimer && time != std::get<1>(cpu)){
                    slice = true;
                }
                if(!slice){
                if(time < 10000){std::cout << "time " << time << "ms: Process " << std::get<0>(cpu) << " completed a CPU burst; " << tempbursts[std::get<0>(cpu)]["cbursts"].size() << " bursts to go [Q";
                if(queue.size() == 0){
                    std::cout << " <empty>]\n";
                }
                else{
                    for(long unsigned int i = 0;i < queue.size();i++){
                        std::cout << " " << queue[i];
                    }
                    std::cout << "]\n";
                }
                }
                double itime = tempbursts[std::get<0>(cpu)]["ibursts"].front();
                tempbursts[std::get<0>(cpu)]["ibursts"].pop_front();
                itime += time;
                unfinished.erase(std::get<0>(cpu));
                if(time < 10000){std::cout << "time " << time << "ms: Process " << std::get<0>(cpu) << " switching out of CPU; blocking on I/O until time " << itime + switcht << "ms [Q";
                if(queue.size() == 0){
                    std::cout << " <empty>]\n";
                }
                else{
                    for(long unsigned int i = 0;i < queue.size();i++){
                        std::cout << " " << queue[i];
                    }
                    std::cout << "]\n";
                }
                }
                io[std::get<0>(cpu)] = itime + switcht;
                time += switcht;
                for(auto it : io){
                            if(it.second <= time){
                                increment = false;
                                if(tempbursts[it.first]["cbursts"].size() != 0){
                                    queue.push_back(it.first);
                                    waitTimes[it.first] = std::make_tuple(std::get<0>(waitTimes[it.first]), it.second);
                                    if(time < 10000){std::cout << "time " << it.second << "ms: Process " << it.first << " completed I/O; added to ready queue [Q";
                                    if(queue.size() == 0){
                                        std::cout << " <empty>]\n";
                                    }
                                    else{
                                        for(long unsigned int i = 0;i < queue.size();i++){
                                            std::cout << " " << queue[i];
                                        }
                                        std::cout << "]\n";
                                    }
                                    }
                                }
                                else{
                                    tempbursts.erase(it.first);
                                }
                                io.erase(it.first);
                                break;
                            }
                        }
                cpu = std::make_tuple(NULL, 0, 0);
                }
                else{
                    if(queue.size() == 0){
                        if(time < 10000){std::cout << "time " << time << "ms: Time slice expired; no preemption because ready queue is empty [Q <empty>]\n";}
                        slicetimer = time + tslice;
                        }
                    else{
                        bool inter = false;
                        //preempt
                        p++;
                        if(std::get<0>(cpu) - 'A' < Pcount - cpuCount){ip++;}
                        else{cp++;}
                        if(unfinished.find(std::get<0>(cpu)) == unfinished.end()){
                            interrupted[std::get<0>(cpu)] = std::make_tuple(std::get<1>(cpu) - time, std::get<2>(cpu));
                            unfinished[std::get<0>(cpu)] = std::get<2>(cpu);
                        }
                        else{interrupted[std::get<0>(cpu)] = std::make_tuple(std::get<1>(cpu) - time, unfinished[std::get<0>(cpu)]);}
                        if(time < 10000){std::cout << "time " << time << "ms: Time slice expired; preempting process " << std::get<0>(cpu) << " with " << std::get<1>(cpu) - time << "ms remaining [Q";
                        if(queue.size() == 0){
                        std::cout << " <empty>]\n";
                        }
                        else{
                            for(long unsigned int i = 0;i < queue.size();i++){
                                std::cout << " " << queue[i];
                            }
                        std::cout << "]\n";
                        }
                        }
                        char premname = queue.front();
                        queue.pop_front();
                        waitTimes[premname] = std::make_tuple(std::get<0>(waitTimes[premname]) + (time - std::get<1>(waitTimes[premname])), 0);
                        queue.push_back(std::get<0>(cpu));
                        waitTimes[std::get<0>(cpu)] = std::make_tuple(std::get<0>(waitTimes[std::get<0>(cpu)]), time);
                        double premtime;
                        if(interrupted.find(premname) == interrupted.end()){
                            premtime = tempbursts[premname]["cbursts"].front();
                            tempbursts[premname]["cbursts"].pop_front();
                        }
                        else{
                            premtime = std::get<0>(interrupted[premname]);
                            inter = true;
                        }
                        time += switcht * 2 ;
                        for(auto it : io){
                            if(it.second <= time){
                                increment = false;
                                if(tempbursts[it.first]["cbursts"].size() != 0){
                                    queue.push_back(it.first);
                                    waitTimes[it.first] = std::make_tuple(std::get<0>(waitTimes[it.first]), it.second);
                                    if(time < 10000){std::cout << "time " << it.second << "ms: Process " << it.first << " completed I/O; added to ready queue [Q";
                                    if(queue.size() == 0){
                                        std::cout << " <empty>]\n";
                                    }
                                    else{
                                        for(long unsigned int i = 0;i < queue.size();i++){
                                            std::cout << " " << queue[i];
                                        }
                                        std::cout << "]\n";
                                    }
                                    }
                                }
                                else{
                                    tempbursts.erase(it.first);
                                }
                                io.erase(it.first);
                                break;
                            }
                        }
                        switches++;
                        if(premname - 'A' < Pcount - cpuCount){ioswitches++;}
                        else{cpuswitches++;}
                        cpu = std::make_tuple(premname, premtime + time, premtime);
                        slicetimer = time + tslice;
                        if(!inter){if(time < 10000){std::cout << "time " << time << "ms: Process " << premname << " started using the CPU for " << premtime << "ms burst [Q";}}
                        else{
                            if(time < 10000){std::cout << "time " << time << "ms: Process " << premname << " started using the CPU for remaining " << std::get<0>(interrupted[premname]) << "ms of " << std::get<1>(interrupted[premname]) << "ms burst [Q";}
                            interrupted.erase(premname);
                            }
                        if(time < 10000){if(queue.size() == 0){
                                std::cout << " <empty>]\n";
                            }
                            else{
                                for(long unsigned int i = 0;i < queue.size();i++){
                                    std::cout << " " << queue[i];
                                }
                                std::cout << "]\n";
                            }
                        }
                        if(tempbursts[std::get<0>(cpu)]["cbursts"].size() == 0 && premtime <= tslice){
                            std::cout << "time " << time + premtime << "ms: Process " << std::get<0>(cpu) << " terminated [Q";
                            if(queue.size() == 0){
                                std::cout << " <empty>]\n";
                            }
                            else{
                                for(long unsigned int i = 0;i < queue.size();i++){
                                    std::cout << " " << queue[i];
                                }
                                std::cout << "]\n";
                            }
                            tempbursts.erase(std::get<0>(cpu));
                            if(tempbursts.size() == 0){
                                time += premtime + switcht;
                                for(auto it : io){
                            if(it.second <= time){
                                increment = false;
                                if(tempbursts[it.first]["cbursts"].size() != 0){
                                    queue.push_back(it.first);
                                    waitTimes[it.first] = std::make_tuple(std::get<0>(waitTimes[it.first]), it.second);
                                    if(time < 10000){std::cout << "time " << it.second << "ms: Process " << it.first << " completed I/O; added to ready queue [Q";
                                    if(queue.size() == 0){
                                        std::cout << " <empty>]\n";
                                    }
                                    else{
                                        for(long unsigned int i = 0;i < queue.size();i++){
                                            std::cout << " " << queue[i];
                                        }
                                        std::cout << "]\n";
                                    }
                                    }
                                }
                                else{
                                    tempbursts.erase(it.first);
                                }
                                io.erase(it.first);
                                break;
                            }
                        }
                                break;
                            } 
                        }
                    }
                }
            }
        }
        for(auto it : io){
            if(it.second == time){
                increment = false;
                if(tempbursts[it.first]["cbursts"].size() != 0){
                    queue.push_back(it.first);
                    waitTimes[it.first] = std::make_tuple(std::get<0>(waitTimes[it.first]), time);
                    if(time < 10000){std::cout << "time " << time << "ms: Process " << it.first << " completed I/O; added to ready queue [Q";
                    if(queue.size() == 0){
                        std::cout << " <empty>]\n";
                    }
                    else{
                        for(long unsigned int i = 0;i < queue.size();i++){
                            std::cout << " " << queue[i];
                        }
                        std::cout << "]\n";
                    }
                    }
                }
                else{
                    tempbursts.erase(it.first);
                }
                io.erase(it.first);
                break;
            }
        }
        if(increment){time++;} //if nothing interesting has happened, increment time
    }
    if(failure){
        for(auto it : tempbursts){
            std::cout << "time " << time << "ms: Process " << it.first << " terminated [Q <empty>]\n";
        }
    }
    std::cout << "time " << time << "ms: Simulator ended for RR [Q <empty>]\n";
    double totalwtime = 0;
    double cpuwtime = 0;
    double iowtime = 0;
    int i = 0;
    for(auto it : waitTimes){
        totalwtime += std::get<0>(it.second);
        if(i < Pcount - cpuCount){
            iowtime += std::get<0>(it.second);
        }
        else{
            cpuwtime += std::get<0>(it.second);
            }
        i++;
    }
    data["avgWtime"] = totalwtime / Bcount; 
    data["cpuavgWtime"] = cpuwtime / cpubcount;
    data["ioavgWtime"] = iowtime / iobcount;
    data["avgTtime"] = (btime + (switches * (switcht*2))+totalwtime) / Bcount;
    data["cpuavgTtime"] = (cbtime + (cpuswitches * (switcht*2))+cpuwtime) / cpubcount;
    data["ioavgTtime"] = (ibtime + (ioswitches * (switcht*2))+iowtime) / iobcount;
    data["switches"] = switches;
    data["preempt"] = p;
    data["cpupreempt"] = cp;
    data["iopreempt"] = ip;
    data["cpuswitches"] = cpuswitches;
    data["ioswitches"] = ioswitches;
    return time;
}

int main(int argc, char* argv[]){
    if(argc != 9){
        std::cerr << "ERROR: <Invalid number of arguments>" << std::endl;
        exit(1);
    }
    if((atoi(argv[1])) > 26 || atoi(argv[1]) <= 0){
        std::cerr << "ERROR: <Too many or not enough tings man>" << std::endl;
        exit(1);
    }
    int n = atoi(argv[1]);
    int ncpu = atoi(argv[2]);
    srand48(atoi(argv[3]));
    double lambda = atof(argv[4]);
    int ubound = atoi(argv[5]);
    int tcs = atoi(argv[6]);
    double alpha = atof(argv[7]);
    int tslice = atoi(argv[8]);
    std::cout << "<<< PROJECT PART I -- process set (n=" << n << ") with "<< ncpu  << " CPU-bound ";
    if(ncpu == 1){std::cout << "process >>>\n";}
    else{std::cout << "processes >>>\n";}
    std::map<char, std::map<std::string, std::deque<double>>> processBursts;
    for(int i = 0;i<n;i++){
        bool isIO = true;
        if(i >= (n-ncpu)){
            isIO = false;
            std::cout << "CPU-bound process ";
        }
        else{std::cout << "I/O-bound process ";}
        std::cout << (char)('A' + i) << ":";//firgure out int to ascii so you can print A-Z
        std::map<std::string, std::deque<double>> burstData = generateBursts(isIO, lambda, ubound);
        processBursts[(char)('A'+i)] = burstData;
    }
    double totalBtime = 0;
    double cpuBtime = 0;
    int cpucount = 0;
    int iocount = 0;
    double ioBtime = 0;
    int Bcount = 0;
    for(auto & it : processBursts){
        for(auto & i : it.second.at("cbursts")){
            if((it.first - 'A') < n-ncpu){
                ioBtime += i;
                iocount++;
            }
            else{
                cpuBtime += i;
                cpucount++;
            }
            Bcount++;
            totalBtime += i;
        }
    }
    double avgBtime = totalBtime/Bcount;
    //avgBtime = avgBtime/1000.0;
    double cpuavgBtime = cpuBtime/cpucount;
    double ioavgBtime = ioBtime/iocount;
    std::map<std::string, double> fcfsData;
    fcfsData["avgBtime"] = avgBtime;
    fcfsData["cpuavgBtime"] = cpuavgBtime;
    fcfsData["ioavgBtime"] = ioavgBtime;
    fcfsData["preempt"] = 0;
    std::cout << "\n" << "<<< PROJECT PART II -- t_cs=" << tcs << "ms; alpha=" << alpha << "; t_slice=" << tslice<< "ms >>>\n";
    std::ofstream outfile;
    outfile.open("simout.txt");
    int fcfsTime = fcfs(processBursts, fcfsData, Bcount, n, tcs, ncpu, cpucount, iocount, totalBtime, cpuBtime, ioBtime);
    double tempcpuUtil = ceil(1000.0*(totalBtime/fcfsTime * 100.0));
    fcfsData["cpuUtil"] = tempcpuUtil / 1000.0;
    outfile << "Algorithm FCFS\n";
    outfile << "-- CPU utilization: " << fcfsData["cpuUtil"] << "%\n";
    outfile << "-- average CPU burst time: " << fcfsData["avgBtime"] << " ms (" << fcfsData["cpuavgBtime"] << " ms/" << fcfsData["ioavgBtime"] << " ms)\n";
    outfile << "-- average wait time: " << fcfsData["avgWtime"] << " ms (" << fcfsData["cpuavgWtime"] << " ms/" << fcfsData["ioavgWtime"] << " ms)\n";
    outfile << "-- average turnaround time: " << fcfsData["avgTtime"] << " ms (" << fcfsData["cpuavgTtime"] << " ms/" << fcfsData["ioavgTtime"] << " ms)\n";
    outfile << "-- number of context switches: " << fcfsData["switches"] << " (" << fcfsData["cpuswitches"] << "/" << fcfsData["ioswitches"] << ")\n";
    outfile << "-- number of preemptions: " << fcfsData["preempt"] << " (0/0)\n";
    outfile << "\n";
    std::cout << "\n";
    std::map<std::string, double> sjfData;
    sjfData["avgBtime"] = avgBtime;
    sjfData["cpuavgBtime"] = cpuavgBtime;
    sjfData["ioavgBtime"] = ioavgBtime;
    sjfData["preempt"] = 0;
    int sjfTime = sjf(processBursts, sjfData, Bcount, n, tcs, alpha, lambda, ncpu, cpucount, iocount, totalBtime, cpuBtime, ioBtime);
    tempcpuUtil = ceil(1000.0*(totalBtime/sjfTime * 100.0));
    sjfData["cpuUtil"] = tempcpuUtil / 1000.0;
    outfile << "Algorithm SJF\n";
    outfile << "-- CPU utilization: " << sjfData["cpuUtil"] << "%\n";
    outfile << "-- average CPU burst time: " << sjfData["avgBtime"] << " ms (" << sjfData["cpuavgBtime"] << " ms/" << sjfData["ioavgBtime"] << " ms)\n";
    outfile << "-- average wait time: " << sjfData["avgWtime"] << " ms (" << sjfData["cpuavgWtime"] << " ms/" << sjfData["ioavgWtime"] << " ms)\n";
    outfile << "-- average turnaround time: " << sjfData["avgTtime"] << " ms (" << sjfData["cpuavgTtime"] << " ms/" << sjfData["ioavgTtime"] << " ms)\n";
    outfile << "-- number of context switches: " << sjfData["switches"] << " (" << sjfData["cpuswitches"] << "/" << sjfData["ioswitches"] << ")\n";
    outfile << "-- number of preemptions: " << sjfData["preempt"] << " (0/0)\n";
    outfile << "\n";
    std::cout << "\n";
    std::map<std::string, double> srtData;
    srtData["avgBtime"] = avgBtime;
    srtData["cpuavgBtime"] = cpuavgBtime;
    srtData["ioavgBtime"] = ioavgBtime;
    srtData["preempt"] = 0;
    int srtTime = srt(processBursts, srtData, Bcount, n, tcs, alpha, lambda, ncpu, cpucount, iocount, totalBtime, cpuBtime, ioBtime);
    tempcpuUtil = ceil(1000.0*(totalBtime/srtTime * 100.0));
    srtData["cpuUtil"] = tempcpuUtil / 1000.0;
    outfile << "Algorithm SRT\n";
    outfile << "-- CPU utilization: " << srtData["cpuUtil"] << "%\n";
    outfile << "-- average CPU burst time: " << srtData["avgBtime"] << " ms (" << srtData["cpuavgBtime"] << " ms/" << srtData["ioavgBtime"] << " ms)\n";
    outfile << "-- average wait time: " << srtData["avgWtime"] << " ms (" << srtData["cpuavgWtime"] << " ms/" << srtData["ioavgWtime"] << " ms)\n";
    outfile << "-- average turnaround time: " << srtData["avgTtime"] << " ms (" << srtData["cpuavgTtime"] << " ms/" << srtData["ioavgTtime"] << " ms)\n";
    outfile << "-- number of context switches: " << srtData["switches"] << " (" << srtData["cpuswitches"] << "/" << srtData["ioswitches"] << ")\n";
    outfile << "-- number of preemptions: " << srtData["preempt"] << " (0/0)\n";
    outfile << "\n";
    std::cout << "\n";
    std::map<std::string, double> rrData;
    rrData["avgBtime"] = avgBtime;
    rrData["cpuavgBtime"] = cpuavgBtime;
    rrData["ioavgBtime"] = ioavgBtime;
    rrData["preempt"] = 0;
    int rrTime = rr(processBursts, rrData, Bcount, n, tcs, tslice, lambda, ncpu, cpucount, iocount, totalBtime, cpuBtime, ioBtime);
    tempcpuUtil = ceil(1000.0*(totalBtime/rrTime * 100.0));
    rrData["cpuUtil"] = tempcpuUtil / 1000.0;
    outfile << "Algorithm RR\n";
    outfile << "-- CPU utilization: " << rrData["cpuUtil"] << "%\n";
    outfile << "-- average CPU burst time: " << rrData["avgBtime"] << " ms (" << rrData["cpuavgBtime"] << " ms/" << rrData["ioavgBtime"] << " ms)\n";
    outfile << "-- average wait time: " << rrData["avgWtime"] << " ms (" << rrData["cpuavgWtime"] << " ms/" << rrData["ioavgWtime"] << " ms)\n";
    outfile << "-- average turnaround time: " << rrData["avgTtime"] << " ms (" << rrData["cpuavgTtime"] << " ms/" << rrData["ioavgTtime"] << " ms)\n";
    outfile << "-- number of context switches: " << rrData["switches"] << " (" << rrData["cpuswitches"] << "/" << rrData["ioswitches"] << ")\n";
    outfile << "-- number of preemptions: " << rrData["preempt"] << " (0/0)\n";
    outfile << "\n";
}

bool operator<(std::tuple<char,double> a, std::tuple<char,double> b){
    if(std::get<1>(a) < std::get<1>(b)){return true;}
    return false;
}