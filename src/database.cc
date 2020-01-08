/*******************************************************************************
 * Copyright (c) 2018 Nicola Del Gobbo
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the license at http://www.apache.org/licenses/LICENSE-2.0
 *
 * THIS CODE IS PROVIDED ON AN *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS
 * OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY
 * IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE,
 * MERCHANTABLITY OR NON-INFRINGEMENT.
 *
 * See the Apache Version 2.0 License for specific language governing
 * permissions and limitations under the License.
 *
 * Contributors - initial API implementation:
 * Nicola Del Gobbo <nicoladelgobbo@gmail.com>
 ******************************************************************************/

#include "database.h"
#include <iostream>

Napi::FunctionReference Database::constructor;

Napi::Object Database::Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, "Database", {
        InstanceMethod("open", &Database::Open),
        InstanceMethod("close", &Database::Close),
        InstanceMethod("size", &Database::Size),
        InstanceMethod("keys", &Database::Keys),
        InstanceMethod("get", &Database::Get),
        InstanceMethod("del", &Database::Del),
        InstanceMethod("put", &Database::Put),
        InstanceMethod("replace", &Database::Replace),
        InstanceMethod("exists", &Database::Exists)
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("Database", func);
    return exports;
}

Database::Database(const Napi::CallbackInfo& info) 
: Napi::ObjectWrap<Database>(info)
{
    // NOOP
}

Napi::Value Database::Open(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() != 1) {
        throw Napi::TypeError::New(env, "Wrong number of parameters");
    }
    if (!info[0].IsString()) {
        throw Napi::TypeError::New(env, "Parameters incorrect");
    }
    std::string name = info[0].As<Napi::String>();  
    bool ret = this->db.Attach("/dev/shm", name.c_str(), 0);
    return Napi::Boolean::New( env, ret );
}

Napi::Value Database::Close(const Napi::CallbackInfo& info) {
    if( this->db.IsAttached() ) {
       this->db.Detach();
    }
    return info.Env().Undefined();
}

Napi::Value Database::Size(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if( !this->db.IsAttached() ) {
        throw Napi::TypeError::New(env, "Database is not open");
    }
    return info.Env().Undefined();
}

Napi::Value Database::Keys(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if( !this->db.IsAttached() ) {
        throw Napi::TypeError::New(env, "Database is not open");
    }
    return info.Env().Undefined();
}

Napi::Value Database::Get(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if( !this->db.IsAttached() ) {
        throw Napi::TypeError::New(env, "Database is not open");
    }
    if (info.Length() != 1) {
        throw Napi::TypeError::New(env, "Wrong number of parameters");
    }
    if (!info[0].IsString()) {
        throw Napi::TypeError::New(env, "Parameter not a string");
    }
    std::string key = info[0].As<Napi::String>();
    db.MakeHash( key.c_str(), key.size() );

    bool found = db.GetKeyValCopy();
    if(!found) {
	return Napi::Boolean::New(env, false);
    }

    return Napi::String::New(env, shf_val, shf_val_len ); 
}

Napi::Value Database::Exists(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if( !this->db.IsAttached() ) {
        throw Napi::TypeError::New(env, "Database is not open");
    }
    if (info.Length() != 1) {
        throw Napi::TypeError::New(env, "Wrong number of parameters");
    }
    if (!info[0].IsString()) {
        throw Napi::TypeError::New(env, "Parameter not a string");
    }
    std::string key = info[0].As<Napi::String>();
    return Napi::Boolean::New( env, db.KeyExists( key.c_str(), key.size() ));
}
 
Napi::Value Database::Del(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if( !this->db.IsAttached() ) {
        throw Napi::TypeError::New(env, "Database is not open");
    }
    if (info.Length() != 1) {
        throw Napi::TypeError::New(env, "Wrong number of parameters");
    }
    if (!info[0].IsString()) {
        throw Napi::TypeError::New(env, "Parameter not a string");
    }
    std::string key = info[0].As<Napi::String>();
    db.MakeHash( key.c_str(), key.size() );
    return Napi::Boolean::New(env, this->db.DelKeyVal() ); 
}

Napi::Value Database::Put(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if( !this->db.IsAttached() ) {
        throw Napi::TypeError::New(env, "Database is not open");
    }
    if (info.Length() < 2 || info.Length()>3) {
        throw Napi::TypeError::New(env, "Wrong number of parameters");
    }
    if (!info[0].IsString() || !info[1].IsString()) {
        throw Napi::TypeError::New(env, "Parameters not a string");
    }
    uint32_t expires = 0;
    if (info.Length() == 3) {
        if(!info[2].IsNumber()) {
            throw Napi::TypeError::New(env, "Parameters expires not a number");
        }
        expires = info[2].As<Napi::Number>();
    }
    std::string key = info[0].As<Napi::String>();
    std::string val = info[1].As<Napi::String>();
    db.MakeHash( key.c_str(), key.size() );
    return Napi::Boolean::New(env, this->db.PutKeyVal( val.c_str(), val.size(), expires ) ); 
}

Napi::Value Database::Replace(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if( !this->db.IsAttached() ) {
        throw Napi::TypeError::New(env, "Database is not open");
    }
    if (info.Length() < 2 || info.Length()>3) {
        throw Napi::TypeError::New(env, "Wrong number of parameters");
    }
    if (!info[0].IsString() || !info[1].IsString()) {
        throw Napi::TypeError::New(env, "Parameters not a string");
    }
    uint32_t expires = 0;
    if (info.Length() == 3) {
        if(!info[2].IsNumber()) {
            throw Napi::TypeError::New(env, "Parameters expires not a number");
        }
        expires = info[2].As<Napi::Number>();
    }
    std::string key = info[0].As<Napi::String>();
    std::string val = info[1].As<Napi::String>();
    db.MakeHash( key.c_str(), key.size() );
    return Napi::Boolean::New(env, this->db.ReplaceKeyVal( val.c_str(), val.size(), expires ) ); 
}
