/**
 * @author Forrest Jablonski
 */

#include "DataLogger.h"

#include "TextLogger.h"

#include <cstdio>

/**
 *
 */
ym::DataLogger::DataLogger(void)
   : Logger         {},
     _metaData      {},
     _metaDataGuard {}
{
   _metaData.reserve(10);
}

/**
 *
 */
bool ym::DataLogger::dump(SStr_T const Filename) const
{
   auto wasDumpSuccessful = false;
   
   auto const [outfile_Ptr, ErrorCode] = Logger::open(Filename);
   
   if (ErrorCode != 0)
   { // failure to open
      ymLog(0, "Failed to open %s! with error code %d!\n", Filename, ErrorCode);
   }
   else
   { // file opened successfully
      std::fprintf(outfile_Ptr, "Timestamp (us)");
   
      for (auto const & MetaData : _metaData)
      {
         std::fprintf(outfile_Ptr, ",%s", MetaData.getName());
      }
   
      std::fprintf(outfile_Ptr, "\n");
   
      for (uint32 i = 0; i < _metaData.size(); ++i)
      {
         auto const & MetaData = _metaData[i];
   
         auto const Start_idx = MetaData.didWrap() ? MetaData.getNextIndex()
                                                   : 0;
         auto const Size      = MetaData.didWrap() ? MetaData.getCapacity()
                                                   : MetaData.getNextIndex();

         for (uint32 j = 0; j < Size; j = ++j)
         {
            auto const Index = (Start_idx + j) % MetaData.getCapacity();

            std::fprintf(outfile_Ptr, "%lld", MetaData.getTimeStampsPtr()[Index]);
            printNCommas(i, outfile_Ptr);
            printDatum(MetaData, Index, outfile_Ptr);
            printNCommas(_metaData.size() - i - 1u, outfile_Ptr);
            std::fprintf(outfile_Ptr, "\n");
         }
      }
   
      wasDumpSuccessful = true;
   }
   
   return wasDumpSuccessful;
}

/**
 *
 */
void ym::DataLogger::printDatum(VectorMetaData const & MetaData,
                                uint32         const   Index,
                                FILE *         const   outfile_Ptr) const
{
   typedef VectorMetaData::Typing_T::T T;

   switch (MetaData.getTyping())
   {
      case T::Bool:
      {
         auto const Major_idx = Index >> 3u;
         auto const Minor_idx = Index & 0x0000'0007u; // [0, 7]

         auto const Datum = (*MetaData.getDatumPtr<uint8>(Major_idx) & (1u << Minor_idx)) != 0;

         std::fprintf(outfile_Ptr, ",%lu", Datum);
         break;
      }

      case T::Char:    { std::fprintf(outfile_Ptr, ",%c",   *MetaData.getDatumPtr<char   >(Index)); break; }
      case T::Int8:    { std::fprintf(outfile_Ptr, ",%ld",  *MetaData.getDatumPtr<int8   >(Index)); break; }
      case T::Int16:   { std::fprintf(outfile_Ptr, ",%ld",  *MetaData.getDatumPtr<int16  >(Index)); break; }
      case T::Int32:   { std::fprintf(outfile_Ptr, ",%ld",  *MetaData.getDatumPtr<int32  >(Index)); break; }
      case T::Int64:   { std::fprintf(outfile_Ptr, ",%lld", *MetaData.getDatumPtr<int64  >(Index)); break; }
      case T::UInt8:   { std::fprintf(outfile_Ptr, ",%lu",  *MetaData.getDatumPtr<uint8  >(Index)); break; }
      case T::UInt16:  { std::fprintf(outfile_Ptr, ",%lu",  *MetaData.getDatumPtr<uint16 >(Index)); break; }
      case T::UInt32:  { std::fprintf(outfile_Ptr, ",%lu",  *MetaData.getDatumPtr<uint32 >(Index)); break; }
      case T::UInt64:  { std::fprintf(outfile_Ptr, ",%llu", *MetaData.getDatumPtr<uint64 >(Index)); break; }
      case T::Float32: { std::fprintf(outfile_Ptr, ",%lf",  *MetaData.getDatumPtr<float32>(Index)); break; }
      case T::Float64: { std::fprintf(outfile_Ptr, ",%lf",  *MetaData.getDatumPtr<float64>(Index)); break; }
   }
}

/**
 *
 */
void ym::DataLogger::printNCommas(uint32 const NCommas,
                                  FILE * const outfile_Ptr) const
{
   for (uint32 i = 0; i < NCommas; ++i)
   {
      std::fprintf(outfile_Ptr, ",");
   }
}

/**
 *
 */
template <>
ym::DataLogger::VectorMetaData::VectorMetaData(SStr_T const Name,
                                               uint32 const Capacity,
                                               bool   const)
   : CNameable_NV    {Name                                       },
     _timeStamps_Ptr {ymAllocate<TimeRep_T>(Capacity)            },
     _data_Ptr       {ymAllocate<uint8>    (Capacity)            },
     _Typing         {VectorMetaData::Typing_T::getTyping<bool>()},
     _Capacity       {Capacity                                   },
     _next_idx       {0                                          },
     _didWrap        {false                                      }
{
}

/**
 *
 */
ym::DataLogger::VectorMetaData::~VectorMetaData(void)
{
   ymDeallocate(_timeStamps_Ptr);
   ymDeallocate(_data_Ptr      );
}

/**
 *
 */
#pragma warning(disable: 4715) // all control paths return a value
auto ym::DataLogger::VectorMetaData::Typing_T::asString(T const Typing) -> SStr_T
{
   switch (Typing)
   {
      case T::Bool:    { return "bool";    }
      case T::Char:    { return "char";    }
      case T::Int8:    { return "int8";    }
      case T::Int16:   { return "int16";   }
      case T::Int32:   { return "int32";   }
      case T::Int64:   { return "int64";   }
      case T::UInt8:   { return "uint8";   }
      case T::UInt16:  { return "uint16";  }
      case T::UInt32:  { return "uint32";  }
      case T::UInt64:  { return "uint64";  }
      case T::Float32: { return "float32"; }
      case T::Float64: { return "float64"; }
   }
}
