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

      ACTION withdrawnft(uint64_t nft_id, name wax_id, uint64_t level, name schema_name, uint64_t template_id);

      [[eosio::on_notify("alien.worlds::transfer")]]
      void deposittlm(name from, name to, asset quantity, std::string memo);

      ACTION withdrawtlm(name wax_id);

      ACTION wipeall();

      using withdrawnft_action = action_wrapper<"withdrawnft"_n, &triliumquest::withdrawnft>;
      using withdrawtlm_action = action_wrapper<"withdrawtlm"_n, &triliumquest::withdrawtlm>;
      using wipeall_action = action_wrapper<"wipeall"_n, &triliumquest::wipeall>;
  private:
      TABLE nftstaging {
        uint64_t nft_id;
	std::string user_name;
        std::string nft_name;

	uint64_t primary_key() const { return nft_id; }

	// Secondary index for user_name
        eosio::checksum256 get_user_name() const { return eosio::sha256(user_name.c_str(), user_name.size()); }

        // Define the index name and the function to create the index
        typedef eosio::multi_index<"nftstaging"_n, nftstaging, eosio::indexed_by<"byusername"_n, eosio::const_mem_fun<nftstaging, eosio::checksum256, &nftstaging::get_user_name>>> nft_staging_index;
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
