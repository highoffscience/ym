/**
 * @author Forrest Jablonski
 */

#include <cstdio>
#include <memory>
#include <new>

struct S
{
   double d1;
   double d2;
};

int main(void)
{
   std::allocator<S> a;

   std::printf("%lu\n", sizeof(std::allocator<S>));

   return 0;
}
