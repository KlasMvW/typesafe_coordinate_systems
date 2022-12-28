namespace ts
{
  template<typename ThisSystem, typename ParentSystem>
  struct TypesafeSystem
  {
    static constexpr int level = ParentSystem::level + 1;
    using Parent = ParentSystem;
    using System = ThisSystem;
  };

  struct Root
  {
    static constexpr int level = 0;
  };
} // namespace ts