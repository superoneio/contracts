# bankofstake
This contract is used for automatic leasing of cpu and net.
Support for users free delegatebw as an emergency.

## Create account
```
cleos system newaccount --stake-net '10.7000 EOS' --stake-cpu '10.7000 EOS' --buy-ram-kbytes 100 eosio bankofstake  EOS7YBZ94DojoYawwmDVUysYRoHFSpWQ3eu7pPv9MnLyNe5dC3Egq EOS6ByrykFubC6e9fzhp6ZJM95wyDKq9es6bgKTpMQrsCBWwwUHmX
cleos  system newaccount --stake-net '10.7000 EOS' --stake-cpu '10.7000 EOS' --buy-ram-kbytes 100 eosio stake1  EOS7YBZ94DojoYawwmDVUysYRoHFSpWQ3eu7pPv9MnLyNe5dC3Egq EOS6ByrykFubC6e9fzhp6ZJM95wyDKq9es6bgKTpMQrsCBWwwUHmX
cleos system newaccount --stake-net '10.7000 EOS' --stake-cpu '10.7000 EOS' --buy-ram-kbytes 100 eosio stake2  EOS7YBZ94DojoYawwmDVUysYRoHFSpWQ3eu7pPv9MnLyNe5dC3Egq EOS6ByrykFubC6e9fzhp6ZJM95wyDKq9es6bgKTpMQrsCBWwwUHmX
cleos system newaccount --stake-net '10.7000 EOS' --stake-cpu '10.7000 EOS' --buy-ram-kbytes 100 eosio test  EOS7YBZ94DojoYawwmDVUysYRoHFSpWQ3eu7pPv9MnLyNe5dC3Egq EOS6ByrykFubC6e9fzhp6ZJM95wyDKq9es6bgKTpMQrsCBWwwUHmX
```

## Deploy contract
```
cleos set contract bankofstake ../bankofstake
```

## Set permission
```
cleos set account permission bankofstake active '{"threshold":1,"keys":[{"key":"EOS7YBZ94DojoYawwmDVUysYRoHFSpWQ3eu7pPv9MnLyNe5dC3Egq","weight":1}],"accounts":[{"permission":{"actor":"bankofstake","permission":"eosio.code"},"weight":1}]}' owner -p bankofstake@owner
cleos set account permission stake1 active '{"threshold":1,"keys":[{"key":"EOS7YBZ94DojoYawwmDVUysYRoHFSpWQ3eu7pPv9MnLyNe5dC3Egq","weight":1}],"accounts":[{"permission":{"actor":"bankofstake","permission":"eosio.code"},"weight":1}]}' owner -p stake1@owner
cleos set account permission stake2 active '{"threshold":1,"keys":[{"key":"EOS7YBZ94DojoYawwmDVUysYRoHFSpWQ3eu7pPv9MnLyNe5dC3Egq","weight":1}],"accounts":[{"permission":{"actor":"bankofstake","permission":"eosio.code"},"weight":1}]}' owner -p stake2@owner
```

## Transfer EOS to stake account
```
cleos push action eosio.token transfer '["eosio","bankofstake","1000000.0000 EOS","stake"]' -p eosio
cleos push action eosio.token transfer '["eosio","stake1","1000000.0000 EOS","stake"]' -p eosio
cleos push action eosio.token transfer '["eosio","stake2","1000000.0000 EOS","stake"]' -p eosio
```

## Add creditor
```
cleos push action bankofstake addcreditor '["bankofstake"]'  -p bankofstake
cleos push action bankofstake addcreditor '["stake1"]'  -p bankofstake
cleos push action bankofstake addcreditor '["stake2"]'  -p bankofstake
```

## Active creditor
```
cleos push action bankofstake activate '["bankofstake"]'  -p bankofstake
cleos push action bankofstake activate '["stake1"]'  -p bankofstake
cleos push action bankofstake activate '["stake2"]'  -p bankofstake
```

## Lease plan
```
cleos push action bankofstake setplan '["0.0000 EOS","5.0000 EOS","5.0000 EOS","10"]' -p bankofstake
cleos push action bankofstake setplan '["1.0000 EOS","10.0000 EOS","10.0000 EOS","10"]' -p bankofstake
cleos push action bankofstake setplan '["10.0000 EOS","100.0000 EOS","100.0000 EOS","10"]' -p bankofstake
```

## Leasing cpu and net for free
```
cleos push action bankofstake forfree '["test"]' -p test
```

## Leasing cpu and net
```
cleos push action eosio.token transfer '["test","bankofstake","1.0000 EOS",""]' -p test
```

## Leasing cpu and net for others
```
cleos push action eosio.token transfer '["test","bankofstake","1.0000 EOS","stake1"]' -p test
```

## Delete creditor
```
cleos push action bankofstake delcreditor '["stake1"]'  -p bankofstake
```

## Delete plan
```
cleos push action bankofstake delplan '["0"]'  -p bankofstake
```

## Clear expire order
```
cleos push action bankofstake expireorder '[]' -p bankofstake
```

## List active creditor
```
cleos push action bankofstake listactive '[]' -p bankofstake
```