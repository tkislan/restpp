#ifndef HTTP_STREAM_STATE_H
#define HTTP_STREAM_STATE_H

namespace restpp {
enum class HttpStreamState {
  IDLE,
  RESOLVING,
  CONNECTING,
  SENDING,
  READING,
  FINISHED
};
}

#endif // HTTP_STREAM_STATE_H

