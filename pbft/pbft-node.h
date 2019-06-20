#ifndef PBFT_NODE_H
#define PBFT_NODE_H

#include <algorithm>
#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/traced-callback.h"
#include "ns3/address.h"
#include "ns3/boolean.h"
#include <map>

namespace ns3 {

class Address;
class Socket;
class Packet;

class PbftNode : public Application 
{
  public:
    static TypeId GetTypeId (void);

    void SetPeersAddresses (const std::vector<Ipv4Address> &peers);         // 设置所有邻节点的地址

    PbftNode (void);

    virtual ~PbftNode (void);

    uint32_t        m_id;                               // node id
    Ptr<Socket>     m_socket;                           // 监听的socket
    Ptr<Socket>     m_socketClient;                     // 客户端socket
    std::map<Ipv4Address, Ptr<Socket>>      m_peersSockets;            // 邻节点的socket列表
    std::map<Address, std::string>          m_bufferedData;            // map holding the buffered data from previous handleRead events
    
    Address         m_local;                            // 本节点地址
    std::vector<Ipv4Address>  m_peersAddresses;         // 邻节点列表

    int             N;                                  // 总节点数
    // int             v;                                  // 当前视图编号
    // int             n;                                  // 当前消息在视图中的编号
    std::vector<int>  values;                           // 存储要更新的数值的容器
    int             value;                              // 交易要更新的值
    int             leader;                             // 当前leader节点的编号
    int             is_leader;                          // 自己是否是leader

    // int             round;                              // 共识轮数
    int             block_num;                             // 当前区块号
    EventId         blockEvent;                         // 广播区块的事件
    struct TX {
        int v;
        int val;
        int prepare_vote;
        int commit_vote;
    };
    TX tx[1000];

    // 继承 Application 类必须实现的虚函数
    virtual void StartApplication (void);    
    virtual void StopApplication (void); 

    // 处理消息
    void HandleRead (Ptr<Socket> socket);

    // 将数据包中的消息解析为字符串
    std::string getPacketContent(Ptr<Packet> packet, Address from); 

    // 向所有邻节点广播消息 
    void Send (uint8_t data[]);

    // 向某个指定地址的节点发送消息
    void Send(uint8_t data[], Address from);

    // 向所有邻节点广播区块
    void SendBlock(void);

    void viewChange(void);
};

enum Message
{
    REQUEST,           // 0       客户端请求        <REQUEST, t>    t:交易
    PRE_PREPARE,       // 1       预准备消息        <PRE_PREPARE, v, n, b>   v:视图编号   b:区块内容   n:该预准备消息在视图中的编号
    PREPARE,           // 2       准备消息          <PREPARE, v, n, H(b)>
    COMMIT,            // 3       提交             <COMMIT, v, n>
    PRE_PREPARE_RES,   // 4       预准备消息的响应   <PRE_PREPARE_RES, v, n, S>           S:State
    PREPARE_RES,       // 5       准备消息响应
    COMMIT_RES,        // 6       提交响应
    REPLY,             // 7       对客户端的回复       
    VIEW_CHANGE        // 8       view_change消息
};

enum State
{
    SUCCESS,            // 0      成功
    FAILED,             // 1      失败
};
}
#endif
