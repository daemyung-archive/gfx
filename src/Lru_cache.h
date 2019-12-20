//
// This file is part of the "gfx" project
// See "LICENSE" for license information.
//

#include <cstdint>
#include <memory>
#include <optional>
#include <iterator>
#include <list>
#include <unordered_map>

namespace Gfx_lib {

//----------------------------------------------------------------------------------------------------------------------

template<typename T, uint32_t N = 256>
class Lru_cache final
{
public:
    Lru_cache() :
        priorities {},
        pool_ {}
    {
    }

    template<typename... Args>
    void emplace(Args&&... args)
    {
        emplace_(args...);
    }

    std::optional<T*> find(uint64_t key)
    {
        auto iter = pool_.find(key);

        if (std::end(pool_) != iter)
            return iter->second.get();

        return std::nullopt;
    }

    bool contains(uint64_t key) const
    {
        return std::end(pool_) != pool_.find(key);
    }

private:
    template<typename... Args>
    void emplace_(uint64_t key, Args&&... args)
    {
        assert(!contains(key));

        if (N == pool_.size()) {
            pool_.erase(priorities.back());
            priorities.pop_back();
        }

        pool_.emplace(key, std::make_unique<T>(args...));
        priorities.remove(key);
        priorities.push_front(key);
    }

private:
    std::list<uint64_t> priorities;
    std::unordered_map<uint64_t, std::unique_ptr<T>> pool_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
