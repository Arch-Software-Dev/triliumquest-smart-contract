#include <triliumquest.hpp>

[[eosio::on_notify("atomicassets::transfer")]]
void triliumquest::depositnft(name from, name to, asset quantity, std::string memo) {
  // Check if the transfer is for this contract
  if (to != get_self()) {
    return;
  }

  // Parse the memo to get the NFT ID
  size_t separator_pos = memo.find(":");
  check(separator_pos != std::string::npos, "Invalid memo format");
  uint64_t nft_id = std::stoull(memo.substr(0, separator_pos));
  std::string user_name = memo.substr(separator_pos + 1);

  // Check if the user has already staked the NFT
  nft_staging_index _nfts(get_self(), get_self().value);
  auto existing_stake = _nfts.find(nft_id);
  check(existing_stake == _nfts.end(), "NFT is already staked");

  // Store the NFT
  _nfts.emplace(get_self(), [&](auto& n) {
    n.nft_id = nft_id;
    n.owner = from;
    n.user_name = user_name;
  });
}

[[eosio::on_notify("alien.worlds::transfer")]]
void triliumquest::deposittlm(name from, name to, asset quantity, std::string memo) {
  // Check if the transfer is for this contract
  if (to != get_self()) {
    return;
  }

  // Parse the memo to get the NFT ID
  size_t separator_pos = memo.find("user_name:");
  std::string user_name = memo.substr(separator_pos + 1);

  // Check that the asset is TLM
  check(quantity.symbol == symbol("TLM", 4), "Invalid symbol");

  // Check if the user has already staked TLM
  tlm_staging_index _tlm(get_self(), get_self().value);
  auto existing_stake = _tlm.find(from.value);

  // Store the stake
  _tlm.emplace(get_self(), [&](auto& s) {
    s.owner = from;
    s.user_name = user_name;
    s.amount = quantity;
  });
}

[[eosio::action]]
void triliumquest::withdrawnft(uint64_t nft_id) {
  // Find the NFT
  nft_staging_index _nfts(get_self(), get_self().value);
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
