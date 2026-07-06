#pragma once

/**
 * @file AI-generated class to create sample BackendInfo objects
 */

#include <ProjectAgency/DB/AIBackendInfo.h>

#include <memory>
#include <vector>

namespace AITest {

/**
 * @brief Generates AIBackendInfo records with plausible device data.
 *
 * The generator creates from 1 to 100 unique records. Display names are
 * formed by combining a device type (e.g. "Android") with a location
 * (e.g. "Office") and a sequential index to guarantee uniqueness.
 * IDs and tokens are generated as random 64‑character hex strings.
 */
class BackendInfoGenerator {
public:
    BackendInfoGenerator();
    ~BackendInfoGenerator();

    /**
     * @brief Generate a vector of records.
     * @param count Number of records to generate (1..100 inclusive).
     * @return std::vector<DBRecords::AIBackendInfo> filled with random data.
     * @throws std::out_of_range if count is not in [1, 100].
     */
    std::vector<DBRecords::AIBackendInfo> generate(int count) const;

    /**
     * @brief Set a seed for the random number generator (for reproducibility).
     */
    void setSeed(unsigned seed);

private:
    class Impl;
    std::shared_ptr<Impl> d;
};

} // namespace AITest