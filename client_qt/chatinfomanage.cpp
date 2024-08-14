#include "chatinfomanage.h"

ChatInfoManage* ChatInfoManage::m_pInstance = m_pInstance;

ChatInfoManage::ChatInfoManage()
{

}

//获取在线用户信息
GroupUserInfo *ChatInfoManage::getUserInfo(int account)
{
    GroupUserInfo* userInfo =NULL;
    mapGroupInfo::iterator iter = g_GroupUserInfoMap.find(0);
    if(iter!=g_GroupUserInfoMap.end()){//遍历所有群组
        mapGroupUserInfo::iterator ite = iter->second.find(account);//查找群组中的用户信息
        ChatLogInfo()<<u8"群成员个数:"<<iter->second.size();
        if(ite!=iter->second.end()){//用户在iter指向的当前群组中
            userInfo = ite->second;//取出用户信息
            ChatLogInfo()<<u8"账号:" << userInfo->m_account <<u8",用户名:" << QString(userInfo->m_userName);
            return userInfo;//返回用户
        }
        else{
            ChatLogInfo()<<u8"搜索用户失败..";
        }
    }
    else {
        ChatLogInfo()<<u8"不存在群组 0..";
    }
    return userInfo;
}

//创建一个群，加入映射表中
int ChatInfoManage::addGroupChatInfo(GroupChatInfo *pGroupChatInfo)
{
    std::pair<mapGroupChatInfo::iterator, bool> InsertPair = g_GroupCharInfoMap.insert(std::make_pair(pGroupChatInfo->m_account,pGroupChatInfo));
    if(InsertPair.second == true)
        return 0;
    return -1;
}

//通过账号获取一个群信息
GroupChatInfo *ChatInfoManage::getGroupChatInfo(int groupAccount)
{
    GroupChatInfo* groupChatInfo = NULL;
    mapGroupChatInfo::iterator iter = g_GroupCharInfoMap.find(groupAccount);
    if(iter!=g_GroupCharInfoMap.end()){
        groupChatInfo = iter->second;//返回群消息
    }
    else {
        ChatLogInfo()<<u8"查找群号不存在";
        return NULL;
    }
    return groupChatInfo;
}
//将用户groupUserInfo 加入到群groupAccount中
int ChatInfoManage::addGroupUserInfo(int groupAccount, GroupUserInfo *groupUserInfo)
{
    //将群用户信息添加到群里面
    mapGroupInfo::iterator iter = g_GroupUserInfoMap.find(groupAccount);//<账号，用户群角色>
    if(iter != g_GroupUserInfoMap.end()){
        //用户信息加入到群中
        std::pair<mapGroupUserInfo::iterator, bool> InsertPair = iter->second.insert(std::make_pair(groupUserInfo->m_account,groupUserInfo));
        if(InsertPair.second == true)
            return 0;
        else {
            ChatLogInfo()<<"insert groupAccount:"<<groupAccount<<" error";
        }
    }
    else {//群角色不存在
        mapGroupUserInfo groupUserInfoMap;
        //群 map 表中插入用户数据
        //<账号，群角色>
        std::pair<mapGroupUserInfo::iterator, bool> insertPair = groupUserInfoMap.insert(std::make_pair(groupUserInfo->m_account,groupUserInfo));
        if(insertPair.second != true)
            return -1;
        //插入到群中
        std::pair<mapGroupInfo::iterator, bool> InsertPair = g_GroupUserInfoMap.insert(std::make_pair(groupAccount,groupUserInfoMap));
        if(InsertPair.second == true)
            return 0;
    }
    return -1;
}
//获取群用户信息
GroupUserInfo *ChatInfoManage::getGroupUserInfo(int groupAccount, int userAccount)
{
    GroupUserInfo* groupUserInfo =NULL;
    mapGroupInfo::iterator iter = g_GroupUserInfoMap.find(groupAccount);
    if(iter!=g_GroupUserInfoMap.end()){
        mapGroupUserInfo::iterator ite = iter->second.find(userAccount);
        if(ite!=iter->second.end()){
            groupUserInfo = ite->second;
        }
    }
    return groupUserInfo;
}
//添加好友,加入好友映射表
int ChatInfoManage::addFriendInfo(FriendInfo *friendInfo)
{
    std::pair<mapFriendInfo::iterator, bool> InsertPair = m_FriendInfoMap.insert(std::make_pair(friendInfo->m_account,friendInfo));
    if(InsertPair.second == true)
        return 0;
    return -1;
}
//获取好友信息
FriendInfo *ChatInfoManage::getFriendInfo(int account)
{
    FriendInfo* friendInfo = NULL;
    mapFriendInfo::iterator iter = m_FriendInfoMap.find(account);
    if(iter!=m_FriendInfoMap.end()){
        friendInfo = iter->second;
    }
    else {
        ChatLogInfo()<<"not find chatgroup indo";
        return NULL;
    }
    return friendInfo;
}
