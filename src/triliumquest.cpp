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

  // Get the assets for the given account
  atomicassets::assets_t assets = atomicassets::get_assets(to);

  for (const auto& id : asset_ids) {
    // Find the asset with the given id
    auto itA = assets.find(id);
    if (itA == assets.end()) continue;
   
    auto template_id = itA->template_id;

    // Get templates for the given collection
    atomicassets::templates_t templates_table = atomicassets::get_templates(collection_name);

    // Find the template with the given ID
    auto it = templates_table.find(template_id);
    if (it == templates_table.end()) {
        // Template not found
        return;
    }

    // Get the schema for the template
    auto schemas = atomicassets::get_schemas(collection_name);
    const auto& schema_id = it->schema_name.value;
    auto schema_it = schemas.find(schema_id);
    if (schema_it == schemas.end()) {
        // handle error
    }

    // Deserialize the immutable data using the schema format
    ATTRIBUTE_MAP immutable_data = atomicdata::deserialize(it->immutable_serialized_data, schema_it->format);

    // Extract the attribute you need (e.g. "name")
    auto name_it = immutable_data.find("name");
    
    const auto& nft_name = name_it != immutable_data.end() ? std::get<std::string>(name_it->second) : "Name not found";;

    // Burn the NFT
    eosio::action(
      eosio::permission_level{to, "active"_n},
      "atomicassets"_n,
      "burnasset"_n,
      std::make_tuple(to, id)
    ).send();

    // Store the NFT stake
    nft_stakes.emplace(get_self(), [&](auto& n) {
      n.nft_id = id;
      n.user_name = memo;
      n.nft_name = nft_name;
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
void triliumquest::wipeall() {
  require_auth(get_self());

  // Clear staged NFTs
  nftstaging::nft_staging_index _nfts(get_self(), get_self().value);
  auto itr1 = _nfts.begin();
  while (itr1 != _nfts.end()) {
    itr1 = _nfts.erase(itr1);
  }

  // Clear staged TLM
  tlm_staging_index _tlm(get_self(), get_self().value);
  auto itr2 = _tlm.begin();
  while (itr2 != _tlm.end()) {
    itr2 = _tlm.erase(itr2);
  }
}

[[eosio::action]]
void triliumquest::withdrawnft(uint64_t nft_id, name wax_id, uint64_t level, name schema_name, uint64_t template_id) {
  require_auth(get_self());

  // Find the NFT
  nftstaging::nft_staging_index _nfts(get_self(), get_self().value);
  auto stake = _nfts.find(nft_id);
  check(stake != _nfts.end(), "NFT is not staked");

  // Define your collection and template names
  name collection_name = "triliumquest"_n;

  // Define the level
  atomicassets::ATTRIBUTE_MAP mutable_data = {
    {"level", level}
  };

  // Mint the NFT to the player account
  action(
    permission_level{get_self(), "active"_n},
    "atomicassets"_n,
    "mintasset"_n,
    std::make_tuple(get_self(), wax_id, collection_name, schema_name, template_id, vector<atomicassets::ATTRIBUTE_MAP>{}, mutable_data, "")
  ).send();

  // Remove the NFT record
  _nfts.erase(stake);
}

[[eosio::action]]
void triliumquest::withdrawtlm(name wax_id) {
  require_auth(get_self());

  // Find the stake
  tlm_staging_index _tlm(get_self(), get_self().value);
  auto tlm = _tlm.find(get_self().value);
  check(tlm != _tlm.end(), "TLM is not staked");

  // Transfer the TLM back to the owner
  action(
    permission_level{get_self(), "active"_n},
    "alien.worlds"_n,
    "transfer"_n,
    std::make_tuple(get_self(), wax_id, tlm->amount, "Unstaking TLM")
  ).send();

  // Remove the stake
  _tlm.erase(tlm);
}
