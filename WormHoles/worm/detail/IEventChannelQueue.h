#ifndef __WH_IEVENT_CHANNEL_QUEUE_H__
#define __WH_IEVENT_CHANNEL_QUEUE_H__

namespace worm::detail {
class IEventChannelQueue {
public:
    virtual void DispatchAll() = 0;

public:
    virtual ~IEventChannelQueue() = default;
};
} // namespace worm::detail

#endif // !__IEVENT_CHANNEL_QUEUE_H__