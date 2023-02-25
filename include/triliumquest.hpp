#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/symbol.hpp>
#include <eosio/name.hpp>
#include <eosio/multi_index.hpp>

using namespace eosio;

CONTRACT triliumquest : public contract {
   public:
      using contract::contract;

      [[eosio::on_notify("atomicassets::transfer")]]
      void depositnft(name from, name to, asset quantity, std::string memo);

      ACTION withdrawnft(uint64_t nft_id);

      [[eosio::on_notify("alien.worlds::transfer")]]
      void deposittlm(name from, name to, asset quantity, std::string memo);

      ACTION withdrawtlm();

      using withdrawnft_action = action_wrapper<"withdrawnft"_n, &triliumquest::withdrawnft>;
      using withdrawtlm_action = action_wrapper<"withdrawtlm"_n, &triliumquest::withdrawtlm>;
  private:
      TABLE nftstaging {
        uint64_t nft_id;
	name owner;
	std::string user_name;

	uint64_t primary_key() const { return nft_id; }
      };

      TABLE tlmstaging {
        std::string user_name;
	asset amount;
	name owner;

	uint64_t primary_key() const { return owner.value; }
      };

      typedef eosio::multi_index<"itemstaging"_n, nftstaging> nft_staging_index;
      typedef eosio::multi_index<"tlmstaging"_n, tlmstaging> tlm_staging_index;
};
