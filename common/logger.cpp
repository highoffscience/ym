/**
 * @author Forrest Jablonski
 */

#include "logger.h"

/**
 *
 */
ym::Logger::Logger(void)
   : _outfile_uptr {nullptr, [](std::FILE * const Ptr) { std::fclose(Ptr); }}
{
}

/*
 TODO! Why doesn't this compile?
*/
#include <memory>
std::unique_ptr<int> get(int const NObjects)
{
   std::allocator<int> a;
   auto * const data_Ptr = (NObjects > 0ul) ? a.allocate(NObjects) : nullptr;
   auto deleterr = [&](int * p) { a.deallocate(data_Ptr, NObjects); };
   return std::unique_ptr<int, decltype(deleterr)>{data_Ptr, deleterr};
}

void test(void)
{
   auto u = get(10);
}

/**
 *
 */
bool ym::Logger::openOutfile(str const Filename)
{
   if (!_outfile_uptr)
   {
      _outfile_uptr.reset(std::fopen(Filename, "w"));
   }

   return static_cast<bool>(_outfile_uptr);
}
