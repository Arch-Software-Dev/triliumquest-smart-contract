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
    
    // Deserialize the mutable data using the schema format
    ATTRIBUTE_MAP mutable_data = atomicdata::deserialize(itA->mutable_serialized_data, schema_it->format);

    // default of 1
    uint64_t level = 1;

    // Search for the "level" attribute in mutable data
    auto level_it = mutable_data.find("level");

    // If found, update the level variable
    if (level_it != mutable_data.end()) {
	// Extract the std::string from the std::variant
        std::string level_str = std::get<std::string>(level_it->second);
    
        // Convert the std::string to an integer
        level = atoi(level_str.c_str());
    }

    const auto& nft_name = name_it != immutable_data.end() ? std::get<std::string>(name_it->second) : "Name not found";;

    // Burn the NFT
    /*
    eosio::action(
      eosio::permission_level{to, "active"_n},
      "atomicassets"_n,
      "burnasset"_n,
      std::make_tuple(to, id)
    ).send();
    */
    // Store the NFT stake
    nft_stakes.emplace(get_self(), [&](auto& n) {
      n.id = nft_stakes.available_primary_key();
      n.user_name = memo;
      n.nft_name = nft_name;
      n.level = level;
    });
  }
}

[[eosio::on_notify("atomicassets::transfer")]]
void triliumquest::transfernft( name from, name to, vector<uint64_t> asset_ids, string memo) { 

  if (to != get_self() ) {
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
    // check if staking from other collections that we dont support-- reject whole transaction
    check((itA->collection_name == "testo.worlds"_n || itA->collection_name == "testp.worlds"_n),"can't stake collection from "+ itA->collection_name.to_string());

    // Get templates for the given collection
    atomicassets::templates_t templates_table = atomicassets::get_templates(itA->collection_name);

    // Find the template with the given ID
    auto it = templates_table.find(template_id);
    if (it == templates_table.end()) {
        // Template not found
        return;
    }

    // Get the schema for the template
    auto schemas = atomicassets::get_schemas(itA->collection_name);
    const auto& schema_id = it->schema_name.value;
    auto schema_it = schemas.find(schema_id);
    if (schema_it == schemas.end()) {
        // handle error
    }

    // Deserialize the immutable data using the schema format
    ATTRIBUTE_MAP immutable_data = atomicdata::deserialize(it->immutable_serialized_data, schema_it->format);

    // Extract the attribute you need (e.g. "name")
    auto name_it = immutable_data.find("name");
    
    // Deserialize the mutable data using the schema format
    ATTRIBUTE_MAP mutable_data = atomicdata::deserialize(itA->mutable_serialized_data, schema_it->format);

    // default of 1
    uint64_t level = 1;

    // Search for the "level" attribute in mutable data
    auto level_it = mutable_data.find("level");

    // If found, update the level variable
    if (level_it != mutable_data.end()) {
	// Extract the std::string from the std::variant
        std::string level_str = std::get<std::string>(level_it->second);
    
        // Convert the std::string to an integer
        level = atoi(level_str.c_str());
    }

    const auto& nft_name = name_it != immutable_data.end() ? std::get<std::string>(name_it->second) : "Name not found";;

    // Burn the NFT
    /*
    eosio::action(
      eosio::permission_level{to, "active"_n},
      "atomicassets"_n,
      "burnasset"_n,
      std::make_tuple(to, id)
    ).send();
    */
    // Store the NFT stake
    nft_stakes.emplace(get_self(), [&](auto& n) {
      n.id = nft_stakes.available_primary_key();
      n.user_name = memo;
      n.nft_name = nft_name;
      n.level = level;
    });
  }
}

// [[eosio::on_notify("atomicassets::transfer")]]
// void triliumquest::transfernft( name from, name to, vector<uint64_t> asset_ids, string memo) { 


//   if (to != get_self() ) {
//     return;
//   }

//   // Parse the memo for the user_name
//   size_t staking_pos = memo.find("staking%");
//   if (staking_pos == std::string::npos) {
//     return;
//   }
//   memo.erase(0, staking_pos + 8);


//   name collection_name ;

//   for (auto id : asset_ids) {
//     atomicassets::assets_t assets = atomicassets::get_assets(to);

//     // Check what asset got received
//     auto itA = assets.find(id);

//     if (itA != assets.end()) {
//     collection_name = itA->collection_name;
//     }
    
//     // check if staking from other collections that we dont support-- reject whole transaction
//     check((!(collection_name == "testo.worlds"_n || collection_name == "testp.worlds"_n),"can't stake collection from "+ collection_name.to_string()));

//   nftstaging::nft_staging_index nft_stakes(get_self(), get_self().value);

//   // Get the assets for the given account
//   atomicassets::assets_t assets = atomicassets::get_assets(to);

//   for (const auto& id : asset_ids) {
//     // Find the asset with the given id
//     auto itA = assets.find(id);
//     if (itA == assets.end()) continue;
   
//     auto template_id = itA->template_id;

//     // Get templates for the given collection
//     atomicassets::templates_t templates_table_tr = atomicassets::get_templates(collection_name);
//     atomicassets::templates_t templates_table_to = atomicassets::get_templates(collection_name);
//     atomicassets::templates_t templates_table_tp = atomicassets::get_templates(collection_name);

//     // Find the template with the given ID
//     auto it_tr = templates_table_tr.find(template_id);
//     auto it_to = templates_table_to.find(template_id);
//     auto it_tp = templates_table_tp.find(template_id);


//     if (it_tr == templates_table_tr.end() && it_to == templates_table_to.end() && it_tp == templates_table_tp.end()) {
//         // Template not found
//     check(false,"no templ");
       
//         return;
//     }
//       auto it = templates_table_tr.end(); // initializing 

//     if(it_tr != templates_table_tr.end()){
      
//        it = it_tr;
//     }

//     if(it_to != templates_table_to.end()){
//               it = it_to;
//     }
//     if(it_tp != templates_table_tp.end()){
//        it = it_tp;
//     }
//     if(it==templates_table_tp.end()){
//     check(false,"no in it");

//     }
//     // Get the schema for the template
//     auto schemas = atomicassets::get_schemas(collection_name);
//     const auto& schema_id = it->schema_name.value;
//     auto schema_it = schemas.find(schema_id);
//     if (schema_it == schemas.end()) {
//         // handle error
//     }

//     // Deserialize the immutable data using the schema format
//     ATTRIBUTE_MAP immutable_data = atomicdata::deserialize(it->immutable_serialized_data, schema_it->format);

//     // Extract the attribute you need (e.g. "name")
//     auto name_it = immutable_data.find("name");
    
//     // Deserialize the mutable data using the schema format
//     ATTRIBUTE_MAP mutable_data = atomicdata::deserialize(itA->mutable_serialized_data, schema_it->format);

//     // default of 1
//     uint64_t level = 1;

//     // Search for the "level" attribute in mutable data
//     auto level_it = mutable_data.find("level");

//     // If found, update the level variable
//     if (level_it != mutable_data.end()) {
//   // Extract the std::string from the std::variant
//         std::string level_str = std::get<std::string>(level_it->second);
    
//         // Convert the std::string to an integer
//         level = atoi(level_str.c_str());
//     }

//     const auto& nft_name = name_it != immutable_data.end() ? std::get<std::string>(name_it->second) : "Name not found";;

//     // Burn the NFT
//     /*
//     eosio::action(
//       eosio::permission_level{to, "active"_n},
//       "atomicassets"_n,
//       "burnasset"_n,
//       std::make_tuple(to, id)
//     ).send();
//     */

//     // Store the NFT stake
//     nft_stakes.emplace(get_self(), [&](auto& n) {
//       n.id = nft_stakes.available_primary_key();
//       n.user_name = memo;
//       n.nft_name = nft_name;
//       n.level = level;
//     });
//   }
//  }
// }

[[eosio::action]]
void triliumquest::addnft(std::string user_name, std::string nft_name, uint64_t level) {
  require_auth(get_self());
  print(user_name + nft_name);
  check(user_name!="","Username cannot be empty");
  check(nft_name!="","nftname cannot be empty");
  nftstaging::nft_staging_index nft_stakes(get_self(), get_self().value);
  nft_stakes.emplace(get_self(), [&](auto& n) {
      n.id = nft_stakes.available_primary_key();
      n.user_name = user_name;
      n.nft_name = nft_name;
      n.level = level;
  });
}

[[eosio::action]]
void triliumquest::removenft(uint64_t id) {
    require_auth(get_self());
    nftstaging::nft_staging_index nft_stakes(get_self(), get_self().value);

    auto iterator = nft_stakes.find(id);
    eosio::check(iterator != nft_stakes.end(), "Record with the given ID does not exist");

    nft_stakes.erase(iterator);
}

[[eosio::action]]
void triliumquest::mintnft(name wax_id, name schema_name, uint32_t template_id, uint64_t level)
{
   // Define your collection, templates and schema are passed in
   name collection_name = "triliumquest"_n;

   /*
   atomicassets::ATTRIBUTE_MAP mutable_data = {
     {"level", level}
   };
   */
   
   atomicassets::ATTRIBUTE_MAP mutable_data;

   // Mint the NFT to the player account
   action(
     permission_level{get_self(), "active"_n},
     "atomicassets"_n,
     "mintasset"_n,
     std::make_tuple(get_self(), collection_name, schema_name, template_id, wax_id, vector<atomicassets::ATTRIBUTE_MAP>{}, mutable_data, vector<asset>{})
   ).send();
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
void triliumquest::withdrawnft(name wax_id, uint64_t id, name schema_name, uint64_t template_id) {
  require_auth(get_self());

  // Find the NFT
  nftstaging::nft_staging_index _nfts(get_self(), get_self().value);
  auto stake = _nfts.find(id);

  check(stake != _nfts.end(), "NFT is not found");

  // Define your collection, templates and schema are passed in
  name collection_name = "triliumquest"_n;

  uint32_t level = stake->level;
  std::string user_name = stake->user_name;

  /*
  atomicassets::ATTRIBUTE_MAP mutable_data = {
    {"level", level}
  };
  */

  atomicassets::ATTRIBUTE_MAP mutable_data;

  // Mint the NFT to the player account
  action(
    permission_level{get_self(), "active"_n},
    "atomicassets"_n,
    "mintasset"_n,
    std::make_tuple(get_self(), collection_name, schema_name, template_id, wax_id, vector<atomicassets::ATTRIBUTE_MAP>{}, mutable_data, vector<asset>{})
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


