#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/transaction.hpp>

namespace eosio{

    using std::string;

    class onerror_test : public contract {
        public:
            onerror_test( account_name _self ):contract(_self){};
            ~onerror_test(){};

            //@abi action
            void overtime( uint64_t id ){
                    eosio_assert(false,"hi");
            }

            //@abi action
            void sendderr( uint64_t id ){
                cancel_deferred( _self );
                transaction out;
                out.actions.emplace_back( permission_level{ _self, N(active) }, _self, N(overtime)
                    , std::make_tuple(id) );
                out.delay_sec =  2;
                out.send(uint128_t(_self) << 64 | id, _self, true);
            }

            void resend_overtime( transaction out, uint64_t id ){
                auto now_time = now();
                auto delay_sec = 2;
                out.delay_sec = delay_sec;
                out.expiration = time_point_sec(now() + delay_sec + 60);
                out.send( uint128_t(_self) << 64 | id, _self, true );

            }

    };

    extern "C" { 
        void apply( uint64_t receiver, uint64_t code, uint64_t action ) { 
            auto self = receiver; 
            if( code == N(eosio) && action == N(onerror) ) { 
                print("hi");
                const auto act_data = unpack_action_data<onerror>();
                auto sender = uint64_t( act_data.sender_id >> 64);
                if( sender == self){
                    onerror_test bos(self);
                    bos.resend_overtime( act_data.unpack_sent_trx(), uint64_t( act_data.sender_id) );
                }
            } else if ( code == self || action == N(onerror) ) { 
                onerror_test thiscontract( self );
                switch( action ) { 
                    EOSIO_API( onerror_test, (overtime)(sendderr) );
                } 
            } 
        } 
    }

}// namespace eosio
