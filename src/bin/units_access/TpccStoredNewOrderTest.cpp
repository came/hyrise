// Copyright (c) 2012 Hasso-Plattner-Institut fuer Softwaresystemtechnik GmbH. All rights reserved.
#include "TpccStoredProceduresTest.h"

namespace hyrise {
namespace access {

class TpccStoredNewOrderTest : public TpccStoredProceduresTest {

protected:
  Json::Value newOrderData(int w_id, int d_id, int c_id, int o_carrier_id, const std::string& ol_dist_info, item_list_t items);
  Json::Value doNewOrder(int w_id, int d_id, int c_id, int o_carrier_id, const std::string& ol_dist_info, item_list_t items);
};

Json::Value TpccStoredNewOrderTest::newOrderData(int w_id, int d_id, int c_id, int o_carrier_id, const std::string& ol_dist_info, item_list_t items) {
  Json::Value data;
  data["W_ID"] = w_id;
  data["D_ID"] = d_id;
  data["C_ID"] = c_id;
  data["O_CARRIER_ID"] = o_carrier_id;
  data["OL_DIST_INFO"] = ol_dist_info;

  Json::Value itemData(Json::arrayValue);
  for (size_t i = 0; i < items.size(); ++i) {
     Json::Value item;
     item["I_ID"] = items.at(i).id;
     item["I_W_ID"] = items.at(i).wid;
     item["quantity"] = items.at(i).quantity;
     itemData.append(item);
  }
  data["items"] = itemData;

  return data;
}

Json::Value TpccStoredNewOrderTest::doNewOrder(int w_id, int d_id, int c_id, int o_carrier_id, const std::string& ol_dist_info,
                                                 item_list_t items) {
  const auto data = newOrderData(w_id, d_id, c_id, o_carrier_id, ol_dist_info, items);

  return doStoredProcedure(data, "TPCC-NewOrder");
}



#define T_NewOrder(w_id, d_id, c_id, o_carrier_id, ol_dist_info, itemlist, o_id) \
{\
  const auto response = doNewOrder(w_id, d_id, c_id, o_carrier_id, ol_dist_info, itemlist);\
\
  EXPECT_EQ(w_id, getValuei(response, "W_ID"));\
  EXPECT_EQ(d_id, getValuei(response, "D_ID"));\
  EXPECT_EQ(c_id, getValuei(response, "C_ID"));\
  EXPECT_EQ("CLName" + toString(c_id), getValues(response, "C_LAST"));\
  /*TODO check C_CREDIT*/\
  EXPECT_FLOAT_EQ(0.1 * w_id + 0.01 * c_id, getValuef(response, "C_DISCOUNT"));\
  EXPECT_FLOAT_EQ(0.1 * w_id, getValuef(response, "W_TAX"));\
  EXPECT_FLOAT_EQ(0.01 * d_id, getValuef(response, "D_TAX"));\
  EXPECT_EQ(itemlist.size(), getValuei(response, "O_OL_CNT"));\
  EXPECT_EQ(o_id, getValuei(response, "O_ID"));\
  getValues(response, "O_ENTRY_D");/*TODO check for value*/\
  \
  const Json::Value& items = response["items"];\
  EXPECT_EQ(itemlist.size(), items.size());\
  for (size_t i = 0; i < itemlist.size(); ++i) {\
    const Json::Value& cur = items[(int)i];\
    /*TODO this expects the response list to have the same order as the input list ...*/\
    EXPECT_EQ(itemlist.at(i).wid, getValuei(cur, "OL_SUPPLY_W_ID"));\
    EXPECT_EQ(itemlist.at(i).id, getValuei(cur, "OL_I_ID"));\
    EXPECT_EQ("IName" + toString(itemlist.at(i).id), getValues(cur, "I_NAME"));\
    EXPECT_EQ(itemlist.at(i).quantity, getValuei(cur, "OL_QUANTITY"));\
    getValuei(cur, "S_QUANTITY");/*TODO check for value*/\
    getValues(cur, "brand-generic");/*TODO check for value*/\
    EXPECT_FLOAT_EQ(1.01 * itemlist.at(i).id, getValuef(cur, "I_PRICE"));\
    getValuef(cur, "OL_AMOUNT");/*TODO check for value*/\
  }\
  EXPECT_EQ(getTable(Customer)->size() , i_customer_size);\
  EXPECT_EQ(getTable(Orders)->size()   , i_orders_size + 1);\
  i_orders_size += 1;\
  EXPECT_EQ(getTable(OrderLine)->size(), i_orderLine_size + itemlist.size());\
  i_orderLine_size += itemlist.size();\
  EXPECT_EQ(getTable(Warehouse)->size(), i_warehouse_size);\
  EXPECT_EQ(getTable(NewOrder)->size() , i_newOrder_size + 1);\
  i_newOrder_size += 1;\
  EXPECT_EQ(getTable(District)->size() , i_district_size);\
  EXPECT_EQ(getTable(Item)->size()     , i_item_size);\
  EXPECT_EQ(getTable(Stock)->size()    , i_stock_size);\
  EXPECT_EQ(getTable(History)->size()  , i_history_size);\
}

TEST_F(TpccStoredNewOrderTest, NewOrder) {
//                           {{i_id, i_w_id, quantity}}
  const item_list_t items1 = {{1   , 1     , 1       },
                              {2   , 1     , 2       },
                              {3   , 1     , 3       },
                              {4   , 1     , 4       },
                              {5   , 1     , 5       }}; // 5 items all local
  const item_list_t items2 = {{1   , 1     , 10      },
                              {2   , 1     , 10      },
                              {3   , 1     , 10      },
                              {4   , 1     , 10      },
                              {5   , 1     , 10      },
                              {6   , 1     , 10      },
                              {7   , 1     , 10      },
                              {8   , 1     , 10      },
                              {9   , 1     , 10      },
                              {10  , 1     , 10      },
                              {11  , 1     , 10      },
                              {12  , 1     , 10      },
                              {13  , 1     , 10      },
                              {14  , 1     , 10      },
                              {15  , 1     , 10      }}; // 15 items remote warehouses*/
//          (w_id, d_id, c_id, o_carrier_id, ol_dist_info, itemlist, o_id)
  T_NewOrder(1   , 1   , 1   , 1           , "info1"     , items1  , 6   ); //1st
  T_NewOrder(1   , 2   , 1   , 1           , "info2"     , items1  , 5   ); //1st
  T_NewOrder(2   , 1   , 1   , 1           , "info3"     , items1  , 3   ); //1st
  T_NewOrder(2   , 10  , 1   , 1           , "info4"     , items2  , 3   ); //1st
  T_NewOrder(1   , 2   , 1   , 1           , "info5"     , items2  , 6   ); //2nd
  T_NewOrder(2   , 1   , 1   , 1           , "info6"     , items2  , 4   ); //2nd
}

TEST_F(TpccStoredNewOrderTest, NewOrder_wrongItemCount) {
  //                     (w_id, d_id, c_id, o_carrier_id, ol_dist_info, {{i_id, i_w_id, quantity}});
  EXPECT_THROW(doNewOrder(1   , 1   , 1   , 1           , "info"      , {{1   , 1     , 1       },
                                                                         {2   , 1     , 1       },
                                                                         {3   , 1     , 1       },
                                                                         {4   , 1     , 1       }}), TpccError); // 4 items

  EXPECT_THROW(doNewOrder(1   , 3   , 2   , 1           , "info"      , {{1   , 1     , 1       },
                                                                         {2   , 1     , 1       },
                                                                         {3   , 1     , 1       },
                                                                         {4   , 1     , 1       },
                                                                         {5   , 1     , 1       },
                                                                         {6   , 1     , 1       },
                                                                         {7   , 1     , 1       },
                                                                         {8   , 1     , 1       },
                                                                         {9   , 1     , 1       },
                                                                         {10  , 1     , 1       },
                                                                         {11  , 1     , 1       },
                                                                         {12  , 1     , 1       },
                                                                         {13  , 1     , 1       },
                                                                         {14  , 1     , 1       },
                                                                         {15  , 1     , 1       },
                                                                         {16  , 1     , 1       }}), TpccError); //16 items
}

TEST_F(TpccStoredNewOrderTest, NewOrder_withRollback) {
  //TODO New Order with Rollback
}

TEST_F(TpccStoredNewOrderTest, NewOrder_wrongQuantity) {
  //                     (w_id, d_id, c_id, o_carrier_id, ol_dist_info, {{i_id, i_w_id, quantity}});
  EXPECT_THROW(doNewOrder(1   , 1   , 1   , 1           , "info"      , {{1   , 1     , 1       },
                                                                         {2   , 1     , 1       },
                                                                         {3   , 1     , 1       },
                                                                         {4   , 1     , 1       },
                                                                         {5   , 1     , 11      }, }), TpccError);
  
  EXPECT_THROW(doNewOrder(1   , 1   , 5   , 1           , "info"      , {{1   , 1     , 1       },
                                                                         {2   , 1     , 1       },
                                                                         {3   , 1     , 1       },
                                                                         {4   , 1     , 1       },
                                                                         {5   , 1     , 0       }, }), TpccError);
}

TEST_F(TpccStoredNewOrderTest, NewOrder_twiceTheSameItem) {
  //                     (w_id, d_id, c_id, o_carrier_id, ol_dist_info, {{i_id, i_w_id, quantity}});
  EXPECT_THROW(doNewOrder(1   , 1   , 1   , 1           , "info"      , {{1   , 1     , 1       },
                                                                         {2   , 1     , 1       },
                                                                         {3   , 1     , 1       },
                                                                         {4   , 1     , 1       },
                                                                         {4   , 1     , 1       }}), TpccError);
}

} } // namespace hyrise::access

