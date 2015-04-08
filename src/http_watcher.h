#ifndef HTTP_WATCHER_H
#define HTTP_WATCHER_H

#include <functional>

#include "http_stream_state.h"

namespace restpp {
class HttpWatcher {
public:
  HttpWatcher(std::function<HttpStreamState ()> &&state, std::function<bool ()> &&cancel)
    : state_(state),
      cancel_(cancel)
  {}

  HttpWatcher(HttpWatcher &&other) = default;
  HttpWatcher(const HttpWatcher &other) = delete;

  HttpWatcher &operator=(HttpWatcher &&other) = default;
  HttpWatcher &operator=(const HttpWatcher &other) = delete;

  inline HttpStreamState state() const { return state_(); }
  inline bool running() const {
    auto state = state_();
    return state > HttpStreamState::IDLE && state < HttpStreamState::FINISHED;
  }
  inline bool cancel() const { return cancel_(); }

private:
  std::function<HttpStreamState ()> state_;
  std::function<bool ()> cancel_;
};
}

#endif // HTTP_WATCHER_H

