#include "example.h"

napi_ref Example::constructor;

Example::Example() :
    m_env(nullptr),
    m_wrapper(nullptr)
{
}

Example::~Example() {
    napi_delete_reference(m_env, m_wrapper);
}

napi_value Example::Init(napi_env env, napi_value exports)
{
    std::vector<napi_property_descriptor> properties = {
        NAPI_METHOD_DESCRIPTOR(TimesTwo)
    };

    return napi_helper::Init(env, exports, Example::constructor, Example::New, "Example", properties);
}

napi_value Example::New(napi_env env, napi_callback_info info)
{
    auto func = [] (auto & fi)
    {
        return new Example();
    };

    return napi_helper::New<Example>(env, info, Example::constructor, func, 0);
}

napi_value Example::TimesTwo(napi_env env, napi_callback_info info)
{
    napi_helper_finfo<Example> finfo(env, info, 1);

    auto value = finfo.GetDouble(0);

    return finfo.Return(value * 2.0);
}
