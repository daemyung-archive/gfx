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
        history_ {},
        pool_ {}
    {
    }

    template<typename... Args>
    void emplace(uint64_t key, Args&&... args)
    {
        assert(!contains(key));

        if (N == pool_.size()) {
            pool_.erase(history_.back());
            history_.pop_back();
        }

        pool_.emplace(key, std::make_unique<T>(args...));
        history_.remove(key);
        history_.push_front(key);
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

    void clear()
    {
        pool_.clear();
        history_.clear();
    }

private:
    std::list<uint64_t> history_;
    std::unordered_map<uint64_t, std::unique_ptr<T>> pool_;
};

//----------------------------------------------------------------------------------------------------------------------

} // of namespace Gfx_lib
