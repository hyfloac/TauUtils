#pragma once

#define DELETE_COPY(TYPE) \
    public:                                                                    \
        inline TYPE(const TYPE& copy)            noexcept = delete;            \
        inline TYPE& operator=(const TYPE& copy) noexcept = delete

#define DELETE_MOVE(TYPE) \
    public:                                                    \
        inline TYPE(TYPE&& move)            noexcept = delete; \
        inline TYPE& operator=(TYPE&& move) noexcept = delete

#define DELETE_CM(TYPE) \
    DELETE_COPY(TYPE); \
    DELETE_MOVE(TYPE)

#define DEFAULT_COPY(TYPE) \
    inline TYPE(const TYPE& copy)            noexcept = default; \
    inline TYPE& operator=(const TYPE& copy) noexcept = default

#define DEFAULT_MOVE(TYPE) \
    inline TYPE(TYPE&& move)            noexcept = default; \
    inline TYPE& operator=(TYPE&& move) noexcept = default

#define DEFAULT_CM(TYPE) \
    DEFAULT_COPY(TYPE); \
    DEFAULT_MOVE(TYPE)

#define DEFAULT_COPY_PU(TYPE) public:    DEFAULT_COPY(TYPE)
#define DEFAULT_COPY_PI(TYPE) private:   DEFAULT_COPY(TYPE)
#define DEFAULT_COPY_PO(TYPE) protected: DEFAULT_COPY(TYPE)

#define DEFAULT_MOVE_PU(TYPE) public:    DEFAULT_MOVE(TYPE)
#define DEFAULT_MOVE_PI(TYPE) private:   DEFAULT_MOVE(TYPE)
#define DEFAULT_MOVE_PO(TYPE) protected: DEFAULT_MOVE(TYPE)

#define DEFAULT_CM_PU(TYPE)   public:    DEFAULT_CM(TYPE)
#define DEFAULT_CM_PI(TYPE)   private:   DEFAULT_CM(TYPE)
#define DEFAULT_CM_PO(TYPE)   protected: DEFAULT_CM(TYPE)

#define DELETE_DESTRUCT(TYPE)     public: inline         ~TYPE() noexcept = delete
#define DEFAULT_DESTRUCT(TYPE)    public: inline         ~TYPE() noexcept = default
#define DEFAULT_DESTRUCT_VI(TYPE) public: inline virtual ~TYPE() noexcept = default

#define DELETE_CONSTRUCT(TYPE) private: inline TYPE() noexcept = delete
#define DEFAULT_CONSTRUCT(TYPE) inline TYPE() noexcept = default
#define DEFAULT_CONSTRUCT_PU(TYPE) public:    DEFAULT_CONSTRUCT(TYPE)
#define DEFAULT_CONSTRUCT_PI(TYPE) private:   DEFAULT_CONSTRUCT(TYPE)
#define DEFAULT_CONSTRUCT_PO(TYPE) protected: DEFAULT_CONSTRUCT(TYPE)

#define DECL_OPAQUE_TYPE(TYPE)                      \
    struct TYPE final {                             \
        DEFAULT_CONSTRUCT_PU(TYPE);                 \
        DEFAULT_DESTRUCT(TYPE);                     \
        DEFAULT_CM_PU(TYPE);                        \
    public:                                         \
        void* raw;                                  \
    public:                                         \
        _TYPE(void* const _raw) noexcept            \
            : raw(_raw)                             \
        { }                                         \
        template<typename T>                        \
        [[nodiscard]] T* get() noexcept             \
        { return reinterpret_cast<T*>(raw); }       \
        template<typename T>                        \
        [[nodiscard]] const T* get() const noexcept \
        { return reinterpret_cast<T*>(raw); }       \
    }

namespace tau {

struct TIPDefault final { constexpr TIPDefault() noexcept = default; };
struct TIPRecommended final { constexpr TIPRecommended() noexcept = default; };

static constexpr TIPDefault Default { };
static constexpr const TIPDefault& def = Default;
static constexpr TIPRecommended Recommended { };
static constexpr const TIPRecommended& rec = Recommended;

}
