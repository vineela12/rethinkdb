#ifndef __BUFFER_CACHE_MIRRORED_CONFIG_HPP__
#define	__BUFFER_CACHE_MIRRORED_CONFIG_HPP__

#include "config/args.hpp"
#include "utils.hpp"

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/access.hpp>

#define NEVER_FLUSH -1

/* Configuration for the cache (it can all change from run to run) */

struct mirrored_cache_config_t {
    mirrored_cache_config_t() {
        max_size = (long long int)(8 * MEGABYTE); // This should be overwritten
            // at a place where more information about the system and use of the cache is available.
        wait_for_flush = false;
        flush_timer_ms = DEFAULT_FLUSH_TIMER_MS;
        max_dirty_size = DEFAULT_UNSAVED_DATA_LIMIT;
        flush_dirty_size = 0;
        flush_waiting_threshold = DEFAULT_FLUSH_WAITING_THRESHOLD;
        max_concurrent_flushes = DEFAULT_MAX_CONCURRENT_FLUSHES;
        io_priority_reads = CACHE_READS_IO_PRIORITY;
        io_priority_writes = CACHE_WRITES_IO_PRIORITY;
    }

    // Max amount of memory that will be used for the cache, in bytes.
    long long max_size;

    // If wait_for_flush is true, then write operations will not return until after the data is
    // safely sitting on the disk.
    bool wait_for_flush;

    // flush_timer_ms is how long (in milliseconds) the cache will allow modified data to sit in
    // memory before flushing it to disk. If it is NEVER_FLUSH, then data will be allowed to sit in
    // memory indefinitely.
    int flush_timer_ms;

    // max_dirty_size is the most unsaved data that is allowed in memory before the cache will
    // throttle write transactions. It's in bytes.
    long long max_dirty_size;

    // flush_dirty_size is the amount of unsaved data that will trigger an immediate flush. It
    // should be much less than max_dirty_size. It's in bytes.
    long long flush_dirty_size;

    // flush_waiting_threshold is the maximal number of transactions which can wait
    // for a sync before a flush gets triggered on any single slice. As transactions only wait for
    // sync with wait_for_flush enabled, this option plays a role only then.
    int flush_waiting_threshold;

    // If wait_for_flush is true, concurrent flushing can be used to reduce the latency
    // of each single flush. max_concurrent_flushes controls how many flushes can be active
    // on a specific slice at any given time.
    int max_concurrent_flushes;

    // per-cache priorities used for i/o accounts
    // each cache uses two IO accounts:
    // one account for writes, and one account for reads.
    int io_priority_reads;
    int io_priority_writes;

    friend class boost::serialization::access;
    template<class Archive> void serialize(Archive &ar, UNUSED const unsigned int version) {
        ar & max_size;
        ar & wait_for_flush;
        ar & flush_timer_ms;
        ar & max_dirty_size;
        ar & flush_dirty_size;
        ar & flush_waiting_threshold;
        ar & max_concurrent_flushes;
        ar & io_priority_reads;
        ar & io_priority_writes;
    }
};

/* This part of the serializer is part of the on-disk serializer_config_block and can
only be set at creation time of a database */

struct mirrored_cache_static_config_t {
    mirrored_cache_static_config_t() {
        n_patch_log_blocks = 0;
    }

    // How many blocks of each slice are allocated to the patch log?
    int32_t n_patch_log_blocks;
};

#endif
