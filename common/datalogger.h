/**
 * @author Forrest Jablonski
 */

#pragma once

#include "Standard.h"

#include "Logger.h"
#include "MemoryDefs.h"
#include "TimerDefs.h"
#include "Ymception.h"

#include <mutex>
#include <vector>

namespace ym
{

/**
 * 
 */
class DataLogger : public Logger
{
public:
   explicit DataLogger(void);
   virtual ~DataLogger(void) = default;

   bool dump(SStr_T const Filename) const;

   typedef uint32 Key_T;

   template <typename Data_T>
   Key_T add(SStr_T const VectorName,
             uint32 const VectorCapacity);

   template <typename Datum_T>
   inline void store(Key_T   const Key,
                     Datum_T const Datum);

   template <typename Datum_T>
   void store(Key_T     const Key,
              Datum_T   const Datum,
              TimeRep_T const TimeStamp);

private:
   /**
    *
    */
   class VectorMetaData : public CNameable_NV
   {
   public:
      struct Typing_T
      {
         enum class T : uint32
         {
            Bool,
            Char,
            Int8,
            Int16,
            Int32,
            Int64,
            UInt8,
            UInt16,
            UInt32,
            UInt64,
            Float32,
            Float64
         };

         static SStr_T asString(T const Typing);

         template <typename Datum_T>
         static constexpr auto getTyping(void) { static_assert(false, "Type not supported!"); }
   
         template <> static constexpr auto getTyping<bool   >(void) { return T::Bool;    }
         template <> static constexpr auto getTyping<char   >(void) { return T::Char;    }
         template <> static constexpr auto getTyping<int8   >(void) { return T::Int8;    }
         template <> static constexpr auto getTyping<int16  >(void) { return T::Int16;   }
         template <> static constexpr auto getTyping<int32  >(void) { return T::Int32;   }
         template <> static constexpr auto getTyping<int64  >(void) { return T::Int64;   }
         template <> static constexpr auto getTyping<uint8  >(void) { return T::UInt8;   }
         template <> static constexpr auto getTyping<uint16 >(void) { return T::UInt16;  }
         template <> static constexpr auto getTyping<uint32 >(void) { return T::UInt32;  }
         template <> static constexpr auto getTyping<uint64 >(void) { return T::UInt64;  }
         template <> static constexpr auto getTyping<float32>(void) { return T::Float32; }
         template <> static constexpr auto getTyping<float64>(void) { return T::Float64; }
      };

      template <typename Data_T>
      explicit VectorMetaData(SStr_T const Name,
                              uint32 const Capacity,
                              Data_T const); // dummy to get compiler to call correct specialization

      template <>
      explicit VectorMetaData(SStr_T const Name,
                              uint32 const Capacity,
                              bool   const); // dummy to get compiler to call correct specialization

      ~VectorMetaData(void);

      inline auto getTimeStampsPtr(void) const { return _timeStamps_Ptr; }

      // not reinterpret_cast
      template <typename Datum_T>
      inline auto getDataPtr      (void) const { return static_cast<Datum_T *>(_data_Ptr); }

      inline auto getTyping       (void) const { return _Typing;   }
      inline auto getCapacity     (void) const { return _Capacity; }
      inline auto getNextIndex    (void) const { return _next_idx; }
      inline auto didWrap         (void) const { return _didWrap;  }

      inline auto getNextTimeStampPtr(void) const { return getTimeStampsPtr() + getNextIndex(); }

      template <typename Datum_T>
      inline auto getDatumPtr(uint32 const Index) const { return getDataPtr<Datum_T>() + Index; }

      template <typename Datum_T>
      inline auto getNextDatumPtr(void) const { return getDatumPtr<Datum_T>(getNextIndex()); }

      inline void advanceNextIndex(void) { _next_idx = (_next_idx + 1) % getCapacity(); 
                                           _didWrap |= (getNextIndex() == 0); }

   private:
      TimeRep_T * const _timeStamps_Ptr;
      void      * const _data_Ptr;
      Typing_T::T const _Typing;
      uint32      const _Capacity;
      uint32            _next_idx;
      bool              _didWrap;
   };

   std::vector<VectorMetaData> _metaData;
   std::mutex                  _metaDataGuard;

   template <typename Datum_T>
   inline void store_helper(Key_T   const Key,
                            Datum_T const Datum);

   template <>
   inline void store_helper(Key_T const Key,
                            bool  const Datum);

   void printDatum(VectorMetaData const & MetaData,
                   uint32         const   Index,
                   FILE *         const   outfile_Ptr) const;
   
   void printNCommas(uint32 const NCommas,
                     FILE * const outfile_Ptr) const;
};

/**
 * Returns the key.
 */
template <typename Data_T>
auto DataLogger::add(SStr_T const VectorName,
                     uint32 const VectorCapacity) -> Key_T
{
   ymAssert(VectorCapacity > 0, "Vector capacity must be greater than 0!");

   // this will fail if Data_T is not a supported type
   // dummy is used to call the correct specialization
   _metaData.emplace_back(VectorName, VectorCapacity, Data_T() /* dummy */);

   return _metaData.size() - 1;
}

/**
 *
 */
template <typename Datum_T>
inline void DataLogger::store(Key_T   const Key,
                              Datum_T const Datum)
{
   store(Key, Datum, ymGetGlobalTime<std::micro>());
}

/**
 * TODO Events should have a timestamp of origin with them
 */
template <typename Datum_T>
void DataLogger::store(Key_T     const Key,
                       Datum_T   const Datum,
                       TimeRep_T const TimeStamp)
{
   ymAssertDbg(_metaData[Key].getTyping() == VectorMetaData::Typing_T::getTyping<Datum_T>(),
      "Trying to store %s in a slot meant for %s!",
      VectorMetaData::Typing_T::asString(VectorMetaData::Typing_T::getTyping<Datum_T>()),
      VectorMetaData::Typing_T::asString(_metaData[Key].getTyping()));
   
   *_metaData[Key].getNextTimeStampPtr() = TimeStamp;

   store_helper(Key, Datum); // populates data_Ptr

   _metaData[Key].advanceNextIndex();
}

/**
 *
 */
template <typename Datum_T>
inline void DataLogger::store_helper(Key_T   const Key,
                                     Datum_T const Datum)
{
   // don't need memcpy here. see <https://en.cppreference.com/w/cpp/language/reinterpret_cast>
   *_metaData[Key].getNextDatumPtr<Datum_T>() = Datum;
}

/**
 *
 */
template <>
inline void DataLogger::store_helper(Key_T const Key,
                                     bool  const Datum)
{
   auto const Major_idx = _metaData[Key].getNextIndex() >> 3u;
   auto const Minor_idx = _metaData[Key].getNextIndex() & 0x0000'0007u; // [0, 7]

   auto & e_ref = *_metaData[Key].getDatumPtr<uint8>(Major_idx);

   e_ref = e_ref & ~(1u    << Minor_idx) | // clear bit
                     Datum << Minor_idx;   // set bit to Datum
}

/**
 *
 */
template <typename Data_T>
DataLogger::VectorMetaData::VectorMetaData(SStr_T const Name,
                                           uint32 const Capacity,
                                           Data_T const)
   : CNameable_NV    {Name                                         },
     _timeStamps_Ptr {ymAllocate<TimeRep_T>(Capacity)              },
     _data_Ptr       {ymAllocate<Data_T>   (Capacity)              },
     _Typing         {VectorMetaData::Typing_T::getTyping<Data_T>()},
     _Capacity       {Capacity                                     },
     _next_idx       {0                                            },
     _didWrap        {false                                        }
{
}

} // ym
