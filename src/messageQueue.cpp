#include "../include/messageQueue.hpp"  

void MessageQueue::sendMessage(InterfaceMessage msg) {
    mutex.lock();
    messages.push(msg);
    mutex.unlock();
}

InterfaceMessage MessageQueue::recvMessage() {
    bool res = mutex.try_lock();
    if (!res) return {};
    if (messages.empty()) {
        mutex.unlock();
        return {};
    }

    InterfaceMessage msg = messages.front();
    messages.pop();

    mutex.unlock();
    
    return msg;
}