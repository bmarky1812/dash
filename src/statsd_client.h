// Copyright (c) 2014-2017 Statoshi Developers
// Copyright (c) 2020-2023 The Dash Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_STATSD_CLIENT_H
#define BITCOIN_STATSD_CLIENT_H

#include <random.h>
#include <threadsafety.h>
#include <util/sock.h>

#include <string>
#include <memory>

static const bool DEFAULT_STATSD_ENABLE = false;
static const uint16_t DEFAULT_STATSD_PORT = 8125;
static const std::string DEFAULT_STATSD_HOST = "127.0.0.1";
static const std::string DEFAULT_STATSD_HOSTNAME = "";
static const std::string DEFAULT_STATSD_NAMESPACE = "";

// schedule periodic measurements, in seconds: default - 1 minute, min - 5 sec, max - 1h.
static const int DEFAULT_STATSD_PERIOD = 60;
static const int MIN_STATSD_PERIOD = 5;
static const int MAX_STATSD_PERIOD = 60 * 60;

namespace statsd {
class StatsdClient {
    public:
        explicit StatsdClient(const std::string& host, const std::string& nodename, uint16_t port, const std::string& ns,
                              bool enabled);

    public:
        int inc(const std::string& key, float sample_rate = 1.f);
        int dec(const std::string& key, float sample_rate = 1.f);
        int count(const std::string& key, int64_t value, float sample_rate = 1.f);
        int gauge(const std::string& key, int64_t value, float sample_rate = 1.f);
        int gaugeDouble(const std::string& key, double value, float sample_rate = 1.f);
        int timing(const std::string& key, int64_t ms, float sample_rate = 1.f);

    public:
        /**
         * (Low Level Api) manually send a message
         * which might be composed of several lines.
         */
        int send(const std::string& message);

        /* (Low Level Api) manually send a message
         * type = "c", "g" or "ms"
         */
        int send(std::string key, int64_t value,
                const std::string& type, float sample_rate);
        int sendDouble(std::string key, double value,
                const std::string& type, float sample_rate);

    protected:
        static void cleanup(std::string& key);

    private:
        bool ShouldSend(float sample_rate);

    private:
        mutable Mutex cs;
        mutable FastRandomContext insecure_rand GUARDED_BY(cs);

        bool m_init{false};
        std::unique_ptr<Sock> m_sock{nullptr};
        std::pair<struct sockaddr_storage, socklen_t> m_server{{}, sizeof(struct sockaddr_storage)};

        const uint16_t m_port;
        const std::string m_host;
        const std::string m_nodename;
        const std::string m_ns;
};
} // namespace statsd

extern std::unique_ptr<statsd::StatsdClient> g_stats_client;

#endif // BITCOIN_STATSD_CLIENT_H
