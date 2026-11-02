                                                                    #include "RecursionToTheRescue.h"
#include "map.h"
#include <climits>
#include <iostream>
#include "Disasters.h"
using namespace std;

/* * * * Doctors Without Orders * * * */

/**
 * Given a list of doctors and a list of patients, determines whether all the patients can
 * be seen. If so, this function fills in the schedule outparameter with a map from doctors
 * to the set of patients that doctor would see.
 *
 * @param doctors  The list of the doctors available to work.
 * @param patients The list of the patients that need to be seen.
 * @param schedule An outparameter that will be filled in with the schedule, should one exist.
 * @return Whether or not a schedule was found.
 */
// void ifcombineOneDoctor(int hoursfree, Vector<Patient>& patients, int sum,
//                         Vector<Vector<Patient>>& patientsbuffers, Vector<Patient>& patientsbuffer, int index) {
//     if (sum <= hoursfree) {
//         int len = patients.size();
//         for (int i = index; i < len; i++) {
//             int hoursneeded = patients[i].hoursNeeded;
//             patientsbuffer.add(patients[i]);
//             sum += hoursneeded;
//             if (sum <= hoursfree) {
//                 patientsbuffers.add(patientsbuffer);
//             }
//             ifcombineOneDoctor(hoursfree, patients, sum, patientsbuffers, patientsbuffer, i + 1);
//             sum -= hoursneeded;
//             patientsbuffer.remove(patientsbuffer.size() - 1);
//         }
//     }
// }

// void kicktheName(Vector<Patient>& patients, const Vector<Patient>& patientsbuffer) {
//     int len1 = patients.size();
//     int len2 = patientsbuffer.size();
//     for (int i = len1 - 1; i >= 0; i--) {
//         for (int j = len2 - 1; j >= 0; j--) {
//             if (patients[i].name == patientsbuffer[j].name) {
//                 patients.remove(i);
//                 break;
//             }
//             if (patientsbuffer.isEmpty()) {
//                 return;
//             }
//         }
//     }
// }

// bool canAllPatientsBeSeenHelper(const Vector<Doctor>& doctors,
//                                 Vector<Patient>& patients,
//                                 Map<string, Set<string>>& schedule, int index) {
//     Vector<Vector<Patient>> patientsbuffers;
//     Vector<Patient> patientsbuffer;
//     if (patients.isEmpty()) {
//         return true;
//     } else if (index != doctors.size()){
//         ifcombineOneDoctor(doctors[index].hoursFree, patients, 0, patientsbuffers, patientsbuffer, 0);
//         int len1 = patientsbuffers.size();
//         for (int i = 0; i < len1; i++) {
//             patientsbuffer = patientsbuffers[i];
//             kicktheName(patients, patientsbuffer);
//             int len2 = patientsbuffer.size();
//             Set<string> s;
//             for (int j = 0; j < len2; j++) {
//                 s.add(patientsbuffer[j].name);
//             }
//             schedule[doctors[index].name] = s;
//             if(canAllPatientsBeSeenHelper(doctors, patients, schedule, index + 1)) {
//                 return true;
//             }
//             schedule.remove(doctors[index].name);
//             patients += patientsbuffer;
//         }
//     }
//     return false;
// }


bool canAllPatientsBeSeenHelper(Vector<Doctor>& doctors, const Vector<Patient>& patients,
                                Map<string, Set<string>>& schedule, int index) {
    if (index >= patients.size()) {
        return true;
    }
    int len = doctors.size();
    for (int i= 0; i < len; i++) {
        if (doctors[i].hoursFree >= patients[index].hoursNeeded) {
            doctors[i].hoursFree -= patients[index].hoursNeeded;
            schedule[doctors[i].name].add(patients[index].name);
            if(canAllPatientsBeSeenHelper(doctors, patients, schedule, index + 1)) {
                return true;
            }
            doctors[i].hoursFree += patients[index].hoursNeeded;
            schedule[doctors[i].name].remove(patients[index].name);
        }
    }
    return false;
}

bool canAllPatientsBeSeen(const Vector<Doctor>& doctors,
                          const Vector<Patient>& patients,
                          Map<string, Set<string>>& schedule) {
    Vector<Doctor> doctors1 = doctors;
    return canAllPatientsBeSeenHelper(doctors1, patients, schedule, 0);
}

/* * * * Disaster Planning * * * */

/**
 * Given a transportation grid for a country or region, along with the number of cities where disaster
 * supplies can be stockpiled, returns whether it's possible to stockpile disaster supplies in at most
 * the specified number of cities such that each city either has supplies or is connected to a city that
 * does.
 * <p>
 * This function can assume that every city is a key in the transportation map and that roads are
 * bidirectional: if there's a road from City A to City B, then there's a road from City B back to
 * City A as well.
 *
 * @param roadNetwork The underlying transportation network.
 * @param numCities   How many cities you can afford to put supplies in.
 * @param locations   An outparameter filled in with which cities to choose if a solution exists.
 * @return Whether a solution exists.
 */

void initializeLocationBool(Map<string, int>& locationbool, const Map<string, Set<string>>& roadNetwork) {
    for (string p: roadNetwork) {
        locationbool[p] = 0;
    }
}

void addcoveragrCities(const Map<string, Set<string>>& roadNetwork,
                       Map<string, int>& locationbool, int& remainCities, string city) {
    Set<string> tempt = roadNetwork[city];
    tempt.add(city);
    for (string s : tempt) {
        if (locationbool[s] == 0) {
            remainCities--;
        }
        locationbool[s]++;
    }
}

void removecoverage(const Map<string, Set<string>>& roadNetwork,
                    Map<string, int>& locationbool, int& remainCities, string city) {
    Set<string> tempt = roadNetwork[city];
    tempt.add(city);
    for (string s : tempt) {
        if (locationbool[s] == 1) {
            remainCities++;
        }
        locationbool[s]--;
    }
}

void canBeMadeDisasterReadyHelper(const Map<string, Set<string>>& roadNetwork, int& remainCities,
                                  int numCities, Map<string, int>& locationbool,
                                  Set<string>& locations, Set<string>& bestlocations) {
    if (remainCities == 0) {
        if (bestlocations.isEmpty() || locations.size() < bestlocations.size()) {
            bestlocations = locations;
        }
    } else if (locations.size() < numCities && (bestlocations.isEmpty() || locations.size() < bestlocations.size())) {
        string target;
        for (string p: roadNetwork) {
            if (locationbool[p] == 0) {
                target = p;
                break;
            }
        }
        Set<string> tempset = roadNetwork[target];
        tempset.add(target);
        for (string s : tempset) {
            addcoveragrCities(roadNetwork, locationbool, remainCities, s);
            locations.add(s);
            canBeMadeDisasterReadyHelper(roadNetwork, remainCities, numCities, locationbool, locations, bestlocations);
            removecoverage(roadNetwork, locationbool, remainCities, s);
            locations.remove(s);
        }
    }
}

bool canBeMadeDisasterReady(const Map<string, Set<string>>& roadNetwork,
                            int numCities,
                            Set<string>& locations) {
    Map<string, int> locationbool;
    initializeLocationBool(locationbool, roadNetwork);
    Set<string> bestlocations;
    int remain = roadNetwork.size();
    canBeMadeDisasterReadyHelper(roadNetwork, remain, numCities, locationbool, locations, bestlocations);
    locations = bestlocations;
    if (bestlocations.isEmpty()) {
        return false;
    } else {
        return true;
    }
}


/* * * * Winning the Election * * * */

/**
 * Given a list of the states in the election, including their popular and electoral vote
 * totals, and the number of electoral votes needed, as well as the index of the lowest-indexed
 * state to consider, returns information about how few popular votes you'd need in order to
 * win that at least that many electoral votes.
 *
 * @param electoralVotesNeeded the minimum number of electoral votes needed
 * @param states All the states in the election (plus DC, if appropriate)
 * @param minStateIndex the lowest index in the states Vector that should be considered
 */

MinInfo minPopularVoteToGetAtLeastHelper(int electoralVotesNeeded, const Vector<State>& states, Map<pair<int, int>,MinInfo>& mono,
                                      int minStateIndex) {
    if (electoralVotesNeeded <= 0) {
        return {0, {}};
    }
    if (mono.containsKey({electoralVotesNeeded, minStateIndex})) {
        return mono[{electoralVotesNeeded, minStateIndex}];
    }
    if (minStateIndex == states.size() && electoralVotesNeeded > 0) {
        return {INT_MAX, {}};
    }
    MinInfo oneinclude = minPopularVoteToGetAtLeastHelper(electoralVotesNeeded - states[minStateIndex].electoralVotes,
                                               states, mono, minStateIndex + 1);
    if(oneinclude.popularVotesNeeded != INT_MAX){
        oneinclude.popularVotesNeeded +=states[minStateIndex].popularVotes / 2 + 1;
        oneinclude.statesUsed.add(states[minStateIndex]);
    }
    MinInfo oneexclude = minPopularVoteToGetAtLeastHelper(electoralVotesNeeded, states, mono, minStateIndex + 1);
    if(oneinclude.popularVotesNeeded > oneexclude.popularVotesNeeded) {
        mono[{electoralVotesNeeded, minStateIndex}] = oneexclude;
        return oneexclude;
    } else {
        mono[{electoralVotesNeeded, minStateIndex}] = oneinclude;
        return oneinclude;
    }
}

// MinInfo minPopularVoteToGetAtLeast(int electoralVotesNeeded, const Vector<State>& states, int minStateIndex) {
    // MinInfo info;
    // Map<pair<int, int>,MinInfo> mono;
    // return minPopularVoteToGetAtLeastHelper(electoralVotesNeeded, states, mono, minStateIndex, info);
// }

/**
 * Given a list of all the states in an election, including their popular and electoral vote
 * totals, returns information about how few popular votes you'd need to win in order to win
 * the presidency.
 *
 * @param states All the states in the election (plus DC, if appropriate)
 * @return Information about how few votes you'd need to win the election.
 */
MinInfo minPopularVoteToWin(const Vector<State>& states) {
    int electoralVotesNeeded = 0;
    for(State one : states) {
        electoralVotesNeeded += one.electoralVotes;
    }
    Map<pair<int, int>,MinInfo> mono{};
    return minPopularVoteToGetAtLeastHelper(electoralVotesNeeded / 2 + 1, states, mono, 0);
}
