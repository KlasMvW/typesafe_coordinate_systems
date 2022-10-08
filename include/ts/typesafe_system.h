namespace ts
{
  template<typename ThisSystem, typename ParentSystem>
  struct System
  {
    static constexpr int level = ParentSystem::level + 1;
    using parent = ParentSystem;
    using system = ThisSystem;
  };

  struct Root
  {
    static constexpr int level = 0;
  };
} // namespace ts