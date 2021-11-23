
#ifdef EAMS
  #include "EAMS/simple_types.h"

  #include "WriteBufferInterface.h"
  #include <string>

  class WriteBuffer : public EmbeddedProto::WriteBufferInterface
  {
    static constexpr uint8_t N = 10;
    public:
      WriteBuffer() : data_{0}, count_(0) {};
      ~WriteBuffer() override = default;
      void clear() override { count_ = 0; }
      uint32_t get_size() const override { return count_; }
      uint32_t get_max_size() const override { return N; }
      uint32_t get_available_size() const override { return N - count_; }
      
      bool push(const uint8_t byte) override 
      { 
        const bool result = N > count_;
        if(result)
        {
          data_[count_] = byte;
          ++count_;
        }
        return result;
      };

      bool push(const uint8_t* bytes, const uint32_t length) override
      {
        const bool result = N >= (count_ + length);
        if(result) 
        {
          memcpy(&(data_[count_]), bytes, length);
          count_ += length;
        }
        return result;
      }

    private:
      uint8_t data_[N];
      uint8_t count_;
  };

#elif GOOGLE
  #include "Google/simple_types.pb.h"
  #include <string>
#elif NANOPB
  #include "Nanopb/simple_types.pb.h"
  #include "pb_encode.h"
#endif

int main(int argc, char **argv)
{
  Test_Simple_Types msg;
#ifdef EAMS
  msg.set_a_int32(1);
  WriteBuffer buffer;
  msg.serialize(buffer);
#elif GOOGLE
  msg.set_a_int32(1);
  std::string str;
  msg.SerializeToString(&str);
#elif NANOPB
  msg.a_int32 = 1;
  uint8_t buffer[10];
  pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
  pb_encode(&ostream, Test_Simple_Types_fields, &msg);
#endif
}