#include <clang-c/Index.h>

#include <iostream>

CXChildVisitResult visitClass(CXCursor cursor,
                              CXCursor /*parent*/,
                              CXClientData)
{
   const auto kind = clang_getCursorKind(cursor);

   if (kind != CXCursor_CXXMethod)
      return CXChildVisit_Recurse;

   CXString name = clang_getCursorSpelling(cursor);

   std::cout << "  " << clang_getCString(name) << '\n';

   clang_disposeString(name);

   return CXChildVisit_Continue;
}

CXChildVisitResult visit(CXCursor cursor,
                         CXCursor /*parent*/,
                         CXClientData)
{
   const auto kind = clang_getCursorKind(cursor);

   if (kind == CXCursor_ClassDecl ||
      kind == CXCursor_StructDecl)
   {
      CXString name = clang_getCursorSpelling(cursor);

      std::cout << clang_getCString(name) << ":\n";

      clang_disposeString(name);

      // Visit the members of this class.
      clang_visitChildren(cursor, visitClass, nullptr);

      // Don't recurse here, since visitClass already handled
      // this class's children.
      return CXChildVisit_Continue;
   }

   return CXChildVisit_Recurse;
}

int main(int argc, char** argv)
{
   if (argc != 2)
   {
      std::cerr << "Usage: " << argv[0] << " <header>\n";
      return 1;
   }

   CXIndex index = clang_createIndex(
      /* excludeDeclarationsFromPCH = */ 0,
      /* displayDiagnostics = */ 0);

   CXTranslationUnit translationUnit =
      clang_parseTranslationUnit(
         index,
         argv[1],
         nullptr,
         0,
         nullptr,
         0,
         CXTranslationUnit_None);

   if (translationUnit == nullptr)
   {
      std::cerr << "Failed to parse: " << argv[1] << '\n';

      clang_disposeIndex(index);
      return 1;
   }

   CXCursor root =
      clang_getTranslationUnitCursor(translationUnit);

   clang_visitChildren(root, visit, nullptr);

   clang_disposeTranslationUnit(translationUnit);
   clang_disposeIndex(index);

   return 0;
}
