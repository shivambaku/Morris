#ifndef NAPI_HELPER_H_
#define NAPI_HELPER_H_

#include <node_api.h>
#include <array>
#include <assert.h>
#include <string>
#include <vector>
#include <functional>

#define NAPI_METHOD_DECLARATION(name) \
static napi_value name(napi_env env, napi_callback_info info);

#define NAPI_METHOD(class, name) \
napi_value class ::name(napi_env env, napi_callback_info info)

#define NAPI_METHOD_DESCRIPTOR(name)            \
{                                               \
    #name, 0, name, 0, 0, 0, napi_default, 0    \
}

#define NAPI_PROPERTY_DESCRIPTOR(name, getter, setter)  \
{                                                       \
    name, 0, 0, getter, setter, 0, napi_default, 0      \
}

template <class C>
struct napi_helper_finfo;

class napi_helper
{
public:
    static napi_value Init(napi_env env, napi_value exports, napi_ref & constructor, napi_callback new_function, std::string const & name, std::vector<napi_property_descriptor> const & properties)
    {
        napi_status status;
        napi_value cons;
        status = napi_define_class(env, name.c_str(), NAPI_AUTO_LENGTH, new_function,
                                     nullptr, properties.size(), &properties[0],
                                     &cons);
        assert(status == napi_ok);
        status = napi_create_reference(env, cons, 1, &constructor);
        assert(status == napi_ok);
        status = napi_set_named_property(env, exports, name.c_str(), cons);
        assert(status == napi_ok);
        return exports;
    }

    template <class T>
    static napi_value New(napi_env env, napi_callback_info info, napi_ref & constructor, std::function<T*(napi_helper_finfo<T>&)> const & func, int argc)
    {
        napi_helper_finfo<T> finfo(env, info, argc, true);

        napi_value target;
        auto status = napi_get_new_target(env, info, &target);
        assert(status == napi_ok);
        bool is_constructor = target != nullptr;

        if (is_constructor)
        {
            T *obj = func(finfo);

            obj->m_env = env;

            status = napi_wrap(env,
                            finfo.JSThis(),
                            reinterpret_cast<void *>(obj),
                            [](napi_env env, void *nativeObject, void *) {
                                    reinterpret_cast<T *>(nativeObject)->~T();
                            },
                            nullptr,
                            &obj->m_wrapper);
            assert(status == napi_ok);

            return finfo.JSThis();
        }
        else
        {
            napi_value cons;
            status = napi_get_reference_value(env, constructor, &cons);
            assert(status == napi_ok);

            napi_value instance;
            status = napi_new_instance(env, cons, finfo.Count(), finfo.Arguments(), &instance);
            assert(status == napi_ok);

            return instance;
        }

        return finfo.Return();
    }
};

template <class C>
struct napi_helper_finfo
{
  public:
    napi_helper_finfo(napi_env env, napi_callback_info inf, size_t n = 0, bool constructor = false)
    {
        m_env = env;
        m_info = inf;
        m_argc = n;
        m_argv = m_argc != 0 ? new napi_value[m_argc] : nullptr;

        napi_status status;
        status = napi_get_cb_info(m_env, m_info, &m_argc, m_argv, &m_jsthis, nullptr);
        assert(status == napi_ok);

        if (constructor == false)
        {
            status = napi_unwrap(env, m_jsthis, reinterpret_cast<void **>(&m_this));
            assert(status == napi_ok);
        }
    }

    ~napi_helper_finfo()
    {
        if (m_argv != nullptr)
        {
            delete[] m_argv;
        }
    }

    napi_value GetValue(int i)
    {
        return m_argv[i];
    }

    std::string GetString(int i)
    {
        return GetString<256>(m_argv[i]);
    }

    std::string GetString(napi_value jValue)
    {
        return GetString<256>(jValue);
    }

    template <size_t size>
    std::string GetString(napi_value jValue)
    {
        char value[size];
        size_t len;
        auto status = napi_get_value_string_utf8(m_env, jValue, (char *)&value, size, &len);
        assert(status == napi_ok);
        return std::string(value);
    }

    double GetDouble(napi_value jValue)
    {
        double value;
        auto status = napi_get_value_double(m_env, jValue, &value);
        assert(status == napi_ok);
        return value;
    }

    double GetDouble(int i)
    {
        return GetDouble(m_argv[i]);
    }

    uint32_t GetUInt(napi_value jValue)
    {
        uint32_t  value;
        auto status = napi_get_value_uint32(m_env, jValue, &value);
        assert(status == napi_ok);
        return value;
    }

    uint32_t GetUInt(int i)
    {
        return GetUInt(m_argv[i]);
    }

    int32_t GetInt32(int i)
    {
        int32_t value;
        auto status = napi_get_value_int32(m_env, m_argv[i], &value);
        assert(status == napi_ok);
        return value;
    }

    int64_t GetInt64(int i)
    {
        int64_t value;
        auto status = napi_get_value_int64(m_env, m_argv[i], &value);
        assert(status == napi_ok);
        return value;
    }

    napi_value Return(int value)
    {
        napi_value jsValue;
        auto status = napi_create_int32(m_env, value, &jsValue);
        assert(status == napi_ok);
        return jsValue;
    }

    napi_value Return(unsigned int value)
    {
        napi_value jsValue;
        auto status = napi_create_uint32(m_env, value, &jsValue);
        assert(status == napi_ok);
        return jsValue;
    }

    napi_value Return(double value)
    {
        napi_value jsValue;
        auto status = napi_create_double(m_env, value, &jsValue);
        assert(status == napi_ok);
        return jsValue;
    }

    napi_value Return(std::string value)
    {
        napi_value jsValue;
        auto status = napi_create_string_utf8(m_env, value.c_str(), value.length(), &jsValue);
        assert(status == napi_ok);
        return jsValue;
    }

    template<typename T>
    napi_value Return(std::vector<T> const & vector)
    {
        auto jArray = CreateArray(vector.size());
        for (unsigned i = 0; i < vector.size(); ++i)
        {
            SetElement(jArray, i, Return(vector[i]));
        }
        return jArray;
    }

    template<typename T, size_t S>
    napi_value Return(std::array<T, S> const & array)
    {
        auto jArray = CreateArray(array.size());
        for (unsigned i = 0; i < array.size(); ++i)
        {
            SetElement(jArray, i, Return(array[i]));
        }
        return jArray;
    }

    napi_value Undefined() {
        napi_value jsValue;
        auto status = napi_get_undefined(m_env, &jsValue);
        assert(status == napi_ok);
        return jsValue;
    }

    napi_value Return()
    {
        return nullptr;
    }

    napi_value CreateArray(size_t length)
    {
        napi_value jsValue;
        auto status = napi_create_array_with_length(m_env, length, &jsValue);
        assert(status == napi_ok);
        return jsValue;
    }

    napi_value CreateObject()
    {
        napi_value jsValue;
        auto status = napi_create_object(m_env, &jsValue);
        assert(status == napi_ok);
        return jsValue;
    }

    void SetElement(napi_value object, uint32_t index, napi_value value)
    {
        auto status = napi_set_element(m_env, object, index, value);
        assert(status == napi_ok);
    }

    void SetProperty(napi_value object, std::string const & name, napi_value value)
    {
        auto status = napi_set_named_property(m_env, object, name.c_str(), value);
        assert(status == napi_ok);
    }

    napi_value GetElement(napi_value object, uint32_t index)
    {
        napi_value jsValue;
        auto status = napi_get_element(m_env, object, index, &jsValue);
        assert(status == napi_ok);
        return jsValue;
    }

    napi_value GetProperty(napi_value object, std::string const & name)
    {
        napi_value jsValue;
        auto status = napi_get_named_property(m_env, object, name.c_str(), &jsValue);
        assert(status == napi_ok);
        return jsValue;
    }

    C * This()
    {
        return m_this;
    }

    napi_value JSThis()
    {
        return m_jsthis;
    }

    napi_value * Arguments()
    {
        return m_argv;
    }

    size_t Count()
    {
        return m_argc;
    }

  private:
    napi_value * m_argv;
    size_t m_argc;
    napi_env m_env;
    napi_callback_info m_info;
    C * m_this;
    napi_value m_jsthis;
};

#endif
