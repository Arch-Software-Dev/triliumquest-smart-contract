# Ricardian Clause

## Description

This smart contract allows users to stake NFTs and TLM tokens. Users can withdraw their staked NFTs or TLM at any time.

## Acceptance

By interacting with this smart contract, you agree to the terms outlined in this Ricardian Clause.

## Parties

**Party A:** The "triliumquest" smart contract and its associated accounts.

**Party B:** Any user who interacts with the smart contract.

## Terms

### Depositing an NFT

When depositing an NFT to this smart contract, the user must include a memo in the format `nft_id:user_name:<username>`. The `nft_id` should be the ID of the NFT being staked, and `<username>` should be replaced with the username of the staker. If an NFT is already staked, the deposit will be rejected. The smart contract will store the NFT and the associated user information in its table.

### Withdrawing an NFT

To withdraw a staked NFT, the user must call the `withdrawnft` action and provide the `nft_id` of the NFT to be withdrawn. The NFT will be transferred back to the original owner and the record will be deleted from the smart contract's table. Only the owner of the NFT can withdraw it.

### Depositing TLM

When depositing TLM to this smart contract, the user must include a memo in the format `user_name:<username>`. The `<username>` should be replaced with the username of the staker. The smart contract will store the TLM and the associated user information in its table.

### Withdrawing TLM

To withdraw staked TLM, the user must call the `withdrawtlm` action. The TLM will be transferred back to the original owner and the record will be deleted from the smart contract's table. Only the owner of the TLM can withdraw it.

## Disclaimer

This smart contract is provided as-is, without warranty of any kind, express or implied, including but not limited to the warranties of merchantability, fitness for a particular purpose and noninfringement. In no event shall the authors or copyright holders be liable for any claim, damages or other liability, whether in an action of contract, tort or otherwise, arising from, out of or in connection with the smart contract or the use or other dealings in the smart contract.
