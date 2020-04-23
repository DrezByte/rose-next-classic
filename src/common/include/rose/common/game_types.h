#pragma once

#include <type_traits>

namespace Rose::Common {

enum class Gender {
    Male,
    Female,
};

template<typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
Gender
gender_from(T i) {
    T low = static_cast<T>(Gender::Male);
    T high = static_cast<T>(Gender::Female);

    if (i <= low || i > high) {
        return Gender::Male;
    }
    return static_cast<Gender>(i);
}

enum class Job {
    Visitor = 0,

    Soldier = 111,
    Knight = 121,
    Champion = 122,

    Muse = 211,
    Mage = 221,
    Cleric = 222,

    Hawker = 311,
    Raider = 321,
    Scout = 322,

    Dealer = 411,
    Bourg = 421,
    Artisan = 422,
};

template<typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type>
Job
job_from(T i) {
    switch (i) {
        case 111:
            return Job::Soldier;
        case 121:
            return Job::Knight;
        case 122:
            return Job::Champion;
        case 211:
            return Job::Muse;
        case 221:
            return Job::Mage;
        case 222:
            return Job::Cleric;
        case 311:
            return Job::Hawker;
        case 321:
            return Job::Raider;
        case 322:
            return Job::Scout;
        case 411:
            return Job::Dealer;
        case 421:
            return Job::Bourg;
        case 422:
            return Job::Artisan;
        default:
            return Job::Visitor;
    }
}

inline bool
is_first_job(Job job) {
    return job == Job::Muse || job == Job::Hawker || job == Job::Dealer || job == Job::Soldier;
}

} // namespace Rose::Common