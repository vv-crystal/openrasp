#include <unordered_map>
#include <list>

namespace openrasp
{
using namespace std;

template <typename T, typename U>
class LRU
{
private:
  struct Item
  {
    Item(const T &k, const U &v) : key(k), value(v) {}
    T key;
    U value;
  };
  list<Item> item_list;
  unordered_map<T, typename list<Item>::iterator> item_map;
  size_t max_size;

  void reorder(const typename list<Item>::iterator it)
  {
    if (it == item_list.begin() && size() > max_size)
    {
      item_map.erase(item_list.back().key);
      item_list.pop_back();
    }
    else
    {
      item_list.splice(item_list.begin(), item_list, it);
    }
  }

public:
  LRU(size_t size = 10) : max_size(size) {}

  typename list<Item>::iterator get(const T &key)
  {
    auto it = item_map.find(key);
    if (it != item_map.end())
    {
      reorder(it->second);
      return it->second;
    }
    else
    {
      return item_list.end();
    }
  }
  void set(const T &key, const U &value)
  {
    auto it = item_map.find(key);
    if (it != item_map.end())
    {
      if (it->second->value != value)
      {
        it->second->value = value;
      }
      reorder(it->second);
    }
    else
    {
      item_list.emplace_front(key, value);
      auto it = item_list.begin();
      item_map.emplace(key, it);
      reorder(it);
    }
  }

  bool contains(const T &key) const
  {
    return item_map.find(key) != item_map.end();
  }
  bool empty() const
  {
    return item_list.empty();
  }
  size_t size() const
  {
    return item_list.size();
  }
  typename list<Item>::iterator begin()
  {
    return item_list.begin();
  }
  typename list<Item>::iterator end()
  {
    return item_list.end();
  }
};
} // namespace openrasp