#ifndef __IEVENT_CHANNEL_QUEUE_H__
#define __IEVENT_CHANNEL_QUEUE_H__

namespace worm::internal {
class IEventChannelQueue {
public:
    virtual void DispatchAll() = 0;

public:
    virtual ~IEventChannelQueue() = default;
};
} // namespace worm::internal

#endif // !__IEVENT_CHANNEL_QUEUE_H__