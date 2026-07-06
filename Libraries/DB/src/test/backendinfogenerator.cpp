#include "backendinfogenerator.hpp"

#include <cstdint>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace AITest {

class BackendInfoGenerator::Impl {
public:
    Impl() : m_rng(std::random_device{}()) {}

    std::vector<DBRecords::AIBackendInfo> generate(int count) const {
        if (count < 1 || count > 100) {
            throw std::out_of_range("count must be between 1 and 100");
        }

        std::vector<DBRecords::AIBackendInfo> result;
        result.reserve(static_cast<size_t>(count));

        for (int i = 0; i < count; ++i) {
            DBRecords::AIBackendInfo info;

            // ID and token: 64‑character hex strings
            info.setId(generateHex(64));
            info.setToken(generateHex(64));

            // Type: random from the enum (excluding SYS_Devtype_max)
            info.setType(randomDeviceType());

            // IP: random private address (192.168.x.y)
            info.setIp(generatePrivateIp());

            // Port: ephemeral range
            info.setPort(randomPort());

            // Display name: type + location + unique index
            info.setDisplayName(buildDisplayName(i));

            result.push_back(std::move(info));
        }
        return result;
    }

    mutable std::mt19937 m_rng;   // mutable because generate() is const but uses RNG

private:
    // ------------------------------------------------------------------------
    //  Compile‑time data: device types and locations
    // ------------------------------------------------------------------------
    const std::vector<std::string>& deviceTypes() const {
        static const std::vector<std::string> types = {
            "Android", "PC", "Server", "Tablet", "Phone",
            "Laptop", "Router", "Camera", "Sensor", "Gateway"
        };
        return types;
    }

    const std::vector<std::string>& locations() const {
        static const std::vector<std::string> locs = {
            "Office", "Lab", "Home", "LivingRoom", "Kitchen",
            "Garage", "ServerRoom", "DataCenter", "Basement", "Rooftop"
        };
        return locs;
    }

    // ------------------------------------------------------------------------
    //  Random generators
    // ------------------------------------------------------------------------
    template <typename T>
    T randomBetween(T min, T max) const {
        std::uniform_int_distribution<T> dist(min, max);
        return dist(m_rng);
    }

    std::string generateHex(std::size_t length) const {
        static const char* const hexChars = "0123456789abcdef";
        std::uniform_int_distribution<int> dist(0, 15);

        std::string hex;
        hex.reserve(length);
        for (std::size_t i = 0; i < length; ++i) {
            hex.push_back(hexChars[dist(m_rng)]);
        }
        return hex;
    }

    DBRecords::AIBackendDeviceType randomDeviceType() const {
        // Exclude SYS_Devtype_max (sentinel)
        const int maxVal = static_cast<int>(DBRecords::AIBackendDeviceType::SYS_Devtype_max);
        const int val = randomBetween(0, maxVal - 1);
        return static_cast<DBRecords::AIBackendDeviceType>(val);
    }

    std::string generatePrivateIp() const {
        // 192.168.0.0 – 192.168.255.255
        const int third = randomBetween(0, 255);
        const int fourth = randomBetween(1, 254);
        std::ostringstream oss;
        oss << "192.168." << third << '.' << fourth;
        return oss.str();
    }

    uint16_t randomPort() const {
        // Use unprivileged ports 1024–65535
        return static_cast<uint16_t>(randomBetween(1024, 65535));
    }

    std::string buildDisplayName(int index) const {
        // Choose a type and a location randomly, then append a unique index.
        const auto& types = deviceTypes();
        const auto& locs = locations();

        std::uniform_int_distribution<std::size_t> typeDist(0, types.size() - 1);
        std::uniform_int_distribution<std::size_t> locDist(0, locs.size() - 1);

        const std::string& type = types[typeDist(m_rng)];
        const std::string& loc = locs[locDist(m_rng)];

        // Build something like "Android Office #5"
        std::ostringstream oss;
        oss << type << ' ' << loc << " #" << (index + 1);
        return oss.str();
    }
};

// ----------------------------------------------------------------------------
//  DeviceInfoGenerator public interface
// ----------------------------------------------------------------------------
BackendInfoGenerator::BackendInfoGenerator()
    : d {std::make_shared<Impl>()}
{

}

BackendInfoGenerator::~BackendInfoGenerator() = default;

void BackendInfoGenerator::setSeed(unsigned seed) {
    d->m_rng.seed(seed);
}

std::vector<DBRecords::AIBackendInfo> BackendInfoGenerator::generate(int count) const {
    return d->generate(count);
}

} // namespace AITest