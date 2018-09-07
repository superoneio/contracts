#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/singleton.hpp>

namespace eosio{

    using std::string;

    class bankofstake : public eosio::contract {
        public:
            bankofstake( account_name _self );
            ~bankofstake();

            void empty();
            void check();
            void expireorder();
            void addcreditor( account_name account );
            void delcreditor( account_name account );
            void listactive();
            void activate( account_name account );
            void setplan( asset price, asset cpu, asset net, uint64_t duration );
            void delplan( uint64_t id );
            void transfer( account_name from, account_name to, asset quantity, string memo );
            void undelegatebw( uint64_t id );
            void forfree(account_name owner);

            int get_active_size(){
                auto active_index = _creditor.get_index<N(isactive)>();
                auto low = active_index.lower_bound(1);
                auto upper = active_index.upper_bound(2);
                auto count = count_iterator(low, upper);
                return count;
            }

            template<typename T1, typename T2>
            int count_iterator(const T1& low, const T2& upper){
                int count = 0;
                auto itr = low;
                while(itr != upper){
                    count++;
                    itr++;
                }
                return count;
            }

            template<typename T>
            auto get_index_iterator(const T& itr, int num){
                auto _itr = itr;
                while(num--){
                    _itr++;
                }
                return _itr;
            }

        private:
            //@abi table
            struct order {
                uint64_t        id;
                account_name    buyer;
                asset           price;
                account_name    creditor;
                account_name    beneficiary;
                uint64_t        plan_id;
                asset           cpu_staked;
                asset           net_staked;
                uint64_t        expire_at;

                uint64_t primary_key()const { return id; }
                uint64_t get_buyer()const { return buyer; }
            };

            //@abi table
            struct history {
                uint64_t    id;
                string      content;
                uint64_t    created_at;

                uint64_t primary_key()const { return id; }
            };

            //@abi table
            struct plan {
                uint64_t    id;
                asset       price;
                asset       cpu;
                asset       net;
                uint64_t    duration;
                uint64_t    created_at;
                uint64_t    updated_at;

                uint64_t primary_key()const { return id; }
                uint64_t get_price()const { return price.amount; }
            };

            //@abi table
            struct creditor {
                account_name    account;
                uint64_t        is_active;
                uint64_t        created_at;
                uint64_t        updated_at;

                uint64_t primary_key()const { return account; }
                uint64_t get_is_active()const { return is_active; }
            };

            //@abi table
            struct global_config {
                uint64_t sender_id = 0;
            };

            typedef eosio::singleton<N(global), global_config> global_configs;
            typedef eosio::multi_index<N(order), order,
                indexed_by< N(buyer), const_mem_fun<order, uint64_t, &order::get_buyer> >
                > orders;
            typedef eosio::multi_index<N(plan), plan,
                indexed_by< N(price), const_mem_fun<plan, uint64_t, &plan::get_price> >
                > plans;
            typedef eosio::multi_index<N(creditor), creditor,
                indexed_by< N(isactive), const_mem_fun<creditor, uint64_t, &creditor::get_is_active> >
                > creditors;  

            orders _order;
            plans _plan;
            creditors _creditor;
            global_configs _global_config;

    };

}// namespace bankofstake
