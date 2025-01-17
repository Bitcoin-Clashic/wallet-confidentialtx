// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CONSENSUS_PARAMS_H
#define BITCOIN_CONSENSUS_PARAMS_H

#include <asset.h>
#include <optional.h>
#include <uint256.h>
#include <limits>

#include <script/script.h> // mandatory_coinbase_destination
#include <amount.h> // genesis_subsidy

namespace Consensus {

enum DeploymentPos
{
    DEPLOYMENT_TESTDUMMY,
    DEPLOYMENT_TAPROOT, // Deployment of Schnorr/Taproot (BIPs 340-342)
    DEPLOYMENT_DYNA_FED, // Deployment of dynamic federation
    // NOTE: Also add new deployments to VersionBitsDeploymentInfo in versionbits.cpp
    MAX_VERSION_BITS_DEPLOYMENTS
};

/**
 * Struct for each individual consensus rule change using BIP9.
 */
struct BIP9Deployment {
    /** Bit position to select the particular bit in nVersion. */
    int bit;
    /** Start MedianTime for version bits miner confirmation. Can be a date in the past */
    // ELEMENTS: Interpreted as block height!
    int64_t nStartTime;
    /** Timeout/expiry MedianTime for the deployment attempt. */
    // ELEMENTS: Interpreted as block height!
    int64_t nTimeout;

    // ELEMENTS: allow overriding the signalling period length rather than using `nMinerConfirmationWindow`
    Optional<uint32_t> nPeriod{nullopt};
    // ELEMENTS: allow overriding the activation threshold rather than using `nRuleChangeActivationThreshold`
    Optional<uint32_t> nThreshold{nullopt};

    /** Constant for nTimeout very far in the future. */
    static constexpr int64_t NO_TIMEOUT = std::numeric_limits<int64_t>::max();

    /** Special value for nStartTime indicating that the deployment is always active.
     *  This is useful for testing, as it means tests don't need to deal with the activation
     *  process (which takes at least 3 BIP9 intervals). Only tests that specifically test the
     *  behaviour during activation cannot use this. */
    static constexpr int64_t ALWAYS_ACTIVE = -1;
};

/**
 * Parameters that influence chain consensus.
 */
struct Params {
    uint256 hashGenesisBlock;
    int nSubsidyHalvingInterval;
    /* Block hash that is excepted from BIP16 enforcement */
    uint256 BIP16Exception;
    /** Block height and hash at which BIP34 becomes active */
    int BIP34Height;
    uint256 BIP34Hash;
    /** Block height at which BIP65 becomes active */
    int BIP65Height;
    /** Block height at which BIP66 becomes active */
    int BIP66Height;
    /** Block height at which CSV (BIP68, BIP112 and BIP113) becomes active */
    int CSVHeight;
    /** Block height at which Segwit (BIP141, BIP143 and BIP147) becomes active.
     * Note that segwit v0 script rules are enforced on all blocks except the
     * BIP 16 exception blocks. */
    int SegwitHeight;
    /** Don't warn about unknown BIP 9 activations below this height.
     * This prevents us from warning about the CSV and segwit activations. */
    int MinBIP9WarningHeight;
    /**
     * Minimum blocks including miner confirmation of the total of 2016 blocks in a retargeting period,
     * (nPowTargetTimespan / nPowTargetSpacing) which is also used for BIP9 deployments.
     * Examples: 1916 for 95%, 1512 for testchains.
     */
    uint32_t nRuleChangeActivationThreshold;
    uint32_t nMinerConfirmationWindow;
    BIP9Deployment vDeployments[MAX_VERSION_BITS_DEPLOYMENTS];
    /** Proof of work parameters */
    uint256 powLimit;
    bool fPowAllowMinDifficultyBlocks;
    bool fPowNoRetargeting;
    int64_t nPowTargetSpacing;
    int64_t nPowTargetTimespan;
    int64_t DifficultyAdjustmentInterval() const { return nPowTargetTimespan / nPowTargetSpacing; }
    /** The best chain should have at least this much work */
    uint256 nMinimumChainWork;
    /** By default assume that the signatures in ancestors of this block are valid */
    uint256 defaultAssumeValid;

    /**
     * If true, witness commitments contain a payload equal to a Bitcoin Script solution
     * to the signet challenge. See BIP325.
     */
    bool signet_blocks{false};
    std::vector<uint8_t> signet_challenge;

    //
    // ELEMENTS CHAIN PARAMS
    CScript mandatory_coinbase_destination;
    CAmount genesis_subsidy;
    CAsset subsidy_asset;
    bool connect_genesis_outputs;
    bool has_parent_chain;
    uint256 parentChainPowLimit;
    uint32_t pegin_min_depth;
    CScript parent_chain_signblockscript;
    bool ParentChainHasPow() const { return parent_chain_signblockscript == CScript();}
    CScript fedpegScript;
    CAsset pegged_asset;
    CAsset parent_pegged_asset;
    // g_con_blockheightinheader global hack instead of proper arg due to circular dep
    std::string genesis_style;
    CScript signblockscript;
    uint32_t max_block_signature_size;
    // g_signed_blocks - Whether blocks are signed or not, get around circular dep
    // Set positive to avoid division by 0
    // for non-dynafed chains and unit tests
    uint32_t dynamic_epoch_length = std::numeric_limits<uint32_t>::max();
    // Used to seed the extension space for first dynamic blocks
    std::vector<std::vector<unsigned char>> first_extension_space;
    // Used to allow M-epoch-old peg-in addresses as deposits
    // default 1 to not break legacy chains implicitly.
    size_t total_valid_epochs = 1;
    bool elements_mode = false;
};
} // namespace Consensus

#endif // BITCOIN_CONSENSUS_PARAMS_H
