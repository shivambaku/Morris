#ifndef EXAMPLE_H_
#define EXAMPLE_H_

#include "napi_helper.h"

class Example
{
public:                                                                            
    static napi_value Init(napi_env env, napi_value exports);                       
    static napi_value New(napi_env env, napi_callback_info info);
    static napi_value TimesTwo(napi_env env, napi_callback_info info);

private:
    explicit Example();
    ~Example();

private:
    static napi_ref constructor;
    napi_env m_env;
    napi_ref m_wrapper;

    friend class napi_helper;
};

#endif
