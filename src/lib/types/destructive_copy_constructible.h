namespace StackOverflow
{

    template <typename T>
    struct DestructiveCopyConstructible
    {
        mutable T value;

        DestructiveCopyConstructible() {}

        DestructiveCopyConstructible(T &&v) : value(std::move(v)) {}

        DestructiveCopyConstructible(const DestructiveCopyConstructible<T> &rhs)
            : value(std::move(rhs.value))
        {
        }

        DestructiveCopyConstructible(DestructiveCopyConstructible<T> &&rhs) = default;

        DestructiveCopyConstructible &operator=(const DestructiveCopyConstructible<T> &rhs) = delete;

        DestructiveCopyConstructible &operator=(DestructiveCopyConstructible<T> &&rhs) = delete;
    };

    template <typename T>
    using DCC_T =
        DestructiveCopyConstructible<typename std::remove_reference<T>::type>;

    template <typename T>
    inline DCC_T<T> MoveToDcc(T &&r)
    {
        return DCC_T<T>(std::move(r));
    }

} // namespace StackOverflow