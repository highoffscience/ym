/**
 * @author Forrest Jablonski
 */

#include <cstdio>
#include <new>

int main(void)
{
   for (int i = 1; i < 10; ++i)
   {
      auto * const data_Ptr = ::operator new(i * 10000, std::align_val_t{32});
      std::printf("%p -- %lu\n", data_Ptr, reinterpret_cast<unsigned long>(data_Ptr) % 32ul);
   }

   return 0;
}
