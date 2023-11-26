////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief For-loop parallelization using threads
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


#include <functional>
#include <vector>
#include <future>


namespace yart
{
    namespace threads
    {
        /// @brief Parallelized for-loop using std futures
        /// @tparam T Numeric type of the iterable
        /// @param begin Range start value
        /// @param end Range end value (inclusive)
        /// @param func Function applied over the range
        template<typename T>
        void parallel_for(T begin, T end, std::function<void(T)> func)
        {
            // Internal wrapper function
            auto wrapper = [&func](T _begin, T _end) {
                for (T i = _begin; i < _end; ++i) {
                    func(i);
                }
            };

            uint32_t thread_num_hint = std::thread::hardware_concurrency();
            int thread_num = thread_num_hint ? static_cast<int>(thread_num_hint) : 8;

            T length = end - begin;
            const auto [batch_size, remainder] = std::div(length, thread_num);

            // launch threads
            std::vector<std::future<void>> threads(thread_num);
            if (remainder) {
                for(size_t i = 0; i < thread_num - 1; ++i) {
                    T start = begin + i * batch_size;
                    threads[i] = std::async(std::launch::async, wrapper, start, start + batch_size);
                }

                // Process the last segment with the remainder
                T start = begin + (thread_num - 1) * batch_size;
                threads[thread_num - 1] = std::async(std::launch::async, wrapper, start, end);
            } else {
                for(size_t i = 0; i < thread_num; ++i) {
                    T start = begin + i * batch_size;
                    threads[i] = std::async(std::launch::async, wrapper, start, start + batch_size);
                }
            }
            
            for (auto& thread : threads)
                thread.get();
        }

    } // namespace threads
} // namespace yart
