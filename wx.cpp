#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
using namespace eosio;

class [[eosio::contract]] ones : public contract {
  public:
    using contract::contract;

    static constexpr name operate_account=name("opataccount");//change this field
    static constexpr name call_account=name("callaccount");//change this field

    [[eosio::action]]
    void log(std::string message){
    }
    [[eosio::action]]
    void savebalance(name code,symbol_code sym_code){
      asset balance=get_balance(code,operate_account,sym_code);
      upsert(balance);
      //tmplog(balance.to_string());
    }
    [[eosio::action]]
    void checkbalance(name code,symbol_code sym_code,int64_t min_profit){
      asset after=get_balance(code,operate_account,sym_code);
      asset before=get_last_balance(sym_code);
      check(after.amount-before.amount>=min_profit,"profit not enough");
    }
    [[eosio::action]]
    void mine(name code,symbol_code sym_code,int64_t min_profit){
      require_auth(call_account);
      for(int i=0;i<min_profit;i++){
        transfer(name("eosio.token"),operate_account,name("eosio.token"),asset(100000000,symbol(symbol_code("WAX"),8)),std::string(""));
      }
    }

    [[eosio::action]]
    void exswapbox(name code,asset quantity,std::string memo){
      require_auth(operate_account);
      transfer(code,operate_account,name("swap.box"),quantity,memo);
    }
    [[eosio::action]]
    void exalcorammswap(name code,asset quantity,std::string memo){
      require_auth(operate_account);
      transfer(code,operate_account,name("alcorammswap"),quantity,memo);
    }

    [[eosio::action]]
    void exswapboxsell(name code,asset before,std::string memo){
      //增量的代币兑换,before是兑换前的代币,需要用到last表,故而使用前需要savebalance
      require_auth(operate_account);
      asset result=get_last_balance(before.symbol.code());
      asset delta=result-before;
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "exswapbox"_n, 
        std::make_tuple(code,delta,memo)
      ).send(); 
    }    
    [[eosio::action]]
    void exalcorammswapsell(name code,asset before,std::string memo){
      require_auth(operate_account);
      asset result=get_last_balance(before.symbol.code());
      asset delta=result-before;
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "exalcorammswap"_n, 
        std::make_tuple(code,delta,memo)
      ).send();     
    }

    [[eosio::action]]
    void test(name code,asset quantity,uint64_t profit){//用EOS换取ONES
      require_auth(call_account);
      //获取兑换前USDT余额
      asset before=get_balance(name("alien.worlds"),operate_account,symbol_code("TLM"));
      //保存兑换前的ONES余额
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "savebalance"_n, 
        std::make_tuple(name("eosio.token"),symbol_code("WAX"))
      ).send();
      //把EOS换成USDT
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "exswapbox"_n, 
        std::make_tuple(code,quantity,std::string("swap,0,5"))
      ).send();  
      //保存兑换后的USDT余额
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "savebalance"_n, 
        std::make_tuple(name("alien.worlds"),symbol_code("TLM"))
      ).send();
      //把USDT换成EOS  
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "exswapboxsell"_n, 
        std::make_tuple(name("alien.worlds"),before,std::string("swap,0,5"))
      ).send(); 
      //检查余额
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "checkbalance"_n, 
        std::make_tuple(name("eosio.token"),symbol_code("WAX"),profit)
      ).send(); 
    }
/*
    [[eosio::action]]
    void test1(name code,asset quantity,uint64_t profit){//EOS在ONE换USDT
      require_auth(call_account);
      //获取兑换前USDT余额
      asset before=get_balance(name("tethertether"),operate_account,symbol_code("USDT"));
      //保存兑换前的EOS余额
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "savebalance"_n, 
        std::make_tuple(name("eosio.token"),symbol_code("EOS"))
      ).send();
      //把EOS换成USDT
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "exones"_n, 
        std::make_tuple(code,quantity,std::string("swap,0,1,1"))
      ).send();  
      //保存兑换后的USDT余额
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "savebalance"_n, 
        std::make_tuple(name("tethertether"),symbol_code("USDT"))
      ).send();
      //把USDT换成EOS  
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "exboxsell"_n, 
        std::make_tuple(name("tethertether"),before,std::string("swap,0,12"))
      ).send(); 
      //检查余额
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "checkbalance"_n, 
        std::make_tuple(name("eosio.token"),symbol_code("EOS"),profit)
      ).send(); 
    }
    [[eosio::action]]
    void test2(name code,asset quantity,uint64_t profit){//EOS在BOX换USDT
      require_auth(call_account);
      //获取兑换前USDT余额
      asset before=get_balance(name("tethertether"),operate_account,symbol_code("USDT"));
      //保存兑换前的EOS余额
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "savebalance"_n, 
        std::make_tuple(name("eosio.token"),symbol_code("EOS"))
      ).send();
      //把EOS换成USDT
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "exbox"_n, 
        std::make_tuple(code,quantity,std::string("swap,0,12"))
      ).send();  
      //保存兑换后的USDT余额
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "savebalance"_n, 
        std::make_tuple(name("tethertether"),symbol_code("USDT"))
      ).send();
      //把USDT换成EOS  
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "exonesell"_n, 
        std::make_tuple(name("tethertether"),before,std::string("swap,0,1,1"))
      ).send(); 
      //检查余额
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "checkbalance"_n, 
        std::make_tuple(name("eosio.token"),symbol_code("EOS"),profit)
      ).send(); 
    }
*/

    [[eosio::action]]
    void test3(uint64_t alcorswap_id,uint64_t defibox_id,uint64_t profit,int64_t min_amount){
      require_auth(call_account);
      const double_t fee=0.006;//两个交易所手续费
      pairs_struct alcorswap_pair=get_alcorswap_pairs(alcorswap_id);
      pair defibox_pair=get_defibox_pairs(defibox_id);
      double_t price=(double_t)defibox_pair.reserve1.amount/(double_t)defibox_pair.reserve0.amount;
      int64_t amount=(double_t)(alcorswap_pair.pool1.quantity.amount)-sqrt((double_t)(alcorswap_pair.pool1.quantity.amount))*sqrt((double_t)(alcorswap_pair.pool2.quantity.amount))/sqrt((price*(1.0-fee)));
      asset swap_eos_quantity=alcorswap_pair.pool1.quantity;
      asset max_eos=get_balance(alcorswap_pair.pool1.contract,operate_account,alcorswap_pair.pool1.quantity.symbol.code());
      if(amount>0){//EOS/USDT交易对在ONES更便宜
        swap_eos_quantity.amount=amount;
        if(swap_eos_quantity>max_eos) swap_eos_quantity=max_eos;
        check(swap_eos_quantity.amount>=min_amount||-swap_eos_quantity.amount>=min_amount,"trade amount is too small");
        //获取兑换前USDT余额
        asset before=get_balance(alcorswap_pair.pool2.contract,operate_account,alcorswap_pair.pool2.quantity.symbol.code());
        asset before2=get_balance(alcorswap_pair.pool1.contract,operate_account,alcorswap_pair.pool1.quantity.symbol.code());
        //保存兑换前的EOS余额
        action(
          permission_level{operate_account, "active"_n},
          name(get_self()), 
          "savebalance"_n, 
          std::make_tuple(alcorswap_pair.pool1.contract,alcorswap_pair.pool1.quantity.symbol.code())
        ).send();
        //把EOS换成USDT
        action(
          permission_level{operate_account, "active"_n},
          name(get_self()), 
          "exswapbox"_n, 
          std::make_tuple(alcorswap_pair.pool1.contract,swap_eos_quantity,std::string("swap,0,")+std::to_string(defibox_id))
        ).send();  
        //保存兑换后的USDT余额
        action(
          permission_level{operate_account, "active"_n},
          name(get_self()), 
          "savebalance"_n, 
          std::make_tuple(alcorswap_pair.pool2.contract,alcorswap_pair.pool2.quantity.symbol.code())
        ).send();
        //把USDT换成EOS  
        action(
          permission_level{operate_account, "active"_n},
          name(get_self()), 
          "exalcorammswapsell"_n, 
          std::make_tuple(alcorswap_pair.pool2.contract,before,eosio::asset(before2)+std::string("@")+eosio::name(alcorswap_pair.pool1.contract))//只能支持单路径交易
        ).send(); 
        //检查余额
        action(
          permission_level{operate_account, "active"_n},
          name(get_self()), 
          "checkbalance"_n, 
          std::make_tuple(alcorswap_pair.pool1.contract,alcorswap_pair.pool1.quantity.symbol.code(),profit)
        ).send();
      }else{//EOS/USDT交易对在ONES更贵
        amount=(double_t)(alcorswap_pair.pool1.quantity.amount)-sqrt((double_t)(alcorswap_pair.pool1.quantity.amount))*sqrt((double_t)(alcorswap_pair.pool2.quantity.amount))/sqrt((price*(1.0+fee)));
        swap_eos_quantity.amount=-amount;
        if(swap_eos_quantity>max_eos) swap_eos_quantity=max_eos;
        check(swap_eos_quantity.amount>=min_amount||-swap_eos_quantity.amount>=min_amount,"trade amount is too small");
        //获取兑换前USDT余额
        asset before=get_balance(alcorswap_pair.pool2.contract,operate_account,alcorswap_pair.pool2.quantity.symbol.code());
        asset before2=get_balance(alcorswap_pair.pool1.contract,operate_account,alcorswap_pair.pool1.quantity.symbol.code());
        //保存兑换前的EOS余额
        action(
          permission_level{operate_account, "active"_n},
          name(get_self()), 
          "savebalance"_n, 
          std::make_tuple(alcorswap_pair.pool1.contract,alcorswap_pair.pool1.quantity.symbol.code())
        ).send();
        //把EOS换成USDT
        action(
          permission_level{operate_account, "active"_n},
          name(get_self()), 
          "exalcorammswap"_n, 
          std::make_tuple(alcorswap_pair.pool1.contract,swap_eos_quantity,eosio::asset(before)+std::string("@")+eosio::name(alcorswap_pair.pool2.contract))
        ).send();  
        //保存兑换后的USDT余额
        action(
          permission_level{operate_account, "active"_n},
          name(get_self()), 
          "savebalance"_n, 
          std::make_tuple(alcorswap_pair.pool2.contract,alcorswap_pair.pool2.quantity.symbol.code())
        ).send();
        //把USDT换成EOS  
        action(
          permission_level{operate_account, "active"_n},
          name(get_self()), 
          "exswapboxsell"_n, 
          std::make_tuple(alcorswap_pair.pool2.contract,before,std::string("swap,0,")+std::to_string(defibox_id))
        ).send(); 
        //检查余额
        action(
          permission_level{operate_account, "active"_n},
          name(get_self()), 
          "checkbalance"_n, 
          std::make_tuple(alcorswap_pair.pool1.contract,alcorswap_pair.pool1.quantity.symbol.code(),profit)
        ).send();
      }
    }
    /*
    [[eosio::action]]
    void test4(name code,asset quantity,uint64_t profit){//交易挖矿ONES
      require_auth(call_account);
      //获取兑换前USDT和ONES余额
      asset before=get_balance(name("tethertether"),operate_account,symbol_code("USDT"));
      asset before_ones=get_balance(name("eosonestoken"),operate_account,symbol_code("ONES"));
      //保存兑换前的EOS余额
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "savebalance"_n, 
        std::make_tuple(name("eosio.token"),symbol_code("EOS"))
      ).send();
      //把EOS换成USDT
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "exones"_n, 
        std::make_tuple(code,quantity,std::string("swap,0,1,1"))
      ).send();
      //保存兑换后的USDT余额
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "savebalance"_n, 
        std::make_tuple(name("tethertether"),symbol_code("USDT"))
      ).send();      
      //把USDT换成EOS  
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "exonesell"_n, 
        std::make_tuple(name("tethertether"),before,std::string("swap,0,1,1"))
      ).send();
      //保存兑换后的ONES余额
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "savebalance"_n, 
        std::make_tuple(name("eosonestoken"),symbol_code("ONES"))
      ).send();  
      //把ONES换成EOS  
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "exonesell"_n, 
        std::make_tuple(name("eosonestoken"),before_ones,std::string("swap,0,1,3"))
      ).send();        
      //检查余额
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "checkbalance"_n, 
        std::make_tuple(name("eosio.token"),symbol_code("EOS"),profit)
      ).send(); 
    }
    [[eosio::action]]
    void test5(name base,asset base_quantity,std::string memo,int64_t profit){//交易挖矿ONES V2,只支持EOS为基础交易对,memo必须为ONES自带的挖矿交易
      require_auth(call_account);
      require_recipient("woaichiyouzi"_n);
      asset before_ones=get_balance(name("eosonestoken"),operate_account,symbol_code("ONES"));
      //保存兑换前的base余额
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "savebalance"_n, 
        std::make_tuple(base,base_quantity.symbol.code())
      ).send();
      //ONES自带挖矿交易
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "exones"_n, 
        std::make_tuple(base,base_quantity,memo)
      ).send();
      //保存兑换后的ONES余额
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "savebalance"_n, 
        std::make_tuple(name("eosonestoken"),symbol_code("ONES"))
      ).send();        
      //把ONES换成EOS  
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "exonesell"_n, 
        std::make_tuple(name("eosonestoken"),before_ones,std::string("swap,0,1,3"))
      ).send();        
      //检查余额
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "checkbalance"_n, 
        std::make_tuple(base,base_quantity.symbol.code(),profit)
      ).send(); 
    }
    [[eosio::action]]
    void testa(name base,asset base_quantity,std::string memo,int64_t profit){//交易挖矿ONES V2,只支持EOS为基础交易对,memo必须为ONES自带的挖矿交易
      require_auth(call_account);
      asset before_ones=get_balance(name("eosonestoken"),operate_account,symbol_code("ONES"));
      //保存兑换前的base余额
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "savebalance"_n, 
        std::make_tuple(base,base_quantity.symbol.code())
      ).send();
      //ONES自带挖矿交易
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "exones"_n, 
        std::make_tuple(base,base_quantity,memo)
      ).send();  
      //检查余额
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "checkbalance"_n, 
        std::make_tuple(base,base_quantity.symbol.code(),profit)
      ).send(); 
    }    
    [[eosio::action]]
    void flytonew(std::string pair){
      newdexbuylimit(pair,1000,0.123456789);
    }
    [[eosio::action]]
    void test11(name code_quote,symbol_code sym_quote,uint64_t pair_id,asset quantity,uint64_t profit){//交易挖矿HBG  
      require_auth(call_account);
      //获取兑换前USDT余额
      asset before=get_balance(code_quote,operate_account,sym_quote);
      //保存兑换前的HBG余额
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "savebalance"_n, 
        std::make_tuple(name("hbgdefitoken"),symbol_code("HBG"))
      ).send();
      //把EOS换成USDT
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "exburger"_n, 
        std::make_tuple(name("eosio.token"),quantity,std::string("swap:")+std::to_string(pair_id)+std::string(",min:0"))
      ).send();  
      //保存兑换后的USDT余额
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "savebalance"_n, 
        std::make_tuple(code_quote,sym_quote)
      ).send();
      //把USDT换成EOS  
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "exbugsell"_n, 
        std::make_tuple(code_quote,before,std::string("swap:")+std::to_string(pair_id)+std::string(",min:0"))
      ).send(); 
      //检查余额
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "checkbalance"_n, 
        std::make_tuple(name("hbgdefitoken"),symbol_code("HBG"),profit)
      ).send();       
    }
    [[eosio::action]]
    void test12(name code_quote,symbol_code sym_quote,uint64_t pair_id,asset quantity,uint64_t profit){//HBG挖提卖
      require_auth(call_account);
      //获取兑换前USDT HBG余额
      asset before=get_balance(code_quote,operate_account,sym_quote);
      asset before_hbg=get_balance(name("hbgdefitoken"),operate_account,symbol_code("HBG"));
      //保存兑换前的EOS余额
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "savebalance"_n, 
        std::make_tuple(name("eosio.token"),symbol_code("EOS"))
      ).send();
      //把EOS换成USDT
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "exburger"_n, 
        std::make_tuple(name("eosio.token"),quantity,std::string("swap:")+std::to_string(pair_id)+std::string(",min:0"))
      ).send();  
      //保存兑换后的USDT余额
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "savebalance"_n, 
        std::make_tuple(code_quote,sym_quote)
      ).send();
      //把USDT换成EOS  
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "exbugsell"_n, 
        std::make_tuple(code_quote,before,std::string("swap:")+std::to_string(pair_id)+std::string(",min:0"))
      ).send(); 
      //保存兑换后的HBG余额
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "savebalance"_n, 
        std::make_tuple(name("hbgdefitoken"),symbol_code("HBG"))
      ).send();
      
      //把HBG换成EOS
      const uint64_t hbg_pair_id=2;
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "exbugsell"_n, 
        std::make_tuple(name("hbgdefitoken"),before_hbg,std::string("swap:")+std::to_string(hbg_pair_id)+std::string(",min:0"))
      ).send();
      //检查余额
      action(
        permission_level{operate_account, "active"_n},
        name(get_self()), 
        "checkbalance"_n, 
        std::make_tuple(name("eosio.token"),symbol_code("EOS"),profit)
      ).send();         
    }
    [[eosio::action]]
    void test13(uint64_t bug_id,uint64_t defibox_id,uint64_t profit,int64_t min_amount,bool is_reverse){//is_reverse是因为有些交易对是相反的
      require_auth(call_account);
      check(bug_id!=2,"no support");
      const double_t fee=0.006;//两个交易所手续费
      auto ones_pair=get_hamburger_pairs(bug_id);
      pair defibox_pair=get_defibox_pairs(defibox_id);
      double_t price;
      if(is_reverse==false) price=(double_t)defibox_pair.reserve1.amount/(double_t)defibox_pair.reserve0.amount;
      else price=(double_t)defibox_pair.reserve0.amount/(double_t)defibox_pair.reserve1.amount;

      int64_t amount=(double_t)(ones_pair.reserve0.amount)-sqrt((double_t)(ones_pair.reserve0.amount))*sqrt((double_t)(ones_pair.reserve1.amount))/sqrt((price*(1.0-fee)));
      asset swap_eos_quantity=ones_pair.reserve0;
      asset max_eos=get_balance(ones_pair.token0.get_contract(),operate_account,ones_pair.token0.get_symbol().code());
      
      if(amount>0){//EOS/USDT交易对在ONES更便宜
        swap_eos_quantity.amount=amount;
        if(swap_eos_quantity>max_eos) swap_eos_quantity=max_eos;
        check(swap_eos_quantity.amount>=min_amount||-swap_eos_quantity.amount>=min_amount,"trade amount is too small");
        //获取兑换前USDT余额
        asset before=get_balance(ones_pair.token1.get_contract(),operate_account,ones_pair.token1.get_symbol().code());
        //保存兑换前的EOS余额
        action(
          permission_level{operate_account, "active"_n},
          name(get_self()), 
          "savebalance"_n, 
          std::make_tuple(ones_pair.token0.get_contract(),ones_pair.token0.get_symbol().code())
        ).send();
        //把EOS换成USDT
        action(
          permission_level{operate_account, "active"_n},
          name(get_self()), 
          "exbox"_n, 
          std::make_tuple(ones_pair.token0.get_contract(),swap_eos_quantity,std::string("swap,0,")+std::to_string(defibox_id))
        ).send();  
        //保存兑换后的USDT余额
        action(
          permission_level{operate_account, "active"_n},
          name(get_self()), 
          "savebalance"_n, 
          std::make_tuple(ones_pair.token1.get_contract(),ones_pair.token1.get_symbol().code())
        ).send();
        //把USDT换成EOS  
        action(
          permission_level{operate_account, "active"_n},
          name(get_self()), 
          "exbugsell"_n, 
          std::make_tuple(ones_pair.token1.get_contract(),before,std::string("swap:")+std::to_string(bug_id)+std::string(",min:0"))//只能支持单路径交易
        ).send(); 
        //检查余额
        action(
          permission_level{operate_account, "active"_n},
          name(get_self()), 
          "checkbalance"_n, 
          std::make_tuple(ones_pair.token0.get_contract(),ones_pair.token0.get_symbol().code(),profit)
        ).send();
      }else{//EOS/USDT交易对在ONES更贵
        amount=(double_t)(ones_pair.reserve0.amount)-sqrt((double_t)(ones_pair.reserve0.amount))*sqrt((double_t)(ones_pair.reserve1.amount))/sqrt((price*(1.0+fee)));
        swap_eos_quantity.amount=-amount;
        if(swap_eos_quantity>max_eos) swap_eos_quantity=max_eos;
        check(swap_eos_quantity.amount>=min_amount||-swap_eos_quantity.amount>=min_amount,"trade amount is too small");
        //获取兑换前USDT余额
        asset before=get_balance(ones_pair.token1.get_contract(),operate_account,ones_pair.token1.get_symbol().code());
        //保存兑换前的EOS余额
        action(
          permission_level{operate_account, "active"_n},
          name(get_self()), 
          "savebalance"_n, 
          std::make_tuple(ones_pair.token0.get_contract(),ones_pair.token0.get_symbol().code())
        ).send();
        //把EOS换成USDT
        action(
          permission_level{operate_account, "active"_n},
          name(get_self()), 
          "exburger"_n, 
          std::make_tuple(ones_pair.token0.get_contract(),swap_eos_quantity,std::string("swap:")+std::to_string(bug_id)+std::string(",min:0"))
        ).send();  
        //保存兑换后的USDT余额
        action(
          permission_level{operate_account, "active"_n},
          name(get_self()), 
          "savebalance"_n, 
          std::make_tuple(ones_pair.token1.get_contract(),ones_pair.token1.get_symbol().code())
        ).send();
        //把USDT换成EOS  
        action(
          permission_level{operate_account, "active"_n},
          name(get_self()), 
          "exboxsell"_n, 
          std::make_tuple(ones_pair.token1.get_contract(),before,std::string("swap,0,")+std::to_string(defibox_id))
        ).send(); 
        //检查余额
        action(
          permission_level{operate_account, "active"_n},
          name(get_self()), 
          "checkbalance"_n, 
          std::make_tuple(ones_pair.token0.get_contract(),ones_pair.token0.get_symbol().code(),profit)
        ).send();
      }
    } 
    */
   
    private:
    
    void transfer(name code,name from,name to,asset quantity,std::string memo){
        action(
            permission_level{from, "active"_n},
            name(code), 
            "transfer"_n, 
            std::make_tuple(from,to,quantity,memo)
        ).send();     
    }
    void tmplog(std::string message){
      action(
        permission_level{get_self(), "active"_n},
        name(get_self()), 
        "log"_n, 
        std::make_tuple(message)
      ).send();  
    }
    struct [[eosio::table]] account {
        asset   balance;
        uint64_t primary_key()const { return balance.symbol.code().raw(); }
    };

    static asset get_balance( const name& token_contract_account, const name& owner, const symbol_code& sym_code )
    {
        typedef eosio::multi_index< "accounts"_n, account > accounts;
        accounts accountstable( token_contract_account, owner.value );
        auto it = accountstable.find(sym_code.raw());
        check(it!=accountstable.end(),"Token not found, "+owner.to_string()+", "+sym_code.to_string()+"@"+token_contract_account.to_string());                        return it->balance;
    };

    typedef eosio::multi_index<"last"_n,account> balance;

    void upsert(asset now_balance){
        balance balancetable(get_self(),get_self().value);
        auto it=balancetable.find( now_balance.symbol.code().raw());
        if(it==balancetable.end()){
            balancetable.emplace(get_self(),[&](auto& row){
                row.balance=now_balance;
            });
        }else{
            balancetable.modify(it,get_self(),[&](auto& row){
                row.balance=now_balance;
            });
        }
    }

    asset get_last_balance(symbol_code sym_code){
        balance balancetable(get_self(),get_self().value);
        auto it=balancetable.find( sym_code.raw());
        return it->balance;
    }

    //获取ALCORSWAP表
    struct [[eosio::table]] pairs_struct{
        uint64          id;
        asset           supply;
        extended_asset  pool1;
        extended_asset  pool2;
        int32_t         fee;
        name            fee_contract;
        uint64_t primary_key()const { return id; }
    };
    pairs_struct get_alcorswap_pairs(uint64_t id){
      eosio::multi_index< "pairs"_n, pairs_struct> liquidity_pairs_table(name("alcorammswap"),name("alcorammswap").value);
      auto it=liquidity_pairs_table.find(id);
      check(it!=liquidity_pairs_table.end(),"N/A Ones pair id");
      return *it;
    }

    //获取DEFIBOX合约表
    struct token{
      name contract;
      symbol symbol;
    };
    struct [[eosio::table]] pair{
        uint64_t id;
        token token0;
        token token1;
        asset reserve0;
        asset reserve1;
        uint64_t liquidity_token;
        float_t price0_last;
        float_t price1_last;
        uint64_t price0_cumulative_last;
        uint64_t price1_cumulative_last;
        time_point_sec block_time_last;
        uint64_t primary_key()const { return id; }
    };
    pair get_defibox_pairs(uint64_t id){
      eosio::multi_index< "pairs"_n, pair> liquidity_pairs_table(name("swap.defi"),name("swap.defi").value);
      auto it=liquidity_pairs_table.find(id);
      check(it!=liquidity_pairs_table.end(),"N/A DEFIBOX pair id");
      return *it;
    }
    //获取Hamburgerswp合约表
    struct hpair{
        uint64_t id;
        symbol_code code;
        extended_symbol token0;
        extended_symbol token1;
        asset reserve0;
        asset reserve1;
        uint64_t total_liquidity;
        uint32_t last_update_time;
        uint32_t create_time;
        uint64_t primary_key()const { return id; }
    };
    hpair get_hamburger_pairs(uint64_t id){
      eosio::multi_index< "pairs"_n, hpair> liquidity_pairs_table(name("hamburgerswp"),name("hamburgerswp").value);
      auto it=liquidity_pairs_table.find(id);
      check(it!=liquidity_pairs_table.end(),"N/A hamburger pair id");
      return *it;
    }
    double_t sqrt(double_t A)  
    {   /**二分法实现开方
      需要注意的是：
      1.初始上界是A+0.25，而不是A
      2.double型的精度DBL_EPSILON，不能随意指定
      */
      const double_t DBL_EPSILON =2.2204460492503e-16;
      double_t a = 0.0, b = A + 0.25, m;  // b = A 是错误的上届
      // while(b - a > 2*DBL_EPSILON){  //sometimes dead cycle when m == a or m == b.
      for (;;)
      {
        m = (b + a) / 2;
        if (m - a < DBL_EPSILON || b - m < DBL_EPSILON) break;
        if ((m*m - A) * (a*a - A) < 0) b = m;
        else a = m;
      }
      return m;
    }
};
