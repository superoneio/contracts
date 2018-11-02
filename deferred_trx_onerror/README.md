# deferred_trx_onerror
This contract is used for test deferred transaction.
It will catch onerror when deferred transaction fail.

## Create account
```
cleos system newaccount --stake-net '10.7000 EOS' --stake-cpu '10.7000 EOS' --buy-ram-kbytes 100 eosio onerrortest  EOS7YBZ94DojoYawwmDVUysYRoHFSpWQ3eu7pPv9MnLyNe5dC3Egq EOS6ByrykFubC6e9fzhp6ZJM95wyDKq9es6bgKTpMQrsCBWwwUHmX
```
```
cleos system newaccount --stake-net '10.7000 EOS' --stake-cpu '10.7000 EOS' --buy-ram-kbytes 100 eosio debug  EOS7YBZ94DojoYawwmDVUysYRoHFSpWQ3eu7pPv9MnLyNe5dC3Egq EOS6ByrykFubC6e9fzhp6ZJM95wyDKq9es6bgKTpMQrsCBWwwUHmX
```

## Deploy contract
```
cleos set contract onerrortest ../deferred_trx_onerror/
```

## Sendderr
```
cleos push action onerrortest sendderr '["0"]' -p debug
