module;

#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

export module Factory;
 
export 
template <typename TProduct, typename TId = std::string, typename TCreator = std::function<std::unique_ptr<TProduct>()>>
class GenericFactory
{
    std::unordered_map<TId, TCreator> creators_;

public:
    bool register_creator(TId id, TCreator creator)
    {
        bool is_inserted;
        std::tie(std::ignore, is_inserted) = creators_.emplace(std::move(id), std::move(creator));

        return is_inserted;
    }

    std::unique_ptr<TProduct> create(const TId& id) const
    {
        auto& creator = creators_.at(id);

        return creator();
    }
};