



#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/symbol.hpp>
#include <eosio/name.hpp>
#include <eosio/crypto.hpp>
#include <eosio/multi_index.hpp>
#include <atomicassets-interface.hpp>

using namespace eosio;

CONTRACT triliumquest : public contract {
   public:
      using contract::contract;

      [[eosio::on_notify("atomicassets::logtransfer")]]
      void depositnft(name collection_name, name from, name to, vector<uint64_t> asset_ids, string memo);

      ACTION withdrawnft(name wax_id, uint64_t id, name schema_name, uint64_t template_id);

      [[eosio::on_notify("alien.worlds::transfer")]]
      void deposittlm(name from, name to, asset quantity, std::string memo);

      ACTION withdrawtlm(name wax_id);
      ACTION addnft(std::string user_name, std::string nft_name, uint64_t level);
      ACTION removenft(uint64_t id);
      ACTION mintnft(name wax_id, name schema_name, uint32_t template_id, uint64_t level);

      // this one needs to be commented out when we go to Main Net
      ACTION wipeall();

      using withdrawnft_action = action_wrapper<"withdrawnft"_n, &triliumquest::withdrawnft>;
      using withdrawtlm_action = action_wrapper<"withdrawtlm"_n, &triliumquest::withdrawtlm>;
      using wipeall_action = action_wrapper<"wipeall"_n, &triliumquest::wipeall>;
      using addnft_action = action_wrapper<"addnft"_n, &triliumquest::addnft>;
      using removenft_action = action_wrapper<"removenft"_n, &triliumquest::removenft>;
      using mintnft_action = action_wrapper<"mintnft"_n, &triliumquest::mintnft>;
  
 private:
      TABLE nftstaging {
        uint64_t id;
        std::string user_name;
        std::string nft_name;
        uint64_t level;

        uint64_t primary_key() const { return id; }
        // Secondary index for user_name
        eosio::checksum256 get_user_name() const { return eosio::sha256(user_name.c_str(), user_name.size()); }
        // Define the index name and the function to create the index
        typedef eosio::multi_index<"nftstaging"_n, nftstaging,eosio::indexed_by<"byusername"_n, eosio::const_mem_fun<nftstaging, eosio::checksum256, &nftstaging::get_user_name>>> nft_staging_index;
};

     TABLE tlmstaging {
       std::string user_name;
       asset amount;
       uint64_t primary_key() const { return static_cast<uint64_t>(*reinterpret_cast<const uint64_t *>(get_user_name().data())); }

       // Secondary index for user_name
       eosio::checksum256 get_user_name() const { return eosio::sha256(user_name.c_str(), user_name.size()); }

       // Define the index name and the function to create the index
       typedef eosio::multi_index<"tlmstaging"_n, tlmstaging,
        eosio::indexed_by<"byusername"_n, eosio::const_mem_fun<tlmstaging, eosio::checksum256, &tlmstaging::get_user_name>>> tlm_staging_index;
     };

typedef eosio::multi_index<"tlmstaging"_n, tlmstaging,
 eosio::indexed_by<"byusername"_n, eosio::const_mem_fun<tlmstaging, eosio::checksum256, &tlmstaging::get_user_name>>> tlm_staging_index;


};


