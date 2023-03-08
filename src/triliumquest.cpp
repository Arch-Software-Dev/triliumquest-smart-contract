#include <triliumquest.hpp>

[[eosio::on_notify("atomicassets::logtransfer")]]
void triliumquest::depositnft(name collection_name, name from, name to, vector<uint64_t> asset_ids, string memo) {
  if (to != get_self() || collection_name != "triliumquest"_n) {
    return;
  }

  // Parse the memo for the user_name
  size_t staking_pos = memo.find("staking%");
  if (staking_pos == std::string::npos) {
    return;
  }

  memo.erase(0, staking_pos + 8);
  nftstaging::nft_staging_index nft_stakes(get_self(), get_self().value);


  for (auto id : asset_ids) {
    // Store the NFT stake
    nft_stakes.emplace(get_self(), [&](auto& n) {
      n.owner = from;
      n.nft_id = id;
      n.user_name = memo;
    });
  }
}

[[eosio::on_notify("alien.worlds::transfer")]]
void triliumquest::deposittlm(name from, name to, asset quantity, std::string memo) {
  // Check if the transfer is for this contract and has user_name in memo
  if (to != get_self() || memo.find("user_name:") == std::string::npos) {
    return;
  }

  // Parse the memo to get the user name
  size_t separator_pos = memo.find("user_name:");
  std::string user_name = memo.substr(separator_pos + 10);

  // Check that the asset is TLM
  check(quantity.symbol == symbol("TLM", 4), "Invalid symbol");

  // Check if the user has already staked TLM
  tlm_staging_index _tlm(get_self(), get_self().value);
  auto existing_stake = _tlm.find(from.value);

  if (existing_stake == _tlm.end()) {
    // Store the stake
    _tlm.emplace(get_self(), [&](auto& s) {
      s.owner = from;
      s.user_name = user_name;
      s.amount = quantity;
    });
  } else {
    // Update the stake
    _tlm.modify(existing_stake, same_payer, [&](auto& s) {
      s.amount += quantity;
    });
  }
}

[[eosio::action]]
void triliumquest::withdrawnft(uint64_t nft_id) {
  // Find the NFT
  nftstaging::nft_staging_index _nfts(get_self(), get_self().value);
  auto stake = _nfts.find(nft_id);
  check(stake != _nfts.end(), "NFT is not staked");

  // Check that the owner is the one requesting the unstake
  require_auth(stake->owner);

  // Transfer the NFT back to the original owner
  action(
    permission_level{get_self(), "active"_n},
    "atomicassets"_n,
    "transfer"_n,
    std::make_tuple(get_self(), stake->owner, asset(nft_id, symbol("NFT", 0)), "Withdraw NFT")
  ).send();

  // Remove the NFT record
  _nfts.erase(stake);
}

[[eosio::action]]
void triliumquest::withdrawtlm() {
  // Find the stake
  tlm_staging_index _tlm(get_self(), get_self().value);
  auto tlm = _tlm.find(get_self().value);
  check(tlm != _tlm.end(), "TLM is not staked");

  // Check that the owner is the one requesting the unstake
  require_auth(tlm->owner);

  // Transfer the TLM back to the owner
  action(
    permission_level{get_self(), "active"_n},
    "alien.worlds"_n,
    "transfer"_n,
    std::make_tuple(get_self(), tlm->owner, tlm->amount, "Unstaking TLM")
  ).send();

  // Remove the stake
  _tlm.erase(tlm);
}
