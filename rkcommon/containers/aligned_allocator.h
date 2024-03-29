// Copyright 2009 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <stddef.h>   // Required for size_t and ptrdiff_t and nullptr
#include <new>        // Required for placement new and std::bad_alloc
#include <stdexcept>  // Required for std::length_error

#include "../memory/malloc.h"

namespace rkcommon {
  namespace containers {

    // NOTE(jda) - aligned_allocator implementation loosely based off of Stephen
    //             T. Lavavej's "Mallocator" example:
    //
    // https://blogs.msdn.microsoft.com/vcblog/2008/08/28/the-mallocator/

#define OSPRAY_DEFAULT_ALIGNMENT 64

    template <typename T, int alignment = OSPRAY_DEFAULT_ALIGNMENT>
    struct aligned_allocator
    {
      // Compile-time info //

      using pointer         = T *;
      using const_pointer   = const T *;
      using reference       = T &;
      using const_reference = const T &;
      using value_type      = T;
      using size_type       = size_t;
      using difference_type = ptrdiff_t;

      template <typename U>
      struct rebind
      {
        using other = aligned_allocator<U>;
      };

      // Implementation //

      aligned_allocator()                          = default;
      aligned_allocator(const aligned_allocator &) = default;
      ~aligned_allocator()                         = default;
      aligned_allocator &operator=(const aligned_allocator &) = delete;

      template <typename U, int OA = OSPRAY_DEFAULT_ALIGNMENT>
      aligned_allocator(const aligned_allocator<U, OA> &);

      template <typename U, int OA = OSPRAY_DEFAULT_ALIGNMENT>
      aligned_allocator &operator=(const aligned_allocator<U, OA> &);

      T *address(T &r) const;
      const T *address(const T &s) const;

      size_t max_size() const;

      bool operator!=(const aligned_allocator &other) const;

      void construct(T *const p, const T &t) const;
      void destroy(T *const p) const;

      // Returns true if and only if storage allocated from *this
      // can be deallocated from other, and vice versa.
      // Always returns true for stateless allocators.
      bool operator==(const aligned_allocator &) const;

      // The following will be different for each allocator.
      T *allocate(const size_t n) const;
      void deallocate(T *const p, const size_t n) const;

      template <typename U>
      T *allocate(const size_t n, const U * /* const hint */) const;
    };

    // Inlined member definitions /////////////////////////////////////////////

    template <typename T, int A>
    template <typename U, int OA>
    aligned_allocator<T, A>::aligned_allocator(const aligned_allocator<U, OA> &)
    {
    }

    template <typename T, int A>
    template <typename U, int OA>
    aligned_allocator<T, A> &aligned_allocator<T, A>::operator=(
        const aligned_allocator<U, OA> &)
    {
    }

    template <typename T, int A>
    inline T *aligned_allocator<T, A>::address(T &r) const
    {
      return &r;
    }

    template <typename T, int A>
    inline const T *aligned_allocator<T, A>::address(const T &s) const
    {
      return &s;
    }

    template <typename T, int A>
    inline size_t aligned_allocator<T, A>::max_size() const
    {
      // The following has been carefully written to be independent of
      // the definition of size_t and to avoid signed/unsigned warnings.
      return (static_cast<size_t>(0) - static_cast<size_t>(1)) / sizeof(T);
    }

    template <typename T, int A>
    inline bool aligned_allocator<T, A>::operator!=(
        const aligned_allocator &other) const
    {
      return !(*this == other);
    }

    template <typename T, int A>
    inline void aligned_allocator<T, A>::construct(T *const p, const T &t) const
    {
      void *const pv = static_cast<void *>(p);
      new (pv) T(t);
    }

    template <typename T, int A>
    inline bool aligned_allocator<T, A>::operator==(
        const aligned_allocator &) const
    {
      return true;
    }

    template <typename T, int A>
    inline T *aligned_allocator<T, A>::allocate(const size_t n) const
    {
      if (n == 0)
        return nullptr;

      if (n > max_size()) {
        throw std::length_error(
            "aligned_allocator<T>::allocate() – Integer overflow.");
      }

      void *const pv = memory::alignedMalloc(n * sizeof(T), A);

      if (pv == nullptr)
        throw std::bad_alloc();

      return static_cast<T *>(pv);
    }

    template <typename T, int A>
    inline void aligned_allocator<T, A>::deallocate(T *const p,
                                                    const size_t) const
    {
      memory::alignedFree(p);
    }

    template <typename T, int A>
    template <typename U>
    inline T *aligned_allocator<T, A>::allocate(const size_t n, const U *) const
    {
      return allocate(n);
    }

    template <typename T, int A>
    inline void aligned_allocator<T, A>::destroy(T *const p) const
    {
      p->~T();
    }

  }  // namespace containers
}  // namespace rkcommon
