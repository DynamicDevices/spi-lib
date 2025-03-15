/* ============================================================================
 ** Copyright (C) 2014-2021 Infineon Technologies AG
 ** All rights reserved.
 ** ===========================================================================
 ** This software contains proprietary information of Infineon Technologies AG.
 ** Passing on and copying of this software, and communication of its contents
 ** is not permitted without Infineon's prior written authorisation.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 ** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 ** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 ** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 ** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 ** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 ** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 ** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 ** POSSIBILITY OF SUCH DAMAGE.
 ** ===========================================================================
 */
/**
 * @file UltrasonicDevice.hpp
 *
 * @brief Ultrasonic Device Access
 * 
 */

#ifndef SINGLE_READER_SINGLE_WRITER_RING_BUFFER_HPP
#define SINGLE_READER_SINGLE_WRITER_RING_BUFFER_HPP

#include <cstdint>

#include <vector>
#include <atomic>
#include <thread>
#include <functional>

/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
*/

template<class T>
class SingleReaderSingleWriterRingBuffer
{
    std::atomic<size_t> m_rd_offset;
    std::atomic<size_t> m_wr_offset;
    std::vector<T> m_data;

public:
    SingleReaderSingleWriterRingBuffer() = default;

    void reset()
    {
        m_rd_offset.store(0u);
        m_wr_offset.store(0u);
    }

    void resize(const size_t new_size)
    {
        reset();
        m_data.resize(new_size);
    }

    void resize(const size_t new_size,
        std::function<void(T &)> init_fn)
    {
        resize(new_size);

        for (auto &d : m_data)
            init_fn(d);
    }

    size_t size() const
    {
        return m_data.size();
    }

    bool try_push_fn(std::function<void(T &)> fn)
    {
        const auto rd_offset = m_rd_offset.load();
        const auto wr_offset = m_wr_offset.load();
        if (wr_offset - rd_offset >= m_data.size())
            return false;

        fn(m_data[wr_offset % m_data.size()]);

        m_wr_offset.store(wr_offset + 1u);
        return true;
    }

    bool try_push(const T& value)
    {
        return try_push_fn([&value](T& dest) { dest = value; });
    }

    bool try_pop_fn(std::function<void(T &)> fn)
    {
        const auto rd_offset = m_rd_offset.load();
        const auto wr_offset = m_wr_offset.load();
        if (rd_offset == wr_offset)
            return false;

        fn(m_data[rd_offset % m_data.size()]);

        m_rd_offset.store(rd_offset + 1u);
        return true;
    }

    bool try_pop(T& dest)
    {
        return try_pop_fn([&dest](T& value) { dest = value; });
    }

    bool try_pop()
    {
        return try_pop_fn([](T& value) {});
    }

    size_t fill()
    {
        return m_wr_offset.load() - m_rd_offset.load();
    }

    void wait_fill(const size_t num_entries)
    {
        using namespace std::chrono_literals;
        while (fill() < num_entries) {
            std::this_thread::sleep_for(100us);
        }
    }

    T* peek(size_t index = 0)
    {
        if (fill() > index)
            return &m_data[(m_rd_offset.load() + index) % m_data.size()];
        else
            return nullptr;
    }

};

#endif