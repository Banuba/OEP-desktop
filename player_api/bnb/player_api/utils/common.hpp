#pragma once

#include <stdexcept>
#include <memory>

namespace bnb::player_api
{

    template <class T>
    void validate_not_null(const T& ptr)
    {
        if (ptr == nullptr) {
            throw std::runtime_error("Pointer cannot be `nullptr`.");
        }
    }

    template <std::size_t value>
    constexpr bool is_pow_of_two()
    {
        return (value != 0) && !(value & (value - 1));
    }

    template <std::size_t align, class T>
    T align_by(T x)
    {
        static_assert(is_pow_of_two<align>(), "Alignment must be a power of two.");
        constexpr auto align_bits = align - 1;
        return (x + align_bits) & ~align_bits;
    }

    template <class T>
    T align_by_8(T x)
    {
        return align_by<8>(x);
    }

    template <class T>
    T align_by_16(T x)
    {
        return align_by<16>(x);
    }

    template <class T>
    T align_by_32(T x)
    {
        return align_by<32>(x);
    }

    template <class T>
    T align_by_64(T x)
    {
        return align_by<64>(x);
    }

    template <std::size_t mem_align>
    inline uint8_t* aligned_alloc(std::size_t size)
    {
        // memory struct:      ...*-----------------------*---------------*------------------...---*
        //                        ^                       ^               ^                        ^
        //                (real ptr)           (shift to real ptr)   (aligned ptr)    (end of memory block)
        // (shift to real ptr) = (aligned ptr) - 8
        static_assert(is_pow_of_two<mem_align>() && mem_align >= 8, "Alignment must be a power of two and more than 8.");
        auto full_size = size + mem_align + 8;
        auto* real_ptr = std::malloc(full_size);
        constexpr auto align_bits = mem_align - 1;
        auto* aligned_ptr = reinterpret_cast<uint8_t*>(((reinterpret_cast<std::size_t>(real_ptr) + 8) + align_bits) & ~align_bits);
        auto* shift_to_real_ptr = reinterpret_cast<std::size_t*>(aligned_ptr - 8);
        *shift_to_real_ptr = static_cast<std::size_t>(aligned_ptr - reinterpret_cast<uint8_t*>(real_ptr));
        return aligned_ptr;
    }

    inline void aligned_dealloc(uint8_t* p)
    {
        auto shift = *reinterpret_cast<std::size_t*>(p - 8);
        auto* real_ptr = p - shift;
        std::free(real_ptr);
    }

} // namespace bnb::player_api
