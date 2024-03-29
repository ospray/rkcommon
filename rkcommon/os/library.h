// Copyright 2009 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "../common.h"
// std
#include <memory>
#include <string>
#include <vector>

namespace rkcommon {

  class RKCOMMON_INTERFACE Library
  {
   public:
    using Version = std::vector<int>;

    // Opens a shared library; anchorAddress = nullptr will disable anchored loads
    Library(const void *anchorAddress,
      const std::string &name, const Version &version);
    ~Library();

    // Returns address of a symbol from the library
    void *getSymbol(const std::string &sym) const;

   private:
    Library(void *const lib);

    bool loadLibrary(const void *anchorAddress);

    std::string libraryName;
    Version libraryVersion;
    std::string errorMessage;
    void *lib{nullptr};
    bool freeLibOnDelete{true};

    friend class LibraryRepository;

    template <typename T, typename... Args>
    friend inline std::unique_ptr<T> make_unique(Args &&... args);
  };

  class RKCOMMON_INTERFACE LibraryRepository
  {
   public:
    static LibraryRepository *getInstance();
    static void cleanupInstance();

    ~LibraryRepository();

    LibraryRepository(const LibraryRepository &) = delete;
    LibraryRepository &operator=(const LibraryRepository &) = delete;

    // add/remove a library to/from the repo
    void add(const void *anchorAddress,
      const std::string &name, const Library::Version &version = {});
    void remove(const std::string &name);

    // Returns address of a symbol from any library in the repo
    void *getSymbol(const std::string &sym) const;

    bool libraryExists(const std::string &name) const;

   private:
    using const_library_iterator_t =
        std::vector<std::unique_ptr<Library>>::const_iterator;
    using library_iterator_t = std::vector<std::unique_ptr<Library>>::iterator;

    const_library_iterator_t findLibrary(const std::string &name) const;
    library_iterator_t findLibrary(const std::string &name);

    static std::unique_ptr<LibraryRepository> instance;
    LibraryRepository() = default;

    std::vector<std::unique_ptr<Library>> repo;
  };
}  // namespace rkcommon
