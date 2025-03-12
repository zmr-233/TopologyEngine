/****************************************
 * FILE: Engine/Include/ECS/CompCache.hpp
 * 说明：实现多缓冲 版本号管理
 ****************************************/

#pragma once
#include <atomic>
#include <cstdint>
#include <functional>
#include <limits>
#include <mutex>
#include <optional>
#include <shared_mutex>

#include "CompType.hpp"

constexpr auto MEM_RELAXED  = std::memory_order_relaxed;
constexpr auto MEM_ACQUIRE  = std::memory_order_acquire;
constexpr auto MEM_RELEASE  = std::memory_order_release;
constexpr auto MEM_ACQ_REL  = std::memory_order_acq_rel;
constexpr auto MEM_SEQ_CST  = std::memory_order_seq_cst;
constexpr auto NULL_VERSION = 0;

using MAX_PARALLEL_TYPE          = uint8_t;
constexpr auto MAX_PARALLEL_SIZE = std::numeric_limits<MAX_PARALLEL_TYPE>::max();

template <typename T, uint8_t N>
class CompCache {
   public:
    /**
     * @brief 读操作返回的“句柄对象”
     *
     * - 在构造时会记录: 对应的缓存槽 slot、对 cache 的引用
     * - 在析构时会调用 cache.readDone(slot)，自动释放
     * - 通过 get() 可以拿到对底层 T 的引用
     */
    class ReadHandle {
       public:
        // 禁止拷贝，避免多次析构
        ReadHandle(const ReadHandle&)            = delete;
        ReadHandle& operator=(const ReadHandle&) = delete;

        // 允许移动
        ReadHandle(ReadHandle&& other) noexcept
            : m_cache(other.m_cache), m_slot(other.m_slot) {
            other.m_slot  = INVALID_SLOT;
            other.m_cache = nullptr;
        }
        ReadHandle& operator=(ReadHandle&& other) noexcept {
            if (this != &other) {
                // 先释放自己
                release();
                // 再接管
                m_cache       = other.m_cache;
                m_slot        = other.m_slot;
                other.m_slot  = INVALID_SLOT;
                other.m_cache = nullptr;
            }
            return *this;
        }

        ~ReadHandle() {
            release();
        }

        /**
         * @brief 返回对底层组件数据 T 的引用
         */
        T& get() {
            return m_cache->m_data[m_slot];
        }
        const T& get() const {
            return m_cache->m_data[m_slot];
        }

       private:
        static constexpr uint8_t INVALID_SLOT = 0xFF;

        friend class CompCache;
        ReadHandle(CompCache* cache, uint8_t slot)
            : m_cache(cache), m_slot(slot) {}

        void release() {
            if (m_cache && m_slot != INVALID_SLOT) {
                m_cache->readDone(m_slot);
                m_slot  = INVALID_SLOT;
                m_cache = nullptr;
            }
        }

       private:
        CompCache* m_cache = nullptr;
        uint8_t m_slot          = INVALID_SLOT;
    };
    /**
     * @brief 写操作返回的“句柄对象”
     *
     *  - 在构造时用 slot 抢占写标记（CAS 已经在 writeHandle() 中完成）
     *  - 在析构/commit时 更新 version & readable，并释放写标记
     */
    class WriteHandle {
       public:
        // 禁止拷贝
        WriteHandle(const WriteHandle&)            = delete;
        WriteHandle& operator=(const WriteHandle&) = delete;

        // 允许移动
        WriteHandle(WriteHandle&& other) noexcept
            : m_cache(other.m_cache), m_slot(other.m_slot), m_committed(other.m_committed) {
            other.m_slot      = INVALID_SLOT;
            other.m_cache     = nullptr;
            other.m_committed = true;  // 防止重复提交
        }
        WriteHandle& operator=(WriteHandle&& other) noexcept {
            if (this != &other) {
                release();
                m_cache           = other.m_cache;
                m_slot            = other.m_slot;
                m_committed       = other.m_committed;
                other.m_slot      = INVALID_SLOT;
                other.m_cache     = nullptr;
                other.m_committed = true;
            }
            return *this;
        }

        ~WriteHandle() {
            release();
        }

        // 拿到底层可写的 T 引用
        T& get() {
            return m_cache->m_data[m_slot];
        }
        const T& get() const {
            return m_cache->m_data[m_slot];
        }

        /**
         * @brief 主动提交本次写操作
         *
         * 如果不手动调用 commit()，则在析构时也会自动提交；
         * 但有些场景下，可能想在完成数据填充后立即提交。
         */
        void commit() {
            if (!m_committed && m_cache && m_slot != INVALID_SLOT) {
                auto newVer = m_cache->m_globalVersion.fetch_add(1, MEM_ACQ_REL) + 1;
                m_cache->m_version[m_slot].store(newVer, MEM_RELEASE);
                m_cache->m_readable[m_slot].store(true, MEM_RELEASE);
                m_committed = true;
            }
        }

       private:
        static constexpr uint8_t INVALID_SLOT = 0xFF;
        friend class CompCache;

        WriteHandle(CompCache* cache, uint8_t slot)
            : m_cache(cache), m_slot(slot), m_committed(false) {}

        /**
         * @brief 释放本 WriteHandle 所持的资源
         *
         * 如果还没提交，就自动提交；然后把 m_writing[slot] 设回 false。
         */
        void release() {
            if (m_cache && m_slot != INVALID_SLOT) {
                // 如果用户没有显式提交，那就自动提交一次
                commit();
                // 释放占用的写标记
                m_cache->m_writing[m_slot].store(false, MEM_RELEASE);

                m_slot  = INVALID_SLOT;
                m_cache = nullptr;
            }
        }

       private:
        CompCache* m_cache = nullptr;
        uint8_t m_slot          = INVALID_SLOT;
        bool m_committed        = true;
    };

   public:
    CompCache() {
        reset();
    }

    // =========== 读操作：返回 RAII 句柄 ==============
    ReadHandle read() {
        uint8_t bestSlot = findLatestReadable();
        if (bestSlot == INVALID_SLOT)
            // 没找到可读槽，返回一个无效句柄
            return ReadHandle(nullptr, INVALID_SLOT);
        // 增加阅读者计数
        m_reading[bestSlot].fetch_add(1, MEM_ACQUIRE);
        return ReadHandle(this, bestSlot);
    }

    // =========== 写操作：返回 WriteHandle ==============
    // 与“直接传入 newData”不同的是，这里只做 “拿一个可写槽 + 占用写标记” 的工作，
    // 真正的写入 T 数据由 WriteHandle 自行完成
    // 在 commit/析构时更新 version & readable.
    WriteHandle writeHandle() {
        uint8_t slot = findWritableSlot();
        if (slot == INVALID_SLOT)
            // 没有可写槽，抛异常或根据需求处理
            return WriteHandle(nullptr, INVALID_SLOT);

        // 抢占写标记
        bool expected = false;
        if (!m_writing[slot].compare_exchange_strong(expected, true, MEM_ACQ_REL))
            // 理论上 findWritableSlot() + CAS 成功率应该很高；失败时重试或抛异常
            return WriteHandle(nullptr, INVALID_SLOT);

        // 这里不设 readable，不更新版本号，等到 WriteHandle 的 commit/析构时再做
        return WriteHandle(this, slot);
    }

    // 保留类似“快写一次”的接口
    bool write(const T& newData) {
        auto handle = writeHandle();
        if (!handle.m_cache)
            // 找不到可写槽，处理异常
            return false;
        // 写入数据 -- RAII: 析构时自动 commit / 主动 handle.commit()
        handle.get() = newData;
        return true;
    }

   private:
    static constexpr uint8_t INVALID_SLOT = 0xFF;

    // =========== 读句柄析构回调 ===========
    void readDone(uint8_t slot) {
        m_reading[slot].fetch_sub(1, MEM_RELEASE);
    }

   private:
    // =========== 初始化 ===========
    void reset() {
        for (uint8_t i = 0; i < N; ++i) {
            m_version[i].store(0, MEM_RELAXED);
            m_writing[i].store(false, MEM_RELAXED);
            m_readable[i].store(false, MEM_RELAXED);
            m_reading[i].store(0, MEM_RELAXED);
        }
        m_globalVersion.store(0, MEM_RELAXED);
    }

    // =========== 查找最新可读槽 ===========
    uint8_t findLatestReadable() const {
        uint64_t bestVer = 0;
        uint8_t bestSlot = INVALID_SLOT;
        for (uint8_t i = 0; i < N; ++i)
            if (m_readable[i].load(MEM_ACQUIRE)) {
                auto ver = m_version[i].load(MEM_ACQUIRE);
                if (ver >= bestVer)
                    bestVer = ver, bestSlot = i;
            }

        return bestSlot;
    }

    // =========== 查找可写槽 ===========
    uint8_t findWritableSlot() {
        // 找到版本最旧、且 reading=0、且没有被其他线程写占用的槽
        uint64_t minVer = UINT64_MAX;
        uint8_t minSlot = INVALID_SLOT;

        for (uint8_t i = 0; i < N; ++i)
            if (!m_writing[i].load(MEM_ACQUIRE)) {
                auto rcount = m_reading[i].load(MEM_ACQUIRE);
                if (rcount == 0) {
                    auto ver = m_version[i].load(MEM_RELAXED);
                    if (ver < minVer)
                        minVer = ver, minSlot = i;
                }
            }
        return minSlot;
    }

   private:
    // 实际存储的组件数据
    std::array<T, N> m_data;
    // 每个缓冲槽的版本号
    std::array<std::atomic<uint64_t>, N> m_version;

    // 标记：是否正在写入
    std::array<std::atomic<bool>, N> m_writing;
    // 标记：是否可读
    std::array<std::atomic<bool>, N> m_readable;
    // 标记：是否正在读取(引用计数)
    std::array<std::atomic<MAX_PARALLEL_TYPE>, N> m_reading;

    // 全局维护的版本计数 写一次 +1
    std::atomic<uint64_t> m_globalVersion;
};
