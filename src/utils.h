#ifndef UTILS
#define UTILS

#include <string>
#include <algorithm>
#include <functional>
#include <istream>

#include "asio/streambuf.hpp"
#include "asio/buffers_iterator.hpp"

#include "constants.h"

namespace restpp {
/************************************************************************/
/* Comparator for case-insensitive comparison in STL assos. containers  */
/************************************************************************/
struct ci_less : std::binary_function<std::string, std::string, bool> {
  // case-independent (ci) compare_less binary function
  struct nocase_compare : public std::binary_function<unsigned char,unsigned char,bool> {
    bool operator() (const unsigned char& c1, const unsigned char& c2) const {
        return tolower (c1) < tolower (c2);
    }
  };
  bool operator() (const std::string & s1, const std::string & s2) const {
    return std::lexicographical_compare
      (s1.begin(), s1.end(),   // source range
      s2.begin(), s2.end(),   // dest range
      nocase_compare());  // comparison
  }
};

std::string StreambufToString(const asio::streambuf &buffer) {
  asio::streambuf::const_buffers_type bufs = buffer.data();
  return std::move(std::string(asio::buffers_begin(bufs), asio::buffers_begin(bufs) + buffer.size()));
}

void GetStatusLine(std::istream &is, std::string &http_version, unsigned int &status_code, std::string &status_message) {
  is >> http_version;
  if (!is.good()) return;

  is >> status_code;
  if (!is.good()) return;

  status_message.clear();

  std::istream::sentry s(is, true);
  std::streambuf *sb = is.rdbuf();

  for (;;) {
    auto c = sb->sbumpc();

    switch (c) {
      case '\n':
        return;
      case '\r':
        if (sb->sgetc() == '\n') {
          sb->sbumpc();
          return;
        }
        break;
      case EOF:
        is.setstate(std::ios::eofbit);
        return;
      default:
        status_message.push_back(c);
        break;
    }
  }

  is.setstate(std::ios::failbit);
}

void GetHeader(std::istream &is, std::string &header_name, std::string &header_value) {
  header_name.clear();
  header_value.clear();

  std::istream::sentry s(is, true);
  std::streambuf *sb = is.rdbuf();

  bool parsing_name = true;
  bool header_value_padding = true;

  for (;;) {
    auto c = sb->sbumpc();

    switch (c) {
      case '\n':
        return;
      case '\r':
        if (sb->sgetc() == '\n') {
          sb->sbumpc();
          return;
        }
        break;
      case ' ':
        if (!parsing_name && !header_value_padding) header_value.push_back(c);
        break;
      case EOF:
        is.setstate(std::ios::eofbit);
        return;
      case ':':
        if (parsing_name) {
          parsing_name = false;
          break;
        }
      default:
        if (parsing_name) {
          header_name.push_back(c);
        } else {
          if (header_value_padding) header_value_padding = false;
          header_value.push_back(c);
        }
    }
  }

  is.setstate(std::ios::failbit);
}

void GetChunkSize(std::istream &is, size_t &chunk_size) {
  is >> std::hex >> chunk_size;
  if (!is.good()) return;

  std::istream::sentry s(is, true);
  std::streambuf *sb = is.rdbuf();

  auto r = sb->sbumpc();
  auto n = sb->sbumpc();

  if (r == EOF || n == EOF) {
    is.setstate(std::ios::eofbit);
    return;
  } else if (r != '\r' || n != '\n') {
    is.setstate(std::ios::failbit);
    return;
  }
}

class ChunkInfo {
public:
  ChunkInfo(size_t chunk_size) : chunk_size_(chunk_size), chunk_size_read_(0), /*crlf_read_(false),*/ has_chunk_size_(true) {}

  inline size_t ChunkBytesLeft() const { assert(chunk_size_read_ <= chunk_size_); return chunk_size_ - chunk_size_read_; }
  inline bool ChunkDataRead() const { return ChunkBytesLeft() == 0; }

  inline void ChunkBytesRead(size_t bytes_read) {
    assert(bytes_read <= ChunkBytesLeft());
    chunk_size_read_ += bytes_read;

    if (ChunkDataRead()) Clear();
  }

//  inline void CrlfBytesRead() {
//    assert(ChunkBytesLeft() == 0);
//    assert(crlf_read_);
//    crlf_read_ = true;

//    Clear();
//  }

  inline size_t chunk_size() const { return chunk_size_; }
  inline size_t chunk_size_read() const { return chunk_size_read_; }
//  inline bool crlf_read() const { return crlf_read_; }
  inline bool has_chunk_size() const { return has_chunk_size_; }
  inline operator bool() const { return has_chunk_size_; }

private:
  inline void Clear() { chunk_size_ = 0; chunk_size_read_ = 0; /*crlf_read_ = false; */has_chunk_size_ = false; }

  size_t chunk_size_;
  size_t chunk_size_read_;
//  bool crlf_read_;
  bool has_chunk_size_;
};

//std::pair<asio::buffers_iterator<asio::streambuf::const_buffers_type>, bool> ReadChunk(asio::buffers_iterator<asio::streambuf::const_buffers_type> begin,
//                                                                                       asio::buffers_iterator<asio::streambuf::const_buffers_type> end,
//                                                                                       const std::shared_ptr<ChunkInfo> &chunk_info)
//{
//  return std::make_pair(end, true);
//}
}
#endif // UTILS

