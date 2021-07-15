#pragma once

namespace barTap{
  class encryption32{
    private:
      uint_fast32_t key;

    public:
      encryption32(uint_fast32_t key) : key(key) {}

      uint_fast32_t encrypt(const uint_fast32_t& id){
          return id^key;
      }

      uint_fast32_t decrypt(const uint_fast32_t& id){
          return key^id;
      }
  };
}