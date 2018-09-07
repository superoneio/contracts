#include <eosiolib/transaction.hpp>
#include <eosiolib/print.hpp>
#include<eosiolib/currency.hpp>
#include "bankofstake.hpp"
#include "eosio.system.hpp"

namespace eosio {

    bankofstake::bankofstake( account_name self)
    :contract(self),
    _order(_self,_self),
    _plan(_self,_self),
    _creditor(_self,_self),
    _global_config(_self,_self){
        if( !_global_config.exists() ){
            global_config global;
            global.sender_id = 0;
            _global_config.set(global, _self);
        }
    }

    bankofstake::~bankofstake(){

    }

    void bankofstake::empty(){

    }

    void bankofstake::check(){

    }

    void bankofstake::expireorder(){
        require_auth( _self );

        auto order = _order.begin();
        while( order != _order.end() ){
            if( now() >= order->expire_at ){
                INLINE_ACTION_SENDER(eosiosystem::system_contract, undelegatebw)( N(eosio), { order->creditor, N(active)}
                    ,{ order->creditor, order->beneficiary, order->net_staked, order->cpu_staked });
                order = _order.erase(order);
            }else order ++;
        }
    }

    void bankofstake::addcreditor( account_name account ){
        require_auth( _self );

        eosio_assert( is_account(account), "account does not exist");

        _creditor.emplace(_self, [&]( auto& c ){
            c.account = account;
            c.is_active = 0;
            c.created_at = now();
            c.updated_at = now();
        });
    }

    void bankofstake::delcreditor( account_name account ){
        require_auth(_self);

        auto creditor = _creditor.find(account);
        if(creditor != _creditor.end()){
            _creditor.erase(creditor);
        }
    }

    void bankofstake::listactive(){
        auto active_index = _creditor.get_index<N(isactive)>();
        auto low = active_index.lower_bound(1);
        auto upper = active_index.upper_bound(2);
        auto count = count_iterator(low, upper);

        for(auto creditor = low; creditor != upper; creditor++){
            print(creditor->account , " " , creditor->is_active);
        }
    }

    void bankofstake::activate( account_name account ){
        require_auth(_self);

        auto creditor = _creditor.find(account);

        eosio_assert( creditor != _creditor.end() , "account is not exist in creditor" );

        _creditor.modify(creditor, 0, [&]( auto& c ){
            c.is_active = creditor->is_active == 0?1:0;
        });
    }

    void bankofstake::setplan( asset price, asset cpu, asset net, uint64_t duration ){
        require_auth(_self);

        eosio_assert( price.amount >= 0 && cpu.amount > 0 && net.amount > 0 && duration > 0 , "ensure input positive number" );

        _plan.emplace(_self, [&]( auto& p ){
            p.id = _plan.available_primary_key();
            p.price = price;
            p.cpu = cpu;
            p.net = net;
            p.duration = duration;
        });

    }

    void bankofstake::delplan( uint64_t id ){
        require_auth(_self);

        auto plan = _plan.find(id);
        _plan.erase(plan);
    }
    
    void bankofstake::undelegatebw( uint64_t id ){
        auto order = _order.find(id);

        eosio_assert( order != _order.end(), "order is not exists");
        eosio_assert( now() >= order->expire_at, "it is not expire");

        INLINE_ACTION_SENDER(eosiosystem::system_contract, undelegatebw)( N(eosio), { order->creditor, N(active)}
                    ,{ order->creditor, order->beneficiary, order->net_staked, order->cpu_staked });
        
        _order.erase(order);
    }

    void bankofstake::transfer( account_name from, account_name to, asset quantity, string memo ){
        account_name beneficiary;
        beneficiary = memo != ""?string_to_name(memo.c_str()):from;

        bool assert_judge = false;

        for( auto itr = _plan.begin(); itr != _plan.end(); itr++ ){
            if( itr->price.amount == quantity.amount ){
                assert_judge = true;

                auto active_size = get_active_size();

                eosio_assert( active_size !=0 , "active stake account is zero, maybe you can try it next time");

                auto num = ( now()/86400 ) % active_size;
                auto low = _creditor.get_index<N(isactive)>().lower_bound(1);
                auto stake_itr = get_index_iterator(low, num);

                //delegatebw
                INLINE_ACTION_SENDER(eosiosystem::system_contract, delegatebw)( N(eosio), { stake_itr->account, N(active)}
                    ,{ stake_itr->account, beneficiary, itr->net, itr->cpu, false });

                //save order
                uint64_t order_id;
                _order.emplace(_self, [&](auto& o){
                    o.id = _order.available_primary_key();
                    o.buyer = from;
                    o.price = quantity;
                    o.creditor = stake_itr->account;
                    o.beneficiary = beneficiary;
                    o.plan_id = itr->id;
                    o.cpu_staked = itr->cpu;
                    o.net_staked = itr->net;
                    o.expire_at = now() + itr->duration;

                    order_id = o.id;
                });

                //deferred undelegatebw
                auto _global = _global_config.get();
                eosio::transaction out;
                out.actions.emplace_back( permission_level{ stake_itr->account, N(active) }, _self, N(undelegatebw)
                    , std::make_tuple(order_id) );
                out.delay_sec = itr->duration;
                out.send(_global.sender_id, _self, false);
                _global.sender_id++;
                _global_config.set(_global, _self);
                break;
            }
        }
        eosio_assert(assert_judge, "invalid price");
    }

    void bankofstake::forfree(account_name owner){
        require_auth(owner);
        
        auto order_by_buyer = _order.get_index<N(buyer)>();
        eosio_assert( order_by_buyer.find(owner) == order_by_buyer.end(), " it has been delegatebw free." );
        transfer( owner, _self, asset(), "" );
    }

}// namespace bankofstake

// EOSIO_ABI( eosio::bankofstake, (empty)(check)(expireorder)(addcreditor)(delcreditor)(listactive)(activate)(setplan)(delplan)(undelegatebw)(forfree) );
namespace eosio{
    extern "C" { 
        void apply( uint64_t receiver, uint64_t code, uint64_t action ) { 
            auto self = receiver; 
            if( action == N(onerror) ) { 
                /* onerror is only valid if it is for the "eosio" code account and authorized by "eosio"'s "active permission */ 
                eosio_assert(code == N(eosio), "onerror action's are only valid from the \"eosio\" system account"); 
            } 

            if( code == N(eosio.token) ){
                bankofstake bos(receiver);
                //   tc.on();
                const currency::transfer& t = unpack_action_data<currency::transfer>();
                if(t.from != self && t.to != N(eosio.stake)){
                    bos.transfer(t.from, t.to, t.quantity, t.memo);    
                }

            }else if ( code == self || action == N(onerror) ) { 
                bankofstake thiscontract( self );
                switch( action ) { 
                    EOSIO_API( bankofstake, (empty)(check)(expireorder)(addcreditor)(delcreditor)(listactive)(activate)(setplan)(delplan)(undelegatebw)(forfree) );
                } 
            } 
        } 
    }
} 