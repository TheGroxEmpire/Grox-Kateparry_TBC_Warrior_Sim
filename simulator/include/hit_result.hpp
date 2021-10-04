#ifndef WOW_SIMULATOR_HIT_RESULT_HPP
#define WOW_SIMULATOR_HIT_RESULT_HPP

enum class Hit_result:uint8_t
{
    miss = 0x1,
    dodge = 0x2,
    glancing = 0x4,
    crit = 0x8,
    hit = 0x10,

    TBD = 0,
};

#endif // WOW_SIMULATOR_HIT_RESULT_HPP
