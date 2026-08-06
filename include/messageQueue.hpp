#pragma once
#include <queue>
#include <string>
#include <mutex>

typedef struct {
    public:
        enum class InterfaceMessageType {
            NONE = 0, ADD_MODEL_TO_SCENE, RENDER_MODEL
        };

        InterfaceMessageType type;
        std::string modelName;
        unsigned int sceneIndex;
} InterfaceMessage;

class MessageQueue {
    public:
        void sendMessage(InterfaceMessage msg);
        InterfaceMessage recvMessage();

    private:
        std::queue<InterfaceMessage> messages;
        std::mutex mutex;
};