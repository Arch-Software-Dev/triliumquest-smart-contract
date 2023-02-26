<h1 class="contract"> depositnft </h1>

When a user deposits an NFT to the contract, the NFT is stored in the NFT staging table until the user decides to withdraw it.

<h1 class="contract"> deposittlm </h1>

When a user deposits TLM tokens to the contract, the TLM is stored in the TLM staging table until the user decides to withdraw it.

<h1 class="contract"> withdrawnft </h1>

When a user requests to withdraw their NFT, the NFT is transferred back to the original owner and the NFT record is deleted from the NFT staging table.

<h1 class="contract"> withdrawtlm </h1>

When a user requests to withdraw their TLM tokens, the tokens are transferred back to the original owner and the TLM record is deleted from the TLM staging table.
